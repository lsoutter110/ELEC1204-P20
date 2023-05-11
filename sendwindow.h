#ifndef SENDWINDOW_H
#define SENDWINDOW_H

#include "mainwindow.h"

#include <QMouseEvent>
#include <QObject>
#include <QToolBar>
#include <QColorDialog>
#include <QPen>

using namespace std;

enum DrawingMode {
    DMNone,
    DMStroke,
    DMEraser,
    DMLine,
    DMRect,
    DMCircle,
    DMImage,
};

class SendWindow : public MainWindow
{
    Q_OBJECT

public:
    SendWindow(QWidget *parent = nullptr,
               queue<byte*> *data_queue = nullptr,
               mutex *data_mutex = nullptr);
    ~SendWindow();

//    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void on_mouse_down(i16 mouse_x, i16 mouse_y);
    void on_mouse_up(i16 mouse_x, i16 mouse_y);
    void on_mouse_move(i16 mouse_x, i16 mouse_y);

private:
    bool drawing = false;
    i16 last_mouse_x, last_mouse_y;
    DrawingMode drawing_mode = DMNone;
    QPen drawing_pen = QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap);
    u8 eraser_rad = 10;
    QString selected_image;

    QToolBar *toolbar;
    QColorDialog *col_dialog;

    queue<byte*> *send_queue;
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
    void set_tool_image();
};

#endif // SENDWINDOW_H
