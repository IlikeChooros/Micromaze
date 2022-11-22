#include "Camera.h"

Camera::Camera(TFT_eSPI *tft)
{
    _tft = tft;
}

void Camera::load_map(uint8_t *matrix, uint16_t number_of_cols, uint16_t number_of_rows, uint8_t width_of_vision, uint8_t height_of_vision, uint32_t wall_color)
{
    _map = matrix;
    _number_of_cols = number_of_cols;
    _number_of_rows = number_of_rows;
    _width_of_vision = width_of_vision;
    _height_of_vision = height_of_vision;
    this->wall_color = wall_color;
    door_color = convert_to_RGB(201, 136, 6);

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
            all_gradient_colors[j][i] = HSV_to_RGB_ray_cast(this->colors[j],i, _ray_lenght);
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
}

void Camera::draw_vision_with_ray_cast(float angle, Point player_pos)
{
    double angle_of_ray = 0,
    starting_angle = check_if_overflow(angle - _angle_of_view/2);

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

void Camera::draw_current_vision(uint8_t x, uint8_t y)
{
    uint16_t y_ptr=0;
    uint16_t x_ptr=0;
    
    for (uint16_t y_pos = y - y_margin; y_pos<y+y_margin; y_pos++)
    {
        for (uint16_t x_pos = x -x_margin; x_pos<x+x_margin;x_pos++)
        {
            //Serial.println("Y_POS : "+ String(y_pos)+"  X_POS: "+ String(x_pos));
            if (_map[y_pos*_number_of_cols + x_pos] == 2)
            {
                _tft->fillRect(x_ptr*scale_x, y_ptr*scale_y,scale_x, scale_y, door_color);
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
    // should upgrade this to fast ray cast algorithm
    float sinus = sinf(angle),
        cosinus = cosf(angle); 
    
    double dist;
    int16_t x,y;
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
        ray_position.fl_x += sinus;
        ray_position.fl_y += cosinus;

        ray_position.x = round(ray_position.fl_x);
        ray_position.y = round(ray_position.fl_y);

        ray_pos = ray_position.y * _number_of_cols + ray_position.x;
        if (check_ray_collision(ray_pos))
        {
            color_idx = _map[ray_pos] - 1;
            x = ray_position.x - player_pos.x;

            y = ray_position.y- player_pos.y;

            dist = sqrtf(x*x+y*y);

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