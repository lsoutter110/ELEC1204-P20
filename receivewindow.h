#ifndef RECEIVEWINDOW_H
#define RECEIVEWINDOW_H

#include "mainwindow.h"

class ReceiveWindow : public MainWindow
{
    Q_OBJECT

public:
    ReceiveWindow(QWidget *parent = nullptr, queue<uint8_t*> *data_queue = nullptr);

protected:
    queue<uint8_t*> *receive_queue;

    //TEMPORARY FOR 3.1
private slots:
    void empty_queue();
    //=================
};

#endif // RECEIVEWINDOW_H
