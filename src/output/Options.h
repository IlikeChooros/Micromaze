#ifndef OPTIONS_H
#define OPTIONS_H

#include <TFT_eSPI.h> 
#include <SPI.h>
#include <math.h>

class Option
{
    public:
    uint8_t layer;
    const char* option_name;
    uint8_t str_len;
    uint8_t option_idx;
    uint8_t size;
    bool marked;
};

class Options
{
    TFT_eSPI *_tft;

    uint8_t number_of_opt;
    uint8_t normal_opt_idx;

    Option* normal_options;


    uint16_t calculate_starting_x(uint8_t text_size, uint8_t str_size);
    uint16_t calculate_y(uint8_t text_size);
    public:
    Options(TFT_eSPI *tft);
    void _init_(uint8_t number_of_all_options);
    void draw(uint8_t layer, uint8_t current_option_idx);
    void create_option(uint8_t layer,uint8_t option_idx, const char option_name[], uint8_t text_size, bool mark);
    void set_mark(bool mark, uint8_t opt_idx, uint8_t layer);
    uint8_t get_num_of_options_in_layer(int8_t layer);
};


#endif