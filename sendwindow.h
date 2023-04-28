#ifndef SENDWINDOW_H
#define SENDWINDOW_H

#include "mainwindow.h"
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
public:
    SendWindow(QWidget *parent = nullptr);

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

    QToolBar *toolbar;

private slots:
    void clear_canvas();
};

#endif // SENDWINDOW_H
