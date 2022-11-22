#ifndef HSV_RGB_H
#define HSV_RGB_H

struct HSV
{
    float hue;
    float satrutaion;
    float value;
};

uint32_t inline convert_to_RGB(uint8_t r, uint8_t g, uint8_t b)
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

uint32_t inline HSV_to_RGB_ray_cast(HSV color_hsv, uint8_t dist, uint8_t ray_lenght)
{
    float s = color_hsv.satrutaion/100;
    float v = color_hsv.value/100;

    v = v* (ray_lenght + 1 - dist)/(ray_lenght+1);

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

uint32_t inline HSV_to_RGB(HSV color_hsv)
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


#endif