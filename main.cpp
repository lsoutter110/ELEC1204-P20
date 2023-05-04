#include "sendwindow.h"
#include "receivewindow.h"
#include <QApplication>

#include <thread>
#include <iostream>
using namespace std;

struct Args {
    bool loopback;
};

Args parse_args(int argc, char *argv[]);
void gpio_controller_loopback(queue<uint8_t*> *send_queue, queue<uint8_t*> *receive_queue, mutex *send_mutex, mutex *receive_mutex);
void gpio_controller(queue<uint8_t*> *send_queue, queue<uint8_t*> *receive_queue, mutex *send_mutex, mutex *receive_mutex);

int main(int argc, char *argv[])
{
    Args args = parse_args(argc, argv);
    QApplication a(argc, argv);

    queue<uint8_t*> send_queue, receive_queue;
    mutex send_mutex, receive_mutex;

    SendWindow s(nullptr, &send_queue, &send_mutex);
    ReceiveWindow r(nullptr, &receive_queue, &receive_mutex);

    //launch thread with gpio_controller
    thread gpio_thread(args.loopback?gpio_controller_loopback:gpio_controller, &send_queue, &receive_queue, &send_mutex, &receive_mutex);

    s.show();
    r.show();

    return a.exec();
}

void gpio_controller_loopback(queue<uint8_t*> *send_queue, queue<uint8_t*> *receive_queue, mutex *send_mutex, mutex *receive_mutex) {
    while(1) {
        if(send_queue->empty()) continue;

        //wait for lock on send
        send_mutex->lock();
        //get front packet
        uint8_t *packet = send_queue->front();
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

void gpio_controller(queue<uint8_t*> *send_queue, queue<uint8_t*> *receive_queue, mutex *send_mutex, mutex *receive_mutex) {

}

Args parse_args(int argc, char *argv[]) {
    Args args = {false};
    for(int i=0; i<argc; i++) {
        if(strcmp(argv[i], "--loopback")==0) args.loopback = true;
    }
    return args;
}
