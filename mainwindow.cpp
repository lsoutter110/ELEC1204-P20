#include "mainwindow.h"
#include "canvasobject.h"

#include <QPainter>
#include <QPen>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    resize(800, 600);
}

MainWindow::~MainWindow() {
    for(CanvasObject* c : canvas_objects) {
        delete c;
    }
}

void MainWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QPen pen;

    for(auto c : canvas_objects) {
        c->draw(&painter, &pen);
    }
}

void MainWindow::clear_canvas() {
    for(CanvasObject *c : canvas_objects) {
        delete c;
    }
    canvas_objects.clear();

    update();
}
