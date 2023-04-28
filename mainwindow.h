#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// STD INCLUDES
#include <vector>
using namespace std;

// QT INCLUDES
#include <QMainWindow>

// USER INCLUDES
#include "canvasobject.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void paintEvent(QPaintEvent *event);
    void drawToCanvas(vector<CanvasObject> canvas_objects);

protected:
    vector<CanvasObject*> canvas_objects;

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
