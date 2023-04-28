#include "receivewindow.h"

#include <QDebug>

ReceiveWindow::ReceiveWindow(QWidget *parent, queue<uint8_t*> *data_queue) : MainWindow(parent), receive_queue(data_queue)
{
    setWindowTitle("Interactive Whiteboard - Receive");
}

void ReceiveWindow::empty_queue() {
    qDebug() << "Queue length " << receive_queue->size();

    while(!receive_queue->empty()) {
        uint8_t *packet = receive_queue->front();
        receive_queue->pop();

        //CHECK CHECKSUM!!

        switch(*packet) {
        case PTClearScreen:
            clear_canvas();
            break;
        case PTLine:
            canvas_objects.push_back(new Line(packet));
            break;
        }

        delete packet;
    }

    update();
}
