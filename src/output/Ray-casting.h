#ifndef _RAY_CASTING
#define _RAY_CASTING

#include "Matrix_map.h"
#include "Player_tft.h"

struct HSV
{
    float hue;
    float satrutaion;
    float value;
};

class Ray_casting
{
    Point ray_position;
    uint8_t _ray_lenght;

    TFT_eSPI *_tft;
    Matrix_map *_map;

    uint16_t _number_of_rows;
    uint16_t _number_of_cols;

    double height_of_segment;
    double width_of_segment;

    HSV *_colors;

    double _angle_itr;
    double _angle_of_view;

    float check_if_overlow(double angle);
    void ray_cast(double angle, Point player_pos, uint16_t counter);
    bool wall_collision(uint16_t ray_pos);
    void draw_segment(double distance, uint16_t counter,uint32_t color);
    uint32_t converToRGB(uint8_t r, uint8_t g, uint8_t b);
    uint32_t HSV_to_RGB(HSV color, uint8_t dist);

    public:
    Ray_casting(TFT_eSPI *tft, Matrix_map *matrix_map, uint8_t lenght_of_ray, double angle_increment, double angle_of_view,HSV *colors);
    void _init_();
    void draw(Point player_pos, double angle);
};

#endif