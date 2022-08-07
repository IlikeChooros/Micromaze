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

void Camera::load_player(uint32_t player_c, uint32_t angle_color, float angle, Point player_pos)
{
    _player_angle = angle;
    player_color = player_c;
    player_angle_color = angle_color;
    prev_player_pos = player_pos;
}

void Camera::load_ray_casting(float angle_itr, float angle_of_view, uint8_t ray_lenght,HSV* colors)
{
    this->colors = colors;
    this->_angle_itr = angle_itr;
    this->_angle_of_view = angle_of_view;
    this->_ray_lenght = ray_lenght;
    all_gradient_colors = new uint32_t* [3]; // there are 3 types of possible obstacles, walls, doors, nodes(walls), in this order with index 0,1,2
    for (uint8_t j=0;j<3;j++)
    {   
        all_gradient_colors[j] = new uint32_t [ray_lenght+1];
        for (uint i=0;i<ray_lenght+1;i++)
        {
            all_gradient_colors[j][i] = HSV_to_RGB(this->colors[j],i);
        }
    }
}

void Camera::draw_vision(Point player_pos)
{
    draw_current_vision(camera_x(player_pos.x),camera_y(player_pos.y));
}

void Camera::draw_player(Point player_pos, float angle)
{
    uint16_t x_pos, y_pos;

    x_pos = player_x(player_pos.x);
    y_pos = player_y(player_pos.y);
    _player_angle = angle;

    _tft->fillRect(x_pos*scale_x, y_pos*scale_y, scale_x, scale_y, player_color);
    //show_current_angle_of_player(x_pos*scale_x, y_pos*scale_y);
}

void Camera::draw_vision_with_ray_cast(float angle, Point player_pos)
{
    double angle_of_ray = 0,
    starting_angle = check_if_overflow(angle - _angle_of_view/2);

    //camera_x(player_pos.x) - player_pos.x;
    //camera_y(player_pos.y) - player_pos.y;

    for (; angle_of_ray<_angle_of_view; angle_of_ray+=_angle_itr)
    {
        starting_angle = check_if_overflow(starting_angle + _angle_itr);
        ray_cast(starting_angle, player_pos,  player_pos.x - camera_x(player_pos.x), player_pos.y - camera_y(player_pos.y));
    }
}

void Camera::clear_prev_player_position(Point player_pos)
{
    _tft->fillRect(player_x(prev_player_pos.x)*scale_x, player_y(prev_player_pos.y)*scale_y, scale_x, scale_y, TFT_BLACK);
    prev_player_pos = player_pos;
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
uint16_t Camera::camera_x(uint16_t player_x)
{
    uint16_t x;
    if (player_x < x_margin)
    {
        x = x_margin;
    }
    else if (player_x > _number_of_cols - x_margin)
    {
        x = _number_of_cols - x_margin;
    }
    else{
        x = player_x;
    }
    return x;
}

uint16_t Camera::camera_y(uint16_t player_y)
{
    int16_t y;
    if (player_y<y_margin)
    {
        y = y_margin;
    }
    else if (player_y > _number_of_rows - y_margin)
    {
        y = _number_of_rows - y_margin;
    }
    else {
        y = player_y;
    }
    return y;
}

uint16_t Camera::player_x(uint16_t player_x)
{
    uint16_t x;
    if (player_x < x_margin)
    {
        x = player_x;
    }
    else if(player_x > _number_of_cols - x_margin)
    {
        x = player_x - _number_of_cols;
        x += 2*x_margin;
    }
    else{
        x = x_margin;
    }
    return x;
}   

uint16_t Camera::player_y(uint16_t player_y)
{
    uint16_t y;
    if (player_y < y_margin)
    {
        y = player_y;
    }

    else if(player_y > _number_of_rows - y_margin)
    {
        y = player_y - _number_of_rows;
        y += 2*y_margin;
    }
    else{
        y = y_margin;
    }
    return y;
}

void Camera::show_current_angle_of_player(uint16_t x_pos, uint16_t y_pos)
{
    Serial.println("Showing the angle x: "+String(x_pos)+" y: "+ String(y_pos));
    double sinus = sin(_player_angle);
    double cosinus = cos(_player_angle);

    float x,y;

    x = (x_pos + scale_x/2);
    y = (y_pos + scale_y/2);

    uint8_t scale=3;

    float itr_x = scale_x/(2*scale);  // sin(angle) = y/itr_x
    float itr_y = scale_y/(2*scale);

    for (uint8_t i=0; i< scale; i++)
    {
        x+=sinus*itr_x;
        y+=cosinus*itr_y;

        Serial.println("x_itr "+ String(itr_x) +"y_itr "+ String(itr_y) + " x: "+ String(x)+ " y:"+String(y));
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

void Camera::ray_cast(double angle, Point player_pos, int16_t vector_x, int16_t vector_y)
{
    double dist,x,y;
    uint8_t color_idx;
    Point_extended ray_position;
    int16_t y_pos, x_pos;

    uint32_t ray_pos;

    ray_position.x = player_pos.x;
    ray_position.y = player_pos.y;
    ray_position.fl_x = player_pos.x;
    ray_position.fl_y = player_pos.y;

    for (uint8_t i = 0; i<_ray_lenght;i++)
    {
        ray_position.fl_x += sin(angle);
        ray_position.fl_y += cos(angle);

        ray_position.x = round(ray_position.fl_x);
        ray_position.y = round(ray_position.fl_y);

        ray_pos = ray_position.y * _number_of_cols + ray_position.x;
        if (check_ray_collision(ray_pos))
        {
            color_idx = _map[ray_pos] - 1;
            x = ray_position.x - player_pos.x;

            y = ray_position.y- player_pos.y;

            dist = sqrt(x*x+y*y);

            x+=x_margin;
            y+=y_margin;

            // if ((x<0 || x>_width_of_vision) || (y <0 || y > _height_of_vision)) // out of player vision
            // {
            //     return;
            // }

            _tft->fillRect((x+vector_x)*scale_x,(y+vector_y)*scale_y,scale_x, scale_y, all_gradient_colors[color_idx][(uint8_t) dist]);

            return;
        }
    }
}

bool Camera::check_ray_collision(uint32_t ray_pos)
{
    return _map[ray_pos] != 0;
}


float Camera::check_if_overflow(double angle)
{
    if (angle>=6.28)
    {
        return angle=0;
    }
    else if (angle<=0)
    {
        return angle = 6.28+angle;
    }
    return angle;
}

uint32_t Camera::convert_to_RGB(uint8_t r, uint8_t g, uint8_t b)
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

uint32_t Camera::HSV_to_RGB(HSV color_hsv, uint8_t dist)
{
    float s = color_hsv.satrutaion/100;
    float v = color_hsv.value/100;

    v = v* (_ray_lenght + 1 - dist)/(_ray_lenght+1);

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