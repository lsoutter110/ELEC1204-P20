#include "canvasobject.h"

#include <QDebug>

// CanvasObject

CanvasObject::CanvasObject() { /* Empty method */ }

void CanvasObject::draw(QPainter *painter) { /* Empty method */ }

// Line

Line::Line(int xi1, int yi1, int xi2, int yi2) : x1(xi1), y1(yi1), x2(xi2), y2(yi2) { /* Empty method */}

void Line::draw(QPainter *painter) {
    painter->drawLine(x1, y1, x2, y2);
}
