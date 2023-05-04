#ifndef CANVASOBJECT_H
#define CANVASOBJECT_H

// STD INCLUDES
#include <vector>
#include <cstdint>
using namespace std;

// QT INCLUDES
#include <QPainter>

// USER INCLUDES

enum PacketType : uint8_t {
    PTClearScreen,
    PTLine,
    PTRect,
    PTCircle,
    PTImage,
};

struct PacketClearScreen {
    PacketType type;
    uint16_t checksum;
};

struct PacketLine {
    PacketType type;
    int16_t x1, y1, x2, y2;
    uint8_t r, g, b;
    uint8_t width;
    uint16_t checksum;
};

struct PacketRect {
    PacketType type;
    int16_t x1, y1, x2, y2;
    uint8_t r, g, b;
    uint8_t width;
    uint16_t checksum;
};

struct PacketCircle {
    PacketType type;
    int16_t x, y;
    int16_t rad;
    uint8_t r, g, b;
    uint8_t width;
    uint16_t checksum;
};

const uint32_t PACKET_SIZE_INDEX[] = {sizeof(PacketClearScreen), sizeof(PacketLine)};

class CanvasObject
{
public:

    CanvasObject();
    virtual ~CanvasObject() {}

    virtual void draw(QPainter *painter, QPen *pen);
    virtual uint8_t *serialise();
};

class Line : public CanvasObject {
public:
    int16_t x1, y1;
    int16_t x2, y2;
    uint8_t r, g, b;
    uint8_t width;

    Line(int16_t xi1, int16_t yi1, int16_t xi2, int16_t yi2, QPen pen);
    Line(uint8_t *packet);

    void draw(QPainter *painter, QPen *pen) override;
    uint8_t *serialise() override;
};

class Rect : public CanvasObject {
public:
    int16_t x1, y1;
    int16_t x2, y2;
    uint8_t r, g, b;
    uint8_t width;

    Rect(int16_t xi1, int16_t yi1, int16_t xi2, int16_t yi2, QPen pen);
    Rect(uint8_t *packet);

    void draw(QPainter *painter, QPen *pen) override;
    uint8_t *serialise() override;
};

class Circle : public CanvasObject {
public:
    int16_t x, y;
    int16_t rad;
    uint8_t r, g, b;
    uint8_t width;

    Circle(int16_t xi, int16_t yi, int16_t radi, QPen pen);
    Circle(uint8_t *packet);

    void draw(QPainter *painter, QPen *pen) override;
    uint8_t *serialise() override;
};

uint8_t *serialise_clear_screen();

#endif // CANVASOBJECT_H
