#ifndef _POINT_H
#define _POINT_H

#include "Arduino.h"

class Point
{
    public:
    uint8_t x;
    uint8_t y;
    Point(uint8_t x, uint8_t y);
};

#endif