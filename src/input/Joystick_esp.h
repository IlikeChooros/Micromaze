#ifndef _JOYSTICK
#define _JOYSTICK

#include "Arduino.h"

class Joystick
{
    uint8_t _angalog_x;
    uint8_t _angalog_y;

    uint8_t _ignore_val;

    uint64_t time;
    uint16_t interval_time;

    void (*_on_left)();
    void (*_on_right)();
    void (*_on_up)();
    void (*_on_down)();
    public:
    Joystick(uint8_t analog_x, uint8_t analog_y);
    void _init_();
    void read();
    void on_dir_left(void(*)(void));
    void on_dir_right(void(*)(void));
    void on_dir_up(void(*)(void));
    void on_dir_down(void(*)(void));
    void set_interval(uint16_t interval);
};


#endif