#include "canvasobject.h"

#include <QDebug>

// CanvasObject

CanvasObject::CanvasObject() { /* Empty method */ }

void CanvasObject::draw(QPainter *painter, QPen *pen) { /* Empty method */ }
uint8_t *CanvasObject::serialise() { /* Empty method */ }

// Line

Line::Line(int16_t xi1, int16_t yi1, int16_t xi2, int16_t yi2, QPen pen) : x1(xi1), y1(yi1), x2(xi2), y2(yi2) {
    QColor c = pen.color();
    r = static_cast<uint8_t>(c.red());
    g = static_cast<uint8_t>(c.green());
    b = static_cast<uint8_t>(c.blue());
    width = static_cast<uint8_t>(pen.width());
}

Line::Line(uint8_t *packet) {
    PacketLine *pl = (PacketLine*) packet;
    x1 = pl->x1;
    y1 = pl->y1;
    x2 = pl->x2;
    y2 = pl->y2;
    r = pl->r;
    g = pl->g;
    b = pl->b;
    width = pl->width;
}

void Line::draw(QPainter *painter, QPen *pen) {
    pen->setColor({r, g, b});
    pen->setWidth(width);
    painter->setPen(*pen);

    painter->drawLine(x1, y1, x2, y2);
}

uint8_t *Line::serialise() {
    PacketLine *packet = new (PacketLine){
        PTLine,                     //type
        x1, y1,                     //start
        x2, y2,                     //end
        r, g, b, width,             //colour/width
        0,                          //checksum default value
    };
    return (uint8_t*)packet;
}

// Rect

Rect::Rect(int16_t xi1, int16_t yi1, int16_t xi2, int16_t yi2, QPen pen) : x1(xi1), y1(yi1), x2(xi2), y2(yi2) {
    QColor c = pen.color();
    r = static_cast<uint8_t>(c.red());
    g = static_cast<uint8_t>(c.green());
    b = static_cast<uint8_t>(c.blue());
    width = static_cast<uint8_t>(pen.width());
}

Rect::Rect(uint8_t *packet) {
    PacketRect *pl = (PacketRect*) packet;
    x1 = pl->x1;
    y1 = pl->y1;
    x2 = pl->x2;
    y2 = pl->y2;
    r = pl->r;
    g = pl->g;
    b = pl->b;
    width = pl->width;
}

void Rect::draw(QPainter *painter, QPen *pen) {
    pen->setColor({r, g, b});
    pen->setWidth(width);
    painter->setPen(*pen);

    painter->drawLine(x1, y1, x2, y1);
    painter->drawLine(x2, y1, x2, y2);
    painter->drawLine(x2, y2, x1, y2);
    painter->drawLine(x1, y2, x1, y1);
}

uint8_t *Rect::serialise() {
    PacketRect *packet = new (PacketRect){
        PTRect,                     //type
        x1, y1,                     //corner 1
        x2, y2,                     //corner 2
        r, g, b, width,             //colour/width
        0,                          //checksum default value
    };
    return (uint8_t*)packet;
}

// Circle

Circle::Circle(int16_t xi, int16_t yi, int16_t radi, QPen pen) : x(xi), y(yi), rad(radi) {
    QColor c = pen.color();
    r = static_cast<uint8_t>(c.red());
    g = static_cast<uint8_t>(c.green());
    b = static_cast<uint8_t>(c.blue());
    width = static_cast<uint8_t>(pen.width());
}

Circle::Circle(uint8_t *packet) {
    PacketCircle *pl = (PacketCircle*) packet;
    x = pl->x;
    y = pl->y;
    rad = pl->rad;
    r = pl->r;
    g = pl->g;
    b = pl->b;
    width = pl->width;
}

void Circle::draw(QPainter *painter, QPen *pen) {
    pen->setColor({r, g, b});
    pen->setWidth(width);
    painter->setPen(*pen);

    painter->drawEllipse(x-rad, y-rad, 2*rad, 2*rad);
}

uint8_t *Circle::serialise() {
    PacketCircle *packet = new (PacketCircle){
        PTCircle,                     //type
        x, y,                       //centre
        rad,                        //radius
        r, g, b, width,             //colour/width
        0,                          //checksum default value
    };
    return (uint8_t*)packet;
}

// Commands

uint8_t *serialise_clear_screen() {
    PacketClearScreen *packet = new (PacketClearScreen){
        PTClearScreen,  //type
        0,              //checksum default value
    };
    return (uint8_t*)packet;
}

void core1send(uint8_t *packet) {
    //gen_checksum(&packet); //fill checksum field
    //send packet
    //confirm sent
    delete packet;
}
