#include "Camera.h"

Camera::Camera(TFT_eSPI *tft)
{
    _tft = tft;
}

void Camera::load_map(uint8_t *matrix, uint8_t number_of_cols, uint8_t number_of_rows, uint8_t width_of_vision, uint8_t height_of_vision, uint32_t wall_color)
{
    _map = matrix;
    _number_of_cols = number_of_cols;
    _number_of_rows = number_of_rows;
    _width_of_vision = width_of_vision;
    _height_of_vision = height_of_vision;
    this->wall_color = wall_color;

    x_margin = _width_of_vision/2;
    y_margin = _height_of_vision/2;

    scale_x = _tft->width()/(_width_of_vision);   // 240/_width
    scale_y = _tft->height()/(_height_of_vision); //320/_height
}

void Camera::load_player(uint32_t player_c, uint32_t angle_color, float angle)
{
    _player_angle = angle;
    player_color = player_c;
    player_angle_color = angle_color;
}

void Camera::draw_vision(Point_extended player_pos)
{
    uint16_t x,y;

    if (player_pos.x < x_margin)
    {
        x = x_margin;
    }
    else if (player_pos.x > _number_of_cols - x_margin)
    {
        x = _number_of_cols - x_margin;
    }
    else{
        x = player_pos.x;
    }

    if (player_pos.y<y_margin)
    {
        y = y_margin;
    }
    else if (player_pos.y > _number_of_rows - y_margin)
    {
        y = _number_of_rows - y_margin;
    }
    else {
        y = player_pos.y;
    }

    draw_current_vision(x,y);
}

void Camera::draw_player(Point_extended player_pos, float angle)
{
    _player_angle = angle;
    int16_t x_pos, y_pos;

    if (player_pos.x < x_margin)
    {
        x_pos = player_pos.x;
    }
    else if(player_pos.x > _number_of_cols - x_margin)
    {
        x_pos =player_pos.x - _number_of_cols;
        x_pos += 2*x_margin;
    }
    else{
        x_pos = x_margin;
    }

    if (player_pos.y < y_margin)
    {
        y_pos = player_pos.y;
    }

    else if(player_pos.y > _number_of_rows - y_margin)
    {
        y_pos = player_pos.y - _number_of_rows;
        y_pos += 2*y_margin;
    }
    else{
        y_pos = y_margin;
    }

    _tft->fillRect(x_pos*scale_x, y_pos*scale_y, scale_x, scale_y, player_color);
    show_current_angle_of_player(x_pos*scale_x, y_pos*scale_y);
}

// private

// I just rotated my screen
//   ^ X
//   |
//   |
//   |<--------- height/2 (y_margin)   
//   |---------------------| ^
//   |                     | |
//   |                     | |
//   |        (x,y)        | |
//   |          X          | width/2 (x_margin)
//   |                     |
//   |                     |
//   |                     |
//   |---------------------|
//   |
//   |
//   |-------------------------------------------------> Y
//  0,0

void Camera::show_current_angle_of_player(uint8_t x_pos, uint8_t y_pos)
{
    double sinus = sin(_player_angle);
    double cosinus = cos(_player_angle);

    float x,y;

    x = (x_pos + scale_x/2);
    y = (y_pos + scale_y/2);

    uint8_t scale=5;

    float itr_x = scale_x/(2*scale);  // sin(angle) = y/itr_x
    float itr_y = scale_y/(2*scale);

    for (uint8_t i=0; i< scale; i++)
    {
        x+=sinus*itr_x;
        y+=cosinus*itr_y;

        _tft->fillRect(round(x), round(y), round(itr_x), round(itr_y), player_angle_color);
        _tft->fillRect(round(x+0.5), round(y+0.5), round(itr_x), round(itr_y), player_angle_color);
        _tft->fillRect(round(x-0.5), round(y-0.5), round(itr_x), round(itr_y), player_angle_color);
    }
}

void Camera::draw_current_vision(uint8_t x, uint8_t y)
{
    uint8_t y_ptr=0;
    uint8_t x_ptr=0;
    
    for (uint8_t y_pos = y - y_margin; y_pos<y+y_margin; y_pos++)
    {
        for (uint8_t x_pos = x -x_margin; x_pos<x+x_margin;x_pos++)
        {
            //Serial.println("Y_POS : "+ String(y_pos)+"  X_POS: "+ String(x_pos));
            if (_map[y_pos*_number_of_cols + x_pos] == 2)
            {
                _tft->fillRect(x_ptr*scale_x, y_ptr*scale_y,scale_x, scale_y, TFT_MAROON);
            }


            else if (_map[y_pos*_number_of_cols + x_pos])
            {
                _tft->fillRect(x_ptr*scale_x, y_ptr*scale_y,scale_x, scale_y, wall_color);
            }
            x_ptr++;
        }
        y_ptr++;
        x_ptr=0;
    }
}