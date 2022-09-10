#include "Options.h"

Options::Options(TFT_eSPI *tft)
{
    this->_tft = tft;
}

void Options::_init_(uint8_t number_of_normal_opt, uint8_t num_of_sliders)
{
    this->number_of_opt = number_of_normal_opt;
    normal_options = 0;
    if (number_of_normal_opt>0)
    {
        normal_options = new Option[number_of_normal_opt];
    }
    normal_opt_idx = 0;
    slider_idx = 0;

    num_of_sliders = 0;
    if (num_of_sliders>0)
    {
        sliders = new Slider[num_of_sliders];
    }
}

void Options::draw_slider(uint8_t layer, uint8_t curr_sli_idx)
{
    uint8_t y,idx;
    for (uint8_t i=0;i<slider_idx;i++)
    {
        if (sliders[i].layer == layer)
        {
            idx = i;
            break;
        }
    }

    y = 0;

    if (sliders[idx].size > 2)
    {
        _tft->setTextSize(sliders[idx].size);
    }

    _tft->setRotation(1);
    _tft->setTextColor(TFT_WHITE);

    bool t=false;

    while(sliders[idx].layer == layer)
    {
        if (sliders[idx].option_idx == curr_sli_idx)
        {
            _tft->setTextColor(TFT_GREEN);
            t = true;
        }
        
        y = 40 + (sliders[idx].option_idx+1)*calculate_y(sliders[idx].size);
        _tft->setCursor(calculate_starting_x(sliders[idx].size, sliders[idx].str_len), y);
        _tft->print(sliders[idx].option_name);

        if (t)
        {
            _tft->setTextColor(TFT_WHITE);
        }
        
        idx++;
        if (idx == slider_idx)
        {
            break;
        }
    }

    _tft->setTextSize(1);
    _tft->setRotation(0);
}

void Options::create_slider(uint8_t layer, uint8_t option_idx, const char option_name[], uint8_t starting_num, uint8_t text_size, uint8_t itr)
{
    sliders[slider_idx].layer = layer;
    sliders[slider_idx].option_idx = option_idx;
    sliders[slider_idx].size = text_size;
    sliders[slider_idx].option_name = option_name;
    sliders[slider_idx].current_val = starting_num;
    sliders[slider_idx].itr = itr;

    uint8_t i=0;
    while(option_name[i] != NULL)
    {
        i++;
    }

    sliders[slider_idx].str_len = i-1;

    slider_idx++;
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
    uint16_t y;
    uint8_t starting_val = 0;

    if (layer == 0)
    {
        starting_val = 40;
    }
    while(normal_options[idx].layer == layer)
    {
        if (normal_options[idx].option_idx == current_option_idx)
        {
            _tft->setTextColor(TFT_GREEN);
            t = true;
        }
        
        y = starting_val + ((normal_options[idx].option_idx+1) * calculate_y(normal_options[idx].size));
        _tft->setCursor(calculate_starting_x(normal_options[idx].size, normal_options[idx].str_len), y);

        Serial.println(" Y: "+String(y) + ",   40 + "+ String(starting_val)+ " + ("+String(normal_options[idx].option_idx + 1)+ ") * "+ String(calculate_y(normal_options[idx].size)));
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