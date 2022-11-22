#ifndef _POINT_H
#define _POINT_H

#include "Arduino.h"

enum Dir{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Point
{
    public:
    uint8_t x;
    uint16_t y;
    Point(uint8_t = 0, uint16_t = 0);
};

#endif