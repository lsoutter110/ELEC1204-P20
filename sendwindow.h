#ifndef SENDWINDOW_H
#define SENDWINDOW_H

#include "receivewindow.h"
#include <QMouseEvent>
#include <QObject>
#include <QToolBar>

enum DrawingMode {
    DMNone,
    DMStroke,
    DMEraser,
};

class SendWindow : public MainWindow
{
    Q_OBJECT

public:
    SendWindow(QWidget *parent = nullptr);
    ~SendWindow();

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
//    void mouseMoveEvent(QMouseEvent *event);
    void on_mouse_down(int mouse_x, int mouse_y);
    void on_mouse_up(int mouse_x, int mouse_y);
    void on_mouse_move(int mouse_x, int mouse_y);

private:
    bool drawing = false;
    int last_mouse_x;
    int last_mouse_y;
    DrawingMode drawing_mode = DMStroke;

    ReceiveWindow *receive_window;
    QToolBar *toolbar;

    queue<uint8_t*> send_queue;

private slots:
    void slot_clear_canvas();

    //TEMPORARY FOR 3.1
    void test_recv();
signals:
    void items_in_queue();
    //=================
};

#endif // SENDWINDOW_H
