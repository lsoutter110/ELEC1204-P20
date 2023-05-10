#ifndef CANVASOBJECT_H
#define CANVASOBJECT_H

// STD INCLUDES
#include <vector>
#include <cstdint>
//#include <initializer_list>
using namespace std;

// QT INCLUDES
#include <QPainter>

// USER INCLUDES
#include "util.h"

enum PacketType : u8 {
    PTClearScreen,
    PTLine,
    PTRect,
    PTCircle,
    PTImage,
};

struct PacketClearScreen {
    PacketType type;
    u16 checksum;
};

struct PacketLine {
    PacketType type;
    i16 x1, y1, x2, y2;
    u8 r, g, b;
    u8 width;
    u16 checksum;
};

struct PacketRect {
    PacketType type;
    i16 x1, y1, x2, y2;
    u8 r, g, b;
    u8 width;
    u16 checksum;
};

struct PacketCircle {
    PacketType type;
    i16 x, y;
    i16 rad;
    u8 r, g, b;
    u8 width;
    u16 checksum;
};

const size_t PACKET_SIZE_INDEX[] = {sizeof(PacketClearScreen), sizeof(PacketLine), sizeof(PacketRect), sizeof(PacketCircle)};

class CanvasObject
{
public:

    CanvasObject();
    virtual ~CanvasObject() {}

    virtual void draw(QPainter *painter, QPen *pen);
    virtual byte *serialise();
};

class Line : public CanvasObject {
public:
    i16 x1, y1;
    i16 x2, y2;
    u8 r, g, b;
    u8 width;

    Line(i16 xi1, i16 yi1, i16 xi2, i16 yi2, QPen pen);
    Line(byte *packet);

    void draw(QPainter *painter, QPen *pen) override;
    byte *serialise() override;
};

class Rect : public CanvasObject {
public:
    i16 x1, y1;
    i16 x2, y2;
    u8 r, g, b;
    u8 width;

    Rect(i16 xi1, i16 yi1, i16 xi2, i16 yi2, QPen pen);
    Rect(byte *packet);

    void draw(QPainter *painter, QPen *pen) override;
    byte *serialise() override;
};

class Circle : public CanvasObject {
public:
    i16 x, y;
    i16 rad;
    u8 r, g, b;
    u8 width;

    Circle(i16 xi, i16 yi, i16 radi, QPen pen);
    Circle(byte *packet);

    void draw(QPainter *painter, QPen *pen) override;
    byte *serialise() override;
};

byte *serialise_clear_screen();

#endif // CANVASOBJECT_H
