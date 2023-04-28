#include "sendwindow.h"
#include <QDebug>
#include <QStyle>
#include <QProxyStyle>

SendWindow::SendWindow(QWidget *parent) : MainWindow(parent) {
    setWindowTitle("Interactive Whiteboard - Send");

    QProxyStyle style;

    toolbar = addToolBar("main toolbar");
    QAction *action_clear_canvas = toolbar->addAction(style.standardIcon(QStyle::SP_FileIcon), "Clear Canvas");

    connect(action_clear_canvas, &QAction::triggered, this, &SendWindow::clear_canvas);
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

void SendWindow::on_mouse_down(int mouse_x, int mouse_y) {
    switch(drawing_mode) {
    case DMNone:
        qDebug() << "on_mouse_down() called on DMNone";
        break;
    case DMStroke:
        last_mouse_x = mouse_x;
        last_mouse_y = mouse_y;
        drawing = true;
        break;
    }
}

void SendWindow::on_mouse_up(int mouse_x, int mouse_y) {
    switch(drawing_mode) {
    case DMNone:
        qDebug() << "on_mouse_up() called on DMNone";
        break;
    case DMStroke:
        drawing = false;
        break;
    }
}

void SendWindow::on_mouse_move(int mouse_x, int mouse_y) {
    switch(drawing_mode) {
    case DMNone:
        qDebug() << "on_mouse_move() called on DMNone";
        break;
    case DMStroke:
        if(!drawing) break;

        Line* l = new Line(last_mouse_x, last_mouse_y, mouse_x, mouse_y);
        // SERIALISE AND SEND TO IO THREAD
        canvas_objects.push_back(l);

        last_mouse_x = mouse_x;
        last_mouse_y = mouse_y;
        update();
        break;
    }
}

void SendWindow::clear_canvas() {
    for(CanvasObject *c : canvas_objects) {
        delete c;
    }
    canvas_objects.clear();
    // CLEAR CANVAS PACKET
    update();
}
