#ifndef RECEIVEWINDOW_H
#define RECEIVEWINDOW_H

#include "mainwindow.h"

#define TIMER_PERIOD 20

class ReceiveWindow : public MainWindow
{
    Q_OBJECT

public:
    ReceiveWindow(QWidget *parent = nullptr,
                  queue<uint8_t*> *data_queue = nullptr,
                  mutex *data_mutex = nullptr);

    void timerEvent(QTimerEvent *event);

protected:
    queue<uint8_t*> *receive_queue;
    mutex *receive_mutex;
};

#endif // RECEIVEWINDOW_H
