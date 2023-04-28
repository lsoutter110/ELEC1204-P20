#ifndef CANVASOBJECT_H
#define CANVASOBJECT_H

// STD INCLUDES
#include <vector>
using namespace std;

// QT INCLUDES
#include <QPainter>

//struct Line {
//    int x1;
//    int x2;
//    int y1;
//    int y2;
//};

class CanvasObject
{
public:

    CanvasObject();

    virtual void draw(QPainter *painter);
    //CanvasObject(BINDATA);
    //BINDATA serialise();
};

class Line : public CanvasObject {
public:
    int x1, y1;
    int x2, y2;

    Line(int xi1, int yi1, int xi2, int yi2);
    void draw(QPainter *painter) override;

};

#endif // CANVASOBJECT_H
