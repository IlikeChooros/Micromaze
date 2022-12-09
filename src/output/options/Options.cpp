#include "Options.h"

Options::Options(uint8_t index, TFT_eSPI *tft, void(*on_action)(void))
{
    this->_tft = tft;
    this->_index = index;
    this->_on_action = on_action;
}

void Options::_init_(uint8_t number_of_normal_opt, uint8_t num_of_sliders, uint8_t num_of_layers)
{
    this->number_of_opt = number_of_normal_opt;
    normal_options = 0;

    bool t = false;

    if (number_of_normal_opt>0)
    {
        normal_options = new Option[number_of_normal_opt];
        t = true;
    }

    normal_opt_idx = 0;
    slider_idx = 0;

    if (num_of_sliders>0)
    {
        sliders = new Slider[num_of_sliders];
    }

    if (num_of_layers>0 && t)
    {
        list_of_idx = new uint8_t* [num_of_layers];
        for (uint8_t i=0;i<num_of_layers;i++)
        {
            list_of_idx[i] = new uint8_t [2];
        }
    }

    prev_layer = 0;
    first_option = true;
}

void Options::create_slider(uint8_t layer, uint8_t option_idx, const char option_name[], int16_t starting_num,int16_t min_num , int16_t max_num  , uint8_t text_size, uint8_t itr)
{
    sliders[slider_idx].layer = layer;
    sliders[slider_idx].option_idx = option_idx;
    sliders[slider_idx].size = text_size;
    sliders[slider_idx].option_name = option_name;
    sliders[slider_idx].current_val = starting_num;
    sliders[slider_idx].max_val = max_num;
    sliders[slider_idx].min_val = min_num;
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
    // list_of_idx [layer][0] ->starting idx of opt, list_of_idx[layer][1] -> ending idx of opt

    if (prev_layer != layer)
    {
        prev_layer = layer;
        first_option = true;
    }

    if (first_option)
    {
        list_of_idx[layer][0] = normal_opt_idx;
        first_option = false;
    }


    list_of_idx[layer][1]=normal_opt_idx;

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
    counter = list_of_idx[layer][1] - list_of_idx[layer][0] + 1;

    for (uint8_t i=0;i<slider_idx;i++)
    {
        if(sliders[i].layer == layer)
        {
            counter++;
        }
    }
    return counter;
}

void Options::draw(uint8_t layer, uint8_t current_option_idx)
{
    uint8_t idx;
    idx = list_of_idx[layer][0];

    _tft->setRotation(1);
    _tft->setTextColor(TFT_WHITE);

    bool is_on_cursor=false;
    uint16_t y;
    uint8_t starting_val = 0;

    if (layer == 0)
    {
        starting_val = 40;
    }
    while(normal_options[idx].layer == layer)
    {
        _tft->setTextSize(normal_options[idx].size);
        if (normal_options[idx].option_idx == current_option_idx)
        {
            _tft->setTextColor(TFT_GREEN);
            is_on_cursor = true;
        }
        
        y = starting_val + ((normal_options[idx].option_idx+1) * calculate_y(normal_options[idx].size));
        _tft->setCursor(calculate_starting_x(normal_options[idx].size, normal_options[idx].str_len), y);
        _tft->print(normal_options[idx].option_name);

        if (normal_options[idx].marked)
        {
            _tft->print("  X");
        }

        if (is_on_cursor)
        {
            _tft->setTextColor(TFT_WHITE);
        }
        
        idx++;

        if (idx == normal_opt_idx)
        {
            break;
        }
    }

    if (slider_idx>0)
    {
        draw_slider(layer, current_option_idx);
    }

    _tft->setTextSize(1);
    _tft->setRotation(0);
}

void Options::draw_slider(uint8_t layer, uint8_t curr_sli_idx)
{
    uint8_t y, idx;
    for (uint8_t i=0;i<slider_idx;i++)
    {
        if (sliders[i].layer == layer)
        {
            idx = i;
            break;
        }
    }

    _tft->setRotation(1);
    _tft->setTextColor(TFT_WHITE);

    bool is_on_cursor=false;

    while(sliders[idx].layer == layer)
    {
        _tft->setTextSize(sliders[idx].size);
        if (sliders[idx].option_idx == curr_sli_idx)
        {
            _tft->setTextColor(TFT_GREEN);
            is_on_cursor = true;
        }

        y = (sliders[idx].option_idx+1)*calculate_y(sliders[idx].size);
        _tft->setCursor(calculate_starting_x(sliders[idx].size, sliders[idx].str_len + 2*sliders[idx].size)- 6*sliders[idx].size, y);
        _tft->print(sliders[idx].option_name);
        _tft->print(" <");
        _tft->print(String(sliders[idx].current_val));
        _tft->print(">");

        //Serial.println(" Y: "+String(y) + ",   0"+ " + ("+String(sliders[idx].option_idx + 1)+ ") * "+ String(calculate_y(sliders[idx].size)));

        if (is_on_cursor)
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

void Options::increment_slider(uint8_t layer, uint8_t opt_idx)
{
    for (uint8_t i=0; i<slider_idx;i++)
    {
        if ((sliders[i].layer == layer) && (sliders[i].option_idx == opt_idx))
        {
            sliders[i].current_val = sliders[i].current_val < sliders[i].max_val ? sliders[i].current_val + sliders[i].itr : sliders[i].min_val;
            return;
        }
    }
}

void Options::decrement_slider(uint8_t layer, uint8_t opt_idx)
{
    for (uint8_t i=0;i<slider_idx;i++)
    {
        if ( (sliders[i].layer == layer) && (sliders[i].option_idx == opt_idx))
        {
            sliders[i].current_val = sliders[i].current_val > sliders[i].min_val ? sliders[i].current_val - sliders[i].itr : sliders[i].max_val;
            return;
        }
    }
}

int16_t Options::get_value(uint8_t layer, uint8_t opt_idx)
{
    for (uint8_t i=0;i<slider_idx;i++)
    {
        if ((sliders[i].layer == layer) && (sliders[i].option_idx == opt_idx))
        {
            return sliders[i].current_val;
        }
    }
    return 0;
}

void Options::set_value(uint8_t layer, uint8_t idx, int16_t val)
{
    for (uint8_t i=0;i<slider_idx;i++)
    {
        if ((sliders[i].layer == layer) && (sliders[i].option_idx == idx))
        {
            sliders[i].current_val = val < sliders[i].max_val && val > sliders[i].min_val ? val : sliders[i].current_val;
            return;
        }
    }
}

void Options::set_mark(bool mark, uint8_t opt_idx, uint8_t layer)
{
    normal_options[list_of_idx[layer][0] + opt_idx].marked = mark;
    // normal_options[ starting_idx_in_this_layer  +  option_idx_that_i_want_to_change_mark_on ]
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