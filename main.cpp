#include "sendwindow.h"
#include "receivewindow.h"
#include "gpio_controller.h"
#include <QApplication>

#include <thread>
#include <iostream>
using namespace std;

struct Args {
    bool loopback;
};

Args parse_args(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    Args args = parse_args(argc, argv);
    QApplication a(argc, argv);

    queue<byte*> send_queue, receive_queue;
    mutex send_mutex, receive_mutex;

    SendWindow s(nullptr, &send_queue, &send_mutex);
    ReceiveWindow r(nullptr, &receive_queue, &receive_mutex);

    //launch thread with gpio_controller
    thread gpio_thread(args.loopback?gpio_controller_loopback:gpio_controller, &send_queue, &receive_queue, &send_mutex, &receive_mutex);

    s.show();
    r.show();

    return a.exec();
}

Args parse_args(int argc, char *argv[]) {
    Args args = {false};
    for(int i=0; i<argc; i++) {
        if(strcmp(argv[i], "--loopback")==0) args.loopback = true;
    }
    return args;
}
