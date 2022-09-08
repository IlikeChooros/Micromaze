#ifndef VISUAL_H
#define VISUAL_H

#include <TFT_eSPI.h> 
#include <SPI.h>
#include <math.h>

#include "../data-sturctures/Point_extended.h"

TFT_eSPI tft = TFT_eSPI();

HSV text_color = {100, 100, 100};

uint32_t convert_to_RGB(uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t RGB;
    r = (r>>3);
    g = (g>>2);
    b = (b>>3);

    RGB = RGB | (r<<11);
    RGB = RGB | (g<<5);
    RGB = RGB | (b);

    return RGB;
}

uint32_t HSV_to_RGB(HSV color_hsv)
{
    float s = color_hsv.satrutaion/100;
    float v = color_hsv.value/100;

    float C = s*v;
    float X = C * ( 1 - abs(fmod(color_hsv.hue/60,2) - 1));
    float m = v - C; 

    float r,g,b;

    if (color_hsv.hue < 60)
    {
        r = C, g=X, b=0;
    }
    else if (color_hsv.hue<120)
    {
        r=X, g=C, b=0;
    }    
    else if (color_hsv.hue<180)
    {
        r=0, g=C, b=X;
    }    
    else if (color_hsv.hue<240)
    {
        r=0, g=X, b=C;
    }    
    else if (color_hsv.hue<300)
    {
        r=X, g=0, b=C;
    }    
    else
    {
        r=C, g=0, b=X;
    }

    return convert_to_RGB(
        (r+m)*255,
        (g+m)*255,
        (b+m)*255
    );
}

void gradient_letters(const char str [], uint8_t starting_hue, float itr_val)
{
    text_color.hue = starting_hue;
    uint8_t index=0;
    while(str[index]!='0')
    {
        tft.print(str[index]);
        text_color.hue+=itr_val;
        text_color.hue = fmod(text_color.hue,360);
        tft.setTextColor(HSV_to_RGB(text_color));
        index++;
    }
    text_color.hue = starting_hue;
}

void fading_effect(const char str [], uint8_t size ,HSV starting_color, uint8_t itr)
{
    uint8_t hue = starting_color.hue;
    uint8_t i=0;
    while (i<size)
    {
        tft.setTextColor(HSV_to_RGB(starting_color));
        tft.print(str[i]);
        if (i<size/2-1)
        {
            starting_color.hue += itr;
        }
        else{
            starting_color.hue -= itr;
        }
        i++;
    }
    starting_color.hue = hue;
}

void introduction()
{
    tft.setCursor(80,0);
    tft.fillScreen(TFT_BLACK);
    tft.setTextFont(2);
    tft.setRotation(1);
    tft.setTextSize(3);

    HSV fading_color = {200,100,100};
    fading_effect("MAZE 2D", 8, fading_color, 20);
    tft.println("");
    tft.println("");
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE);
    tft.print(" Press Joystick to start...");
}


#endif