#include "canvasobject.h"

#include <QDebug>

// CanvasObject

CanvasObject::CanvasObject() { /* Empty method */ }

void CanvasObject::draw(QPainter *painter, QPen *pen) { /* Empty method */ }
byte *CanvasObject::serialise() { /* Empty method */ }

// Line

Line::Line(i16 xi1, i16 yi1, i16 xi2, i16 yi2, QPen pen) : x1(xi1), y1(yi1), x2(xi2), y2(yi2) {
    QColor c = pen.color();
    r = static_cast<u8>(c.red());
    g = static_cast<u8>(c.green());
    b = static_cast<u8>(c.blue());
    width = static_cast<u8>(pen.width());
}

Line::Line(byte *packet) {
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

byte *Line::serialise() {
    PacketLine *packet = new (PacketLine){
        PTLine,                     //type
        x1, y1,                     //start
        x2, y2,                     //end
        r, g, b, width,             //colour/width
        0,                          //checksum default value
    };
    return (byte*)packet;
}

// Rect

Rect::Rect(i16 xi1, i16 yi1, i16 xi2, i16 yi2, QPen pen) : x1(xi1), y1(yi1), x2(xi2), y2(yi2) {
    QColor c = pen.color();
    r = static_cast<u8>(c.red());
    g = static_cast<u8>(c.green());
    b = static_cast<u8>(c.blue());
    width = static_cast<u8>(pen.width());
}

Rect::Rect(byte *packet) {
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

byte *Rect::serialise() {
    PacketRect *packet = new (PacketRect){
        PTRect,                     //type
        x1, y1,                     //corner 1
        x2, y2,                     //corner 2
        r, g, b, width,             //colour/width
        0,                          //checksum default value
    };
    return (byte*)packet;
}

// Circle

Circle::Circle(i16 xi, i16 yi, i16 radi, QPen pen) : x(xi), y(yi), rad(radi) {
    QColor c = pen.color();
    r = static_cast<u8>(c.red());
    g = static_cast<u8>(c.green());
    b = static_cast<u8>(c.blue());
    width = static_cast<u8>(pen.width());
}

Circle::Circle(byte *packet) {
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

byte *Circle::serialise() {
    PacketCircle *packet = new (PacketCircle){
        PTCircle,                     //type
        x, y,                       //centre
        rad,                        //radius
        r, g, b, width,             //colour/width
        0,                          //checksum default value
    };
    return (byte*)packet;
}

// Commands

byte *serialise_clear_screen() {
    PacketClearScreen *packet = new (PacketClearScreen){
        PTClearScreen,  //type
        0,              //checksum default value
    };
    return (byte*)packet;
}

void core1send(byte *packet) {
    //gen_checksum(&packet); //fill checksum field
    //send packet
    //confirm sent
    delete packet;
}
