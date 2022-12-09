#ifndef OPTIONS_H
#define OPTIONS_H

#include <TFT_eSPI.h> 
#include <SPI.h>
#include <math.h>

class Options
{
    public:
    explicit Options(uint8_t index, TFT_eSPI *tft, void(*)(void));
    virtual void draw() = 0;
    virtual void action(bool clicked, bool on_cursor) = 0;
    void (*_on_action)();

    protected:
    uint8_t _index;
    TFT_eSPI *_tft;
};


#endif