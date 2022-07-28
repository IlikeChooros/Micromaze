#ifndef _MATRIX_MAP
#define _MATRIX_MAP

#include <TFT_eSPI.h> 
#include <SPI.h>
#include "Arduino.h"

class Matrix_map
{
    uint8_t *_map;
    uint8_t number_of_rows_map;
    uint8_t number_of_col_map;
    uint32_t _colour;
    TFT_eSPI *_tft;

    uint32_t converToRGB(uint8_t r, uint8_t g, uint8_t b); 

    public:
    Matrix_map(TFT_eSPI *tft);
    void draw_map();
    void set_map(uint8_t matrix_map[],uint8_t row, uint8_t col, uint8_t r, uint8_t g, uint8_t b);
    uint8_t* get_current_map();
    uint8_t get_current_map_rows();
    uint8_t get_current_map_cols();

};

#endif