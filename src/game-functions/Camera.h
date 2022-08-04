#ifndef _CAMERA_H
#define _CAMERA_H

#include <TFT_eSPI.h> 
#include <SPI.h>
#include "Point_extended.h"
#include <math.h>

#define DEGREES_45 45/180*3.14
#define DEGREES_70 80/180*3.14
#define DEGREES_110 100/180*3.14
#define DEGREES_135 135/180*3.14
#define DEGREES_225 225/180*3.14
#define DEGREES_250 260/180*3.14
#define DEGREES_290 280/180*3.14
#define DEGREES_315 315/180*3.14
#define DEGREES_90 3.14*0.5
#define DEGREES_180 3.14
#define DEGREES_270 1.5*3.14
#define DEGREES_360 2*3.14

class Camera
{
    TFT_eSPI *_tft;

    uint8_t _number_of_cols;
    uint8_t _number_of_rows;

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
    float _player_angle;

    void draw_current_vision(uint8_t x, uint8_t y);
    void show_current_angle_of_player(uint8_t x_pos, uint8_t y_pos);

    public:
    Camera(TFT_eSPI *tft);
    void load_map(uint8_t *matrix, uint8_t number_of_cols, uint8_t number_of_rows, uint8_t width_of_vision, uint8_t height_of_vision, uint32_t wall_color);
    void load_player(uint32_t player_color, uint32_t player_angle_color, float angle);
    void draw_vision(Point_extended player_pos);
    void draw_player(Point_extended player_pos, float angle);
};


#endif