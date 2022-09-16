#include "Matrix_map.h"

Matrix_map::Matrix_map(TFT_eSPI *tft)
{
    _tft = tft;
}


void Matrix_map::draw_map() // FOR 2D ONLY
{
    // if (!(_map&&number_of_col_map&&number_of_rows_map))
    // {
    //     return;
    // }
    uint16_t x_pointer;
    uint16_t y_pointer;
    uint16_t width_of_segment = _tft->width()/number_of_col_map; // 240
    uint16_t height_of_segment = _tft->height()/number_of_rows_map; //320

    //Serial.println("H: "+String(tft.height())+" , W:"+String(tft.width())+" H seg:"+String(height_of_segment)+" W seg:"+String(width_of_segment));
    for (uint16_t y=0;y<number_of_rows_map;y++)
    {
        for (uint16_t x=0;x<number_of_col_map;x++)
        {
            x_pointer = x*width_of_segment;
            y_pointer = y*height_of_segment;
            if (_map[y*number_of_col_map+x]==2)
            {
                _tft->fillRect(x_pointer, y_pointer,width_of_segment,height_of_segment,converToRGB(219, 147, 2));
            }
            else if (_map[y*number_of_col_map + x])
            {
                _tft->fillRect(x_pointer, y_pointer,width_of_segment,height_of_segment,_colour);
            }
        }
    }
}

void Matrix_map::set_map(uint8_t map[],uint16_t row, uint16_t col, uint8_t r, uint8_t g, uint8_t b)
{
    _map=map;
    number_of_col_map = col;
    number_of_rows_map = row;
    _colour = converToRGB(r,g,b);
}

uint8_t* Matrix_map::get_current_map()
{
    return _map;
}

uint16_t Matrix_map::get_current_map_cols()
{
    return number_of_col_map;
}

uint16_t Matrix_map::get_current_map_rows()
{
    return number_of_rows_map;
}

//pirvate

uint32_t Matrix_map::converToRGB(uint8_t r, uint8_t g, uint8_t b)
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