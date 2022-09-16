#ifndef _POINT_H
#define _POINT_H

#include "Arduino.h"

class Point
{
    public:
    uint16_t x;
    uint16_t y;
    Point(uint16_t = 0, uint16_t = 0);
};

#endif