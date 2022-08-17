#ifndef _POINT_H
#define _POINT_H

#include "Arduino.h"

class Point
{
    public:
    int16_t x;
    int16_t y;
    Point(int16_t = 0, int16_t = 0);
};

#endif