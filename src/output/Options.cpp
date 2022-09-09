#include "Options.h"

Options::Options(TFT_eSPI *tft)
{
    this->_tft = tft;
}

void Options::_init_(uint8_t number_of_normal_opt)
{
    this->number_of_opt = number_of_normal_opt;

    normal_options = 0;
    if (number_of_normal_opt>0)
    {
        normal_options = new Option[number_of_normal_opt];
    }
    normal_opt_idx = 0;
}

void Options::create_option(uint8_t layer,uint8_t option_idx, const char option_name[], uint8_t size, bool marked)
{
    normal_options[normal_opt_idx].layer = layer;
    normal_options[normal_opt_idx].option_name = option_name;
    normal_options[normal_opt_idx].option_idx = option_idx;
    normal_options[normal_opt_idx].size = size;
    normal_options[normal_opt_idx].marked = marked;

    uint8_t i=0;
    while(option_name[i] != NULL)
    {
        i++;
    }

    normal_options[normal_opt_idx].str_len = i-1;
    normal_opt_idx++;
}

uint8_t Options::get_num_of_options_in_layer(int8_t layer)
{
    uint8_t counter=0;
    for (uint8_t i=0;i<normal_opt_idx;i++)
    {
        if (normal_options[i].layer == layer)
        {
            counter++;
        }
    }
    return counter;
}

void Options::draw(uint8_t layer, uint8_t current_option_idx)
{
    uint8_t idx;
    for (uint8_t i=0;i<normal_opt_idx;i++)
    {
        if(normal_options[i].layer == layer)
        {
            idx = i;
            break;
        }
    }

    _tft->setRotation(1);
    _tft->setTextColor(TFT_WHITE);

    if (normal_options[idx].size > 2)
    {
        _tft->setTextSize(normal_options[idx].size);
    }
    bool t=false;
    uint16_t y=40;

    if (layer == 0)
    {
        y+= 40;
    }
    while(normal_options[idx].layer == layer)
    {
        if (normal_options[idx].option_idx == current_option_idx)
        {
            _tft->setTextColor(TFT_GREEN);
            t = true;
        }
        
        
        _tft->setCursor(calculate_starting_x(normal_options[idx].size, normal_options[idx].str_len), y);
        y += calculate_y(normal_options[idx].size);
        _tft->print(normal_options[idx].option_name);
        if (normal_options[idx].marked)
        {
            _tft->print("  X");
        }

        if (t)
        {
            _tft->setTextColor(TFT_WHITE);
        }
        
        idx++;
        if (idx == normal_opt_idx)
        {
            break;
        }
    }

    _tft->setTextSize(1);
    _tft->setRotation(0);
}

void Options::set_mark(bool mark, uint8_t opt_idx, uint8_t layer)
{
    for (uint8_t i=0;i<normal_opt_idx;i++)
    {
        if (normal_options[i].layer == layer && normal_options[i].option_idx == opt_idx)
        {
            normal_options[i].marked = mark;
        }
    }
}

uint16_t Options::calculate_starting_x(uint8_t text_size, uint8_t str_size)
{
    float max_char = _tft->width()/text_size;
    max_char /= 6;

    // Serial.println("STR_SIZE: "+String(str_size));
    // Serial.println("TEXT_SIZE: "+String(text_size)+ " max_num_of_char = "+String(max_char));
    return (round(max_char) - str_size)*3*text_size;
}

uint16_t Options::calculate_y(uint8_t text_size)
{
    return text_size*9+10;
}