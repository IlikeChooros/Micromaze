#ifndef _Button_h
#define _Button_h

#include <Arduino.h>

void button_do_nothing(void);

class Button
{
    uint8_t _pin;
    uint8_t _state;
    bool _pressed;
    uint8_t _lastState;
    uint32_t _lastDebounceTime;

    uint8_t readState();

    void (*_on_press)();
    void (*_on_release)();

public:
    Button(uint8_t pin);
    void _init_();
    uint8_t state();
    void read();
    void reset();

    void on_press(void(*)(void));
    void on_release(void(*)(void));
};

#endif