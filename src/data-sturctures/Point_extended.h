#ifndef _POINT_EXTENDED_HSV
#define _POINT_EXTENDED_HSV

#include "Arduino.h"

struct Point_extended
{
    uint16_t x;
    uint16_t y;

    float fl_x;
    float fl_y;
};

struct HSV
{
    float hue;
    float satrutaion;
    float value;
};


#endif