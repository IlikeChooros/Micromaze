#include "Ray-casting-2D.h"

Ray_casting::Ray_casting(TFT_eSPI *tft, Matrix_map *matrix, uint8_t ray_lenght, double angle_increment, double angle_of_view, HSV *colors)
{
    _tft = tft;
    _map = matrix;
    _ray_lenght = ray_lenght;
    _angle_itr = angle_increment;
    _angle_of_view = angle_of_view;
    _colors = colors;
}

void Ray_casting::_init_()
{
    _number_of_cols=_map->get_current_map_cols();
    _number_of_rows = _map->get_current_map_rows();

    width_of_segment = _tft->height()/_number_of_cols;
    height_of_segment = _tft->height()/(_angle_of_view/_angle_itr);
}


void Ray_casting::draw(Point_exetened player_pos, double angle)
{
    double ray_angle=angle, angle_of_ray=0, starting_angle = check_if_overlow(angle - _angle_of_view/2- _angle_itr*5), ending_angle = check_if_overlow(angle + _angle_of_view/2);

    //_tft->fillScreen(TFT_BLACK); // COMMENT FOR 2D MODE

    Serial.println("STARTING ANGLE: "+String(starting_angle)+ " ENDING ANGLE: "+ String(ending_angle));

    for (; angle_of_ray<_angle_of_view; angle_of_ray+=_angle_itr)
    {
        starting_angle= check_if_overlow(starting_angle+_angle_itr);
        ray_cast(starting_angle, player_pos);
    }
}

//private

void Ray_casting::ray_cast(double angle, Point_exetened player_pos)
{

    double dist, double_x, double_y;
    ray_position.x = player_pos.x;
    ray_position.y = player_pos.y;

    uint16_t  width_of_segment=_tft->width()/_number_of_cols;
    uint16_t  height_of_segment=_tft->height()/_number_of_rows;

    uint16_t ray_pos;

    float x=ray_position.x,y=ray_position.y;

    for (uint8_t i = 0; i<_ray_lenght; i++)
    {
        
        x+=sin(angle);
        y+=cos(angle);

        ray_position.x= round(x);
        ray_position.y= round(y);

        //_tft->fillRect(ray_position.x*width_of_segment, ray_position.y*height_of_segment,width_of_segment,height_of_segment,converToRGB(93, 173, 23));   //  UNCOMMENT  FOR 2D MODE
        
        
        ray_pos = ray_position.y*_number_of_cols+ray_position.x;
        if (wall_collision(ray_pos))
        {
            uint8_t color_idx = _map->get_current_map()[ray_pos]-1;

            double_x = player_pos.x - ray_position.x;
            double_x *= double_x;

            double_y = player_pos.y - ray_position.y;
            double_y*=double_y;

            dist = sqrt(double_x + double_y);

            
            //Serial.println("X POINT: "+String(x_pointer)+" Y POINT: "+String(y_pointer));

            _tft->fillRect(ray_position.x*width_of_segment, ray_position.y*height_of_segment,width_of_segment,height_of_segment,HSV_to_RGB(_colors[color_idx], (uint8_t) dist));

            return;
        }

        if(!(ray_position.x < _number_of_cols && ray_position.y<_number_of_rows))
        {
            return;
        }
    }
}

bool Ray_casting::wall_collision(uint16_t ray_pos)
{
    return  _map->get_current_map()[ray_pos] != 0;
}

float Ray_casting::check_if_overlow(double angle)
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

uint32_t Ray_casting::HSV_to_RGB(HSV color, uint8_t dist)
{
    float s = color.satrutaion/100;
    float v = color.value/100;

    v = v* (_ray_lenght + 1 - dist)/(_ray_lenght+1);

    float C = s*v;
    float X = C * ( 1 - abs(fmod(color.hue/60,2) - 1));
    float m = v - C; 

    float r,g,b;

    if (color.hue < 60)
    {
        r = C, g=X, b=0;
    }
    else if (color.hue<120)
    {
        r=X, g=C, b=0;
    }    
    else if (color.hue<180)
    {
        r=0, g=C, b=X;
    }    
    else if (color.hue<240)
    {
        r=0, g=X, b=C;
    }    
    else if (color.hue<300)
    {
        r=X, g=0, b=C;
    }    
    else
    {
        r=C, g=0, b=X;
    }

    return converToRGB(
        (r+m)*255,
        (g+m)*255,
        (b+m)*255
    );
}


uint32_t Ray_casting::converToRGB(uint8_t r, uint8_t g, uint8_t b)
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