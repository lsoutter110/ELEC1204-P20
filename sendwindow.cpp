#include "sendwindow.h"
#include <QDebug>
#include <QStyle>
#include <QProxyStyle>

SendWindow::SendWindow(QWidget *parent, queue<byte*> *data_queue, mutex *data_mutex)
    : MainWindow(parent), send_queue(data_queue), send_mutex(data_mutex) {
    setWindowTitle("Interactive Whiteboard - Send");

    // Toolbar setup

    QProxyStyle style;

    toolbar = addToolBar("main toolbar");
    QAction *action_clear_canvas = toolbar->addAction(style.standardIcon(QStyle::SP_FileIcon), "Clear Canvas");
    QAction *action_colour_picker = toolbar->addAction(QIcon("icons/colour_picker.png"), "Pick Colour");
    toolbar->addSeparator();
    QAction *action_set_tool_none = toolbar->addAction(style.standardIcon(QStyle::SP_FileIcon), "No Tool");
    QAction *action_set_tool_stroke = toolbar->addAction(style.standardIcon(QStyle::SP_FileIcon), "Pen Tool");
    QAction *action_set_tool_eraser = toolbar->addAction(style.standardIcon(QStyle::SP_FileIcon), "Eraser");
    toolbar->addSeparator();
    QAction *action_set_tool_line = toolbar->addAction(QIcon("icons/line.png"), "Line");
    QAction *action_set_tool_rect = toolbar->addAction(QIcon("icons/rect.png"), "Rectangle");
    QAction *action_set_tool_circle = toolbar->addAction(QIcon("icons/circle.png"), "Circle");

    connect(action_clear_canvas, &QAction::triggered, this, &SendWindow::slot_clear_canvas);
    connect(action_colour_picker, &QAction::triggered, this, &SendWindow::colour_picker);

    connect(action_set_tool_none, &QAction::triggered, this, &SendWindow::set_tool_none);
    connect(action_set_tool_stroke, &QAction::triggered, this, &SendWindow::set_tool_stroke);
    connect(action_set_tool_eraser, &QAction::triggered, this, &SendWindow::set_tool_eraser);

    connect(action_set_tool_line, &QAction::triggered, this, &SendWindow::set_tool_line);
    connect(action_set_tool_rect, &QAction::triggered, this, &SendWindow::set_tool_rect);
    connect(action_set_tool_circle, &QAction::triggered, this, &SendWindow::set_tool_circle);
}

SendWindow::~SendWindow() {
//    delete receive_window;
}

void SendWindow::mousePressEvent(QMouseEvent *event) {
    QPoint pos = event->pos();
    if(event->button() == Qt::LeftButton) {
        on_mouse_down(pos.x(), pos.y());
//        qDebug() << "left mouse press";
    }
}

void SendWindow::mouseReleaseEvent(QMouseEvent *event) {
    QPoint pos = event->pos();
    if(event->button() == Qt::LeftButton) {
        on_mouse_up(pos.x(), pos.y());
//        qDebug() << "left mouse release";
    }
}

void SendWindow::mouseMoveEvent(QMouseEvent *event) {
    QPoint pos = event->pos();
    on_mouse_move(pos.x(), pos.y());
//    qDebug() << "mouse move";
}

void SendWindow::on_mouse_down(int16_t mouse_x, int16_t mouse_y) {
    switch(drawing_mode) {
    case DMNone:
        qDebug() << "on_mouse_down() called on DMNone";
        break;

    case DMStroke:
        last_mouse_x = mouse_x;
        last_mouse_y = mouse_y;
        drawing = true;
        break;

    case DMEraser:
        break;

    case DMLine:
        drawing = true;

        last_mouse_x = mouse_x;
        last_mouse_y = mouse_y;

        canvas_objects.push_back(new Line(last_mouse_x, last_mouse_y, mouse_x, mouse_y, drawing_pen));
        break;

    case DMRect:
        drawing = true;

        last_mouse_x = mouse_x;
        last_mouse_y = mouse_y;

        canvas_objects.push_back(new Rect(last_mouse_x, last_mouse_y, mouse_x, mouse_y, drawing_pen));
        break;

    case DMCircle:
        drawing = true;

        last_mouse_x = mouse_x;
        last_mouse_y = mouse_y;

        canvas_objects.push_back(new Circle(last_mouse_x, last_mouse_y, 0, drawing_pen));
        break;
    }
}

void SendWindow::on_mouse_up(int16_t mouse_x, int16_t mouse_y) {
    switch(drawing_mode) {
    case DMNone:
        qDebug() << "on_mouse_up() called on DMNone";
        break;

    case DMStroke:
        drawing = false;
        break;

    case DMEraser:
        break;

    case DMLine:
    case DMRect:
    case DMCircle:
        send_mutex->lock();
        send_queue->push(canvas_objects.back()->serialise());
        send_mutex->unlock();
        break;
    }
}

void SendWindow::on_mouse_move(int16_t mouse_x, int16_t mouse_y) {
    switch(drawing_mode) {
    case DMNone:
        qDebug() << "on_mouse_move() called on DMNone";
        break;

    case DMStroke:
        if(!drawing) break;

        canvas_objects.push_back(new Line(last_mouse_x, last_mouse_y, mouse_x, mouse_y, drawing_pen));

        send_mutex->lock();
        send_queue->push(canvas_objects.back()->serialise());
        send_mutex->unlock();

        last_mouse_x = mouse_x;
        last_mouse_y = mouse_y;
        update();
        break;

    case DMEraser:
        break;

    case DMLine:
        if(!drawing) break;

        ((Line*)canvas_objects.back())->x2 = mouse_x;
        ((Line*)canvas_objects.back())->y2 = mouse_y;

        update();
        break;

    case DMRect:
        if(!drawing) break;

        ((Rect*)canvas_objects.back())->x2 = mouse_x;
        ((Rect*)canvas_objects.back())->y2 = mouse_y;

        update();
        break;

    case DMCircle:
        if(!drawing) break;

        ((Circle*)canvas_objects.back())->rad = distance(last_mouse_x, last_mouse_y, mouse_x, mouse_y);

        update();
        break;
    }
}

// SLOTS

void SendWindow::slot_clear_canvas() {
    clear_canvas();

    send_mutex->lock();
    send_queue->push(serialise_clear_screen());
    send_mutex->unlock();
}

void SendWindow::colour_picker() {
    col_dialog = new QColorDialog(drawing_pen.color());
    col_dialog->show();

    connect(col_dialog, &QColorDialog::colorSelected, this, &SendWindow::set_colour);
}

void SendWindow::set_colour() {
    drawing_pen.setColor(col_dialog->selectedColor());
}

void SendWindow::set_tool_none() {
    drawing_mode = DMNone;
}

void SendWindow::set_tool_stroke() {
    drawing_mode = DMStroke;
}

void SendWindow::set_tool_eraser() {
    drawing_mode = DMEraser;
}

void SendWindow::set_tool_line() {
    drawing_mode = DMLine;
}

void SendWindow::set_tool_rect() {
    drawing_mode = DMRect;
}

void SendWindow::set_tool_circle() {
    drawing_mode = DMCircle;
}
