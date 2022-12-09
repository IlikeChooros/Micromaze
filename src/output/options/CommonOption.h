#ifndef COMMON_OPTION_H
#define COMMON_OPTION_H

#include "Options.h"

class CommonOption: public Options
{
    const char* _option_name;
    uint16_t _color;
    public:
    CommonOption(uint8_t index, TFT_eSPI* tft, void(*on_action)(void),const char option_name [], uint16_t color): Options(index, tft, on_action)
    {
        this->_option_name = option_name;
    }

    void draw();
    void action(bool on_action, bool on_cursor);
};

#endif