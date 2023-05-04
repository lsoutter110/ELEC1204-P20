#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// STD INCLUDES
#include <vector>
#include <queue>
#include <mutex>

using namespace std;

// QT INCLUDES
#include <QMainWindow>

// USER INCLUDES
#include "canvasobject.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void paintEvent(QPaintEvent *event);

protected:
    vector<CanvasObject*> canvas_objects;
    void clear_canvas();
};

#endif // MAINWINDOW_H
