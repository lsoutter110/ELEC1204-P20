#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "canvasobject.h"

#include <QPainter>
#include <QPen>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
    for(CanvasObject* c : canvas_objects) {
        delete c;
    }
}

void MainWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));

    for(auto c : canvas_objects) {
        c->draw(&painter);
    }
}
