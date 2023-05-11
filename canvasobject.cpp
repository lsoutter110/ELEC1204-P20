#include "canvasobject.h"

#include <QDebug>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <cstdio>
using namespace std;

// CanvasObject

CanvasObject::CanvasObject() { /* Empty method */ }

void CanvasObject::draw(QPainter *painter, QPen *pen) { /* Empty method */ }
byte *CanvasObject::serialise() { /* Empty method */ }
bool CanvasObject::collide(i16 xi, i16 yi, u8 width) { /*Empty method */ }

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

bool Line::collide(i16 xi, i16 yi, u8 width) {
    i32 w32 = static_cast<i32>(width);
    if(distancesq<i32>(xi,yi,x1,y1) < w32*w32)
        return true;
    if(distancesq<i32>(xi,yi,x2,y2) < w32*w32)
        return true;
    i32 dx=x2-x1, dy=y2-y1;
    i32 l_top = (xi-x1)*dx + (yi-y1)*dy;
    if(l_top < 0) return false;
    i32 l_bot = dx*dx + dy*dy;
    if(l_top > l_bot) return false;
    float lam = l_top/(float)l_bot;
    float ax=xi-x1-lam*dx, ay=yi-y1-lam*dy;
    return ax*ax + ay*ay < w32*w32;
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

bool Rect::collide(i16 xi, i16 yi, u8 width) {
    return ((abs(x1-xi)<width || abs(x2-xi)<width) && (((y1<yi+width)&&(yi<y2+width)) || ((y2<yi+width)&&(yi<y1+width))))
        || ((abs(y1-yi)<width || abs(y2-yi)<width) && (((x1<xi+width)&&(xi<x2+width)) || ((x2<xi+width)&&(xi<x1+width))));
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

bool Circle::collide(i16 xi, i16 yi, u8 width) {
    return abs(distance(xi, yi, x, y)-rad)<width;
}

// Image

Image::Image(i16 xi1, i16 yi1, i16 xi2, i16 yi2, QString f) : x1(xi1), y1(yi1), x2(xi2), y2(yi2), filename(f) {
    cache = QImage(filename);
};

Image::Image(byte *packet) {
    //deserialise
    PacketImage *p = (PacketImage*) packet;
    x1 = p->x1;
    y1 = p->y1;
    x2 = p->x2;
    y2 = p->y2;
    size_t file_size = ((size_t)p->sizeh)<<16 | (size_t)p->sizel;
    byte *data = p->data;
    //create file
    char namebuf[L_tmpnam];
    tmpnam(namebuf);
    //write data to file
    ofstream ofile(namebuf, ios::binary);
    ofile.write((char*)data, file_size);
    ofile.close();
    delete data;
    //setup cache
    cache = QImage(QString(namebuf));
    //destroy temp file
    remove(namebuf);
}

void Image::draw(QPainter *painter, QPen *pen) {
    painter->drawImage(QRect(min(x1,x2), min(y1,y2), abs(x1-x2), abs(y1-y2)), cache);
}

byte *Image::serialise() {
    //load file and find size
    ifstream ifile(filename.toStdString(), ios::ate | ios::binary);
    size_t file_size = ifile.tellg();
    //read file
    char *data = (char*)malloc(file_size);
    ifile.seekg(0, ios::beg);
    ifile.read(data, file_size);
    ifile.close();
    //serialise packet
    PacketImage *packet = new (PacketImage){
        PTImage,
        x1, y1,
        x2, y2,
        (u16)(file_size>>16), (u16)file_size,
        0,
        (byte*)data,
    };
    return (byte*)packet;
}

bool Image::collide(i16 xi, i16 yi, u8 width) {
    return (((y1<yi+width)&&(yi<y2+width)) || ((y2<yi+width)&&(yi<y1+width)))
        && (((x1<xi+width)&&(xi<x2+width)) || ((x2<xi+width)&&(xi<x1+width)));
}

// Commands

byte *serialise_clear_screen() {
    PacketClearScreen *packet = new (PacketClearScreen){
        PTClearScreen,  //type
        0,              //checksum default value
    };
    return (byte*)packet;
}

byte *serialise_erase(u16 index) {
    PacketErase *packet = new (PacketErase){
        PTErase,  //type
        index,    //vector index
        0,        //checksum default value
    };
    return (byte*)packet;
}
