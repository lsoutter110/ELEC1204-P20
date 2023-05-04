#ifndef SENDWINDOW_H
#define SENDWINDOW_H

#include "mainwindow.h"

#include <QMouseEvent>
#include <QObject>
#include <QToolBar>
#include <QColorDialog>
#include <QPen>

#include <cmath>
using namespace std;

enum DrawingMode {
    DMNone,
    DMStroke,
    DMEraser,
    DMLine,
    DMRect,
    DMCircle,
};

class SendWindow : public MainWindow
{
    Q_OBJECT

public:
    SendWindow(QWidget *parent = nullptr,
               queue<uint8_t*> *data_queue = nullptr,
               mutex *data_mutex = nullptr);
    ~SendWindow();

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void on_mouse_down(int16_t mouse_x, int16_t mouse_y);
    void on_mouse_up(int16_t mouse_x, int16_t mouse_y);
    void on_mouse_move(int16_t mouse_x, int16_t mouse_y);

private:
    bool drawing = false;
    int16_t last_mouse_x, last_mouse_y;
    DrawingMode drawing_mode = DMNone;
    QPen drawing_pen = QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap);

    QToolBar *toolbar;
    QColorDialog *col_dialog;

    queue<uint8_t*> *send_queue;
    mutex *send_mutex;

private slots:
    void slot_clear_canvas();
    void colour_picker();
    void set_colour();
    void set_tool_none();
    void set_tool_stroke();
    void set_tool_eraser();
    void set_tool_line();
    void set_tool_rect();
    void set_tool_circle();
};

template<class T>
T distance(T x1, T y1, T x2, T y2) {
    T dx = x1-x2;
    T dy = y1-y2;
    return (T)sqrt(dx*dx + dy*dy);
}

#endif // SENDWINDOW_H
