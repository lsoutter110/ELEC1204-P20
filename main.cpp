#include "sendwindow.h"
#include "receivewindow.h"
#include <QApplication>

#include <thread>
using namespace std;

void gpio_controller(queue<uint8_t*> *send_queue, queue<uint8_t*> *receive_queue, mutex *send_mutex, mutex *receive_mutex);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    queue<uint8_t*> send_queue, receive_queue;
    mutex send_mutex, receive_mutex;

    SendWindow s(nullptr, &send_queue, &send_mutex);
    ReceiveWindow r(nullptr, &receive_queue, &receive_mutex);

    //launch thread with gpio_controller
    thread gpio_thread(gpio_controller, &send_queue, &receive_queue, &send_mutex, &receive_mutex);

    s.show();
    r.show();

    return a.exec();
}

void gpio_controller(queue<uint8_t*> *send_queue, queue<uint8_t*> *receive_queue, mutex *send_mutex, mutex *receive_mutex) {
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

