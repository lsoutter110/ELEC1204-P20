#ifndef GPIO_CONTROLLER_H
#define GPIO_CONTROLLER_H

#include <queue>
#include <mutex>
#include <wiringPi.h>
#include <iostream>
#include "canvasobject.h"
#include "util.h"

#define SEND_CONTROL 12
#define RECV_CONTROL 13
#define DATA_OK 15
#define DATA_ACK 16

using namespace std;

void gpio_controller_loopback[[noreturn]](queue<byte*> *send_queue, queue<byte*> *receive_queue, mutex *send_mutex, mutex *receive_mutex) {
    while(1) {
        if(send_queue->empty()) continue;

        //wait for lock on send
        send_mutex->lock();
        //get front packet
        byte *packet = send_queue->front();
        send_queue->pop();
        //unlock send
        send_mutex->unlock();

        //wait for lock on receive
        receive_mutex->lock();
        //push to queue
        receive_queue->push(packet);
        //unclock receive
        receive_mutex->unlock();
    }
}

void gpio_controller[[noreturn]](queue<byte*> *send_queue, queue<byte*> *receive_queue, mutex *send_mutex, mutex *receive_mutex) {
    //setup hardware
    wiringPiSetup();
    piHiPri(99);
    //control pins
    pinMode(SEND_CONTROL, OUTPUT);
    pinMode(RECV_CONTROL, INPUT);
    pinMode(DATA_OK, OUTPUT);
    pinMode(DATA_ACK, INPUT);
    //data pins
    for(int i=0; i<8; i++)
        pinMode(i, INPUT);

    //setup initial states
    digitalWrite(SEND_CONTROL, LOW);
    digitalWrite(DATA_OK, LOW);
    int data_ok_state = 0;

    //wait for initial reset state
    while(digitalRead(RECV_CONTROL));

    cout << "[GPIO] GPIO Controller initialised!\n";
    cout << "[GPIO] sizes: {";
    for(auto i : PACKET_SIZE_INDEX) cout << i << ", ";
    cout << "}\n";

    //packet buffer
//    byte *packet_buf = (byte*)malloc(max(PACKET_SIZE_INDEX));
    //dyn packet size: a pre-dyn packet will be sent before to set (eg image header)
//    size_t dyn_packet_size = 0;

    while(1) {
        //send
        if(!send_queue->empty()) {
//            cout << "[GPIO] Send mode\n";
            //grab first packet
            send_mutex->lock();
            byte *packet = send_queue->front();
            send_queue->pop();
            send_mutex->unlock();

            size_t packet_size = PACKET_SIZE_INDEX[packet[0]];
            if(packet[0] == PTImage) {
                //dynamic image packet - construct full packet
                PacketImage *pi = (PacketImage*)packet;
                size_t data_size = ((size_t)pi->sizeh)<<16 | (size_t)pi->sizel;
                byte *data = pi->data;
                //update packet_size
                packet_size = sizeof(PacketImage) - sizeof(byte*) + data_size;
                //realloc to full packet size
                packet = (byte*) realloc(packet, packet_size);
                //copy data into where data pointer was
                memcpy(packet + sizeof(PacketImage) - sizeof(byte*), data, data_size);
                delete data;
            }
//            cout << "[GPIO] Sending packet of type " << (u16)packet[0] << " and size " << packet_size << " bytes\n";

            //begin packet
            //pull SEND_CONTROL high
            digitalWrite(SEND_CONTROL, HIGH);
            //wait for RECV_CONTROL high
            while(!digitalRead(RECV_CONTROL));

            for(int i=0; i<8; i++)
                pinMode(i, OUTPUT);

            //send bytes
            for(size_t i=0; i<packet_size; i++) {
//                cout << "[GPIO] sending byte " << hex << (u16)packet[i] << dec << endl;
                //write data
                digitalWriteByte(packet[i]);
                //do not remove
                delayMicroseconds(5);

                //change DATA_OK
                data_ok_state = !data_ok_state;
                digitalWrite(DATA_OK, data_ok_state);

                //wait for DATA_ACK==DATA_OK
                while(digitalRead(DATA_ACK) != data_ok_state);
            }

            for(int i=0; i<8; i++)
                pinMode(i, INPUT);

            //end packet
            //pull SEND_CONTROL low
            digitalWrite(SEND_CONTROL, LOW);
            //wait for RECV_CONTROL low
            while(digitalRead(RECV_CONTROL));
//            cout << "[GPIO] Packet sent\n";

            //delete packet once sent
            delete packet;
        }

        //receive
        if(digitalRead(RECV_CONTROL)) {
            //cout << "[GPIO] Receive mode\n";
            //pull RECV_CONTROL high
            digitalWrite(SEND_CONTROL, HIGH);

            //wait for DATA_ACK!=DATA_OK
            while(digitalRead(DATA_ACK) == data_ok_state);
            //read byte, alloc accordingly
            int packet_type = digitalReadByte();
            size_t packet_size = PACKET_SIZE_INDEX[packet_type];
            //alignment requirement of 2 due to i16 types
            byte *packet = (byte*) aligned_alloc(2, packet_size);
            //cout << "Receiving packet of type " << (u16)packet_type << " and size " << packet_size << " bytes\n";

            for(size_t i=0; i<packet_size; i++) {
                //if dynamic image packet, and have full header
                if(i==(sizeof(PacketImage) - sizeof(byte*)) && packet_type==PTImage) {
                    //dynamic image packet - allocate for full packet
                    PacketImage *pi = (PacketImage*)packet;
                    size_t data_size = ((size_t)pi->sizeh)<<16 | (size_t)pi->sizel;
                    //update packet_size
                    packet_size = sizeof(PacketImage) - sizeof(byte*) + data_size;
                    //reallocate memory
                    packet = (byte*) realloc(packet, packet_size);
                }
                //wait for DATA_ACK!=DATA_OK
                while(digitalRead(DATA_ACK) == data_ok_state);
                //read byte
                packet[i] = (byte) digitalReadByte();
                //cout << "[GPIO] Received byte " << hex << (u16)packet[i] << dec << endl;
                //change DATA_OK
                data_ok_state = !data_ok_state;
                digitalWrite(DATA_OK, data_ok_state);
            }
            //wait for RECV_CONTROL low
            while(digitalRead(RECV_CONTROL));
            //pull SEND_CONTROL low
            digitalWrite(SEND_CONTROL, LOW);
            //cout  << "[GPIO] Packet received\n";

            //if dynamic image packet, reformat to expected format
            if(packet_type == PTImage) {
                PacketImage *pi = (PacketImage*)packet;
                size_t data_size = ((size_t)pi->sizeh)<<16 | (size_t)pi->sizel;
                //allocate data memory and copy over
                byte *data = (byte*) malloc(data_size);
                memcpy(data, packet + sizeof(PacketImage) - sizeof(byte*), data_size);
                //shrink original packet
                packet = (byte*) realloc(packet, sizeof(PacketImage));
                //set data pointer (cannot use pi due to realloc)
                ((PacketImage*)packet)->data = data;
            }

            //send to receive window
            receive_mutex->lock();
            receive_queue->push(packet);
            receive_mutex->unlock();
        }
    }
}

#endif // GPIO_CONTROLLER_H
