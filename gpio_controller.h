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
            cout << "[GPIO] Send mode\n";
            //grab first packet
            send_mutex->lock();
            byte *packet = send_queue->front();
            send_queue->pop();
            send_mutex->unlock();

            //insert dyn packet code (change packet_size and packet)
            size_t packet_size = PACKET_SIZE_INDEX[packet[0]];

            cout << "[GPIO] Sending packet of type " << (u16)packet[0] << " and size " << packet_size << " bytes\n";

            //begin packet
            //pull SEND_CONTROL high
            digitalWrite(SEND_CONTROL, HIGH);
            //wait for RECV_CONTROL high
            while(!digitalRead(RECV_CONTROL));

            for(int i=0; i<8; i++)
                pinMode(i, OUTPUT);

            //send bytes
            for(size_t i=0; i<packet_size; i++) {
                cout << "[GPIO] sending byte " << hex << (u16)packet[i] << dec << endl;
                //write data
                digitalWriteByte(packet[i]);

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

            //delete packet once sent
            delete packet;
        }

        //receive
        if(digitalRead(RECV_CONTROL)) {
            cout << "[GPIO] Receive mode\n";
            //pull RECV_CONTROL high
            digitalWrite(SEND_CONTROL, HIGH);

            //wait for DATA_ACK!=DATA_OK
            while(digitalRead(DATA_ACK) == data_ok_state);
            //read byte, alloc accordingly
            size_t packet_size = PACKET_SIZE_INDEX[digitalReadByte()];
            //alignment requirement of 2 due to i16 types
            byte *packet = (byte*) aligned_alloc(2, packet_size);

            for(size_t i=0; i<packet_size; i++) {
                //wait for DATA_ACK!=DATA_OK
                while(digitalRead(DATA_ACK) == data_ok_state);
                //read byte
                packet[i] = (byte) digitalReadByte();
                //change DATA_OK
                data_ok_state = !data_ok_state;
                digitalWrite(DATA_OK, data_ok_state);
            }
            //wait for RECV_CONRTOL low
            while(digitalRead(RECV_CONTROL));
            //pull SEND_CONTROL low
            digitalWrite(SEND_CONTROL, LOW);

            //send to receive window
            receive_mutex->lock();
            receive_queue->push(packet);
            receive_mutex->lock();
        }
    }
}

#endif // GPIO_CONTROLLER_H
