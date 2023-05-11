#include "sendwindow.h"
#include <QDebug>
#include <QStyle>
#include <QProxyStyle>
#include <QFileDialog>

SendWindow::SendWindow(QWidget *parent, queue<byte*> *data_queue, mutex *data_mutex)
    : MainWindow(parent), send_queue(data_queue), send_mutex(data_mutex) {
    setWindowTitle("Interactive Whiteboard - Send");

    // Toolbar setup

    QProxyStyle style;

    toolbar = addToolBar("main toolbar");
    QAction *action_clear_canvas = toolbar->addAction(style.standardIcon(QStyle::SP_FileIcon), "Clear Canvas");
    QAction *action_colour_picker = toolbar->addAction(QIcon("icons/colour_picker.png"), "Pick Colour");
    toolbar->addSeparator();
    QAction *action_set_tool_none = toolbar->addAction(QIcon("icons/mouse_icon.png"), "No Tool");
    QAction *action_set_tool_stroke = toolbar->addAction(QIcon("icons/pen_icon.png"), "Pen Tool");
    QAction *action_set_tool_eraser = toolbar->addAction(QIcon("icons/eraser_icon.png"), "Eraser");
    toolbar->addSeparator();
    QAction *action_set_tool_line = toolbar->addAction(QIcon("icons/line.png"), "Line");
    QAction *action_set_tool_rect = toolbar->addAction(QIcon("icons/rect.png"), "Rectangle");
    QAction *action_set_tool_circle = toolbar->addAction(QIcon("icons/circle.png"), "Circle");
    toolbar->addSeparator();
    QAction *action_set_tool_image = toolbar->addAction(QIcon("icons/image_icon.png"), "Image");

    connect(action_clear_canvas, &QAction::triggered, this, &SendWindow::slot_clear_canvas);
    connect(action_colour_picker, &QAction::triggered, this, &SendWindow::colour_picker);

    connect(action_set_tool_none, &QAction::triggered, this, &SendWindow::set_tool_none);
    connect(action_set_tool_stroke, &QAction::triggered, this, &SendWindow::set_tool_stroke);
    connect(action_set_tool_eraser, &QAction::triggered, this, &SendWindow::set_tool_eraser);

    connect(action_set_tool_line, &QAction::triggered, this, &SendWindow::set_tool_line);
    connect(action_set_tool_rect, &QAction::triggered, this, &SendWindow::set_tool_rect);
    connect(action_set_tool_circle, &QAction::triggered, this, &SendWindow::set_tool_circle);

    connect(action_set_tool_image, &QAction::triggered, this, &SendWindow::set_tool_image);
}

SendWindow::~SendWindow() {
//    delete receive_window;
}

//void SendWindow::paintEvent(QPaintEvent *event) {
//    MainWindow::paintEvent(event);

//    QPainter painter(this);
//    QPoint mousepos;
//    mapFromGlobal(mousepos);
//    int mouse_x = mousepos.x();
//    int mouse_y = mousepos.y();

//    //paint tool icons
//    switch(drawing_mode) {
//    case DMNone:
//    case DMLine:
//    case DMRect:
//    case DMCircle:
//        break;

//    case DMEraser:
//        painter.setPen(QPen(Qt::gray, 1, Qt::SolidLine, Qt::RoundCap));
//        painter.drawEllipse(mouse_x, mouse_y, eraser_rad, eraser_rad);
//        break;

//    case DMStroke:
//        painter.setPen(QPen(drawing_pen.color(), 1, Qt::SolidLine, Qt::RoundCap));
//        painter.drawEllipse(mouse_x, mouse_y, drawing_pen.width()/2, drawing_pen.width()/2);
//        break;
//    }
//}

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
        for(auto c=canvas_objects.begin(); c<canvas_objects.end(); ++c) {
            //check for collision
            if(!(*c)->collide(mouse_x, mouse_y, eraser_rad))
                continue;
            //delete object
            delete (*c);
            canvas_objects.erase(c);
            //send to queue
            send_mutex->lock();
            send_queue->push(serialise_erase((u16)(c-canvas_objects.begin())));
            send_mutex->unlock();
            //step backward to nullify ++c
            --c;
        }
        //refresh screen
        update();
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

    case DMImage:
        drawing = true;

        last_mouse_x = mouse_x;
        last_mouse_y = mouse_y;

        canvas_objects.push_back(new Image(last_mouse_x, last_mouse_y, mouse_x, mouse_y, selected_image));
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
    case DMImage:
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
        for(auto c=canvas_objects.begin(); c<canvas_objects.end(); ++c) {
            //check for collision
            if(!(*c)->collide(mouse_x, mouse_y, eraser_rad))
                continue;
            //delete object
            delete (*c);
            canvas_objects.erase(c);
            //send to queue
            send_mutex->lock();
            send_queue->push(serialise_erase((u16)(c-canvas_objects.begin())));
            send_mutex->unlock();
            //step backward to nullify ++c
            --c;
        }
        //refresh screen
        update();
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

    case DMImage:
        if(!drawing) break;

        ((Image*)canvas_objects.back())->x2 = mouse_x;
        ((Image*)canvas_objects.back())->y2 = mouse_y;

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

void SendWindow::set_tool_image() {
    drawing_mode = DMImage;
    //select image
    selected_image = QFileDialog::getOpenFileName(this, tr("Open Image"));
}
