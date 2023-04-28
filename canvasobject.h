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

const uint32_t PACKET_SIZE_INDEX[] = {sizeof(PacketClearScreen), sizeof(PacketLine)};

class CanvasObject
{
public:

    CanvasObject();
    virtual ~CanvasObject() {}

    virtual void draw(QPainter *painter);
    virtual uint8_t *serialise();
    //CanvasObject(BINDATA);
    //BINDATA serialise();
};

class Line : public CanvasObject {
public:
    int x1, y1;
    int x2, y2;
    uint8_t r, g, b;
    uint8_t width;

    Line(int xi1, int yi1, int xi2, int yi2);
    Line(uint8_t *packet);

    void draw(QPainter *painter) override;
    uint8_t *serialise() override;
};

uint8_t *serialise_clear_screen();

#endif // CANVASOBJECT_H
