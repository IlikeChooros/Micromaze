#ifndef _CAMERA_H
#define _CAMERA_H

#include <TFT_eSPI.h> 
#include <SPI.h>
#include "../data-sturctures/Point.h"
#include "../data-sturctures/Point_extended.h"
#include <math.h>

class Camera
{
    TFT_eSPI *_tft;

    Point prev_player_pos;

    uint16_t _number_of_cols;
    uint16_t _number_of_rows;

    uint8_t _width_of_vision;
    uint8_t _height_of_vision;

    uint8_t x_margin;
    uint8_t y_margin;

    int16_t scale_y;
    int16_t scale_x;

    uint8_t *_map;

    uint32_t wall_color;
    uint32_t player_color;
    uint32_t player_angle_color;
    uint32_t door_color;

    float _angle_itr;
    float _angle_of_view;
    HSV* colors;
    uint8_t _ray_lenght;

    float _player_angle;

    uint32_t **all_gradient_colors;


    void draw_current_vision(uint8_t x, uint8_t y);
    void show_current_angle_of_player(uint16_t x_pos, uint16_t y_pos);

    float check_if_overflow(double angle);
    bool check_ray_collision(uint32_t ray_pos);
    void ray_cast(double angle, Point player_pos, int16_t vector_x, int16_t vector_y);
    void draw_current_vision_with_ray_cast(Point player_pos, Point_extended ray_pos);

    uint16_t camera_x(uint16_t x);
    uint16_t camera_y(uint16_t y);

    uint16_t player_x(uint16_t x);
    uint16_t player_y(uint16_t y);
    

    uint32_t convert_to_RGB(uint8_t r, uint8_t g, uint8_t b);
    uint32_t HSV_to_RGB(HSV color_hsv, uint8_t dist);

    public:
    Camera(TFT_eSPI *tft);
    void load_map(uint8_t *matrix, uint16_t number_of_cols, uint16_t number_of_rows, uint8_t width_of_vision, uint8_t height_of_vision, uint32_t wall_color);
    void load_player(uint32_t player_color, uint32_t player_angle_color, float player_angle, Point player_pos);
    void load_ray_casting(float angle_itr, float angle_of_view,uint8_t ray_lenght ,HSV *colors);
    void draw_vision(Point player_pos);
    void draw_player(Point player_pos, float player_angle);
    void draw_vision_with_ray_cast(float player_angle ,Point player_pos);
    void clear_prev_player_position(Point player_pos);
};


#endif