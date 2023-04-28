#include "canvasobject.h"

#include <QDebug>

// CanvasObject

CanvasObject::CanvasObject() { /* Empty method */ }

void CanvasObject::draw(QPainter *painter) { /* Empty method */ }
uint8_t *CanvasObject::serialise() { /* Empty method */ }

// Line

Line::Line(int xi1, int yi1, int xi2, int yi2) : x1(xi1), y1(yi1), x2(xi2), y2(yi2) { /* Empty method */ }

Line::Line(uint8_t *packet) {
    PacketLine *pl = (PacketLine*) packet;
    x1 = (int) pl->x1;
    y1 = (int) pl->y1;
    x2 = (int) pl->x2;
    y2 = (int) pl->y2;
    r = pl->r;
    g = pl->g;
    b = pl->b;
    width = pl->width;
}

void Line::draw(QPainter *painter) {
    painter->drawLine(x1, y1, x2, y2);
}

uint8_t *Line::serialise() {
    PacketLine *packet = new (PacketLine){
        PTLine,                     //type
        (int16_t)x1, (int16_t)y1, //start
        (int16_t)x2, (int16_t)y2, //end
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
