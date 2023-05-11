#include "receivewindow.h"

#include <QDebug>

ReceiveWindow::ReceiveWindow(QWidget *parent, queue<byte*> *data_queue, mutex *data_mutex)
    : MainWindow(parent), receive_queue(data_queue), receive_mutex(data_mutex) {
    setWindowTitle("Interactive Whiteboard - Receive");

    startTimer(TIMER_PERIOD);
}

void ReceiveWindow::timerEvent(QTimerEvent *event) {
    while(!receive_queue->empty()) {
        //safely pop first packet off queue
        receive_mutex->lock();
        uint8_t *packet = receive_queue->front();
        receive_queue->pop();
        receive_mutex->unlock();

        //CHECK CHECKSUM!!

        switch(*packet) {
        case PTClearScreen:
            clear_canvas();
            break;
        case PTLine:
            canvas_objects.push_back(new Line(packet));
            break;
        case PTRect:
            canvas_objects.push_back(new Rect(packet));
            break;
        case PTCircle:
            canvas_objects.push_back(new Circle(packet));
            break;
        case PTImage:
            canvas_objects.push_back(new Image(packet));
            break;
        case PTErase:
            delete canvas_objects[((PacketErase*)packet)->index];
            canvas_objects.erase(canvas_objects.begin() + ((PacketErase*)packet)->index);
            break;
        }

        delete packet;
    }

    update();
}
