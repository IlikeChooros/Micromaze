#ifndef _PLAYER_TFT
#define _PLAYER_TFT

#include "Matrix_map.h"
#include <math.h>

struct Point
{
    uint8_t x;
    uint8_t y;

    float fl_x;
    float fl_y;
};

class Player_tft
{
    Point _player_coordinates;
    Point _prev_player_coordinates;
    Point _player_float_coordinates;

    uint16_t _player_height;
    uint16_t _player_width;

    uint16_t _number_of_rows;
    uint16_t _number_of_cols;

    TFT_eSPI *_tft;
    Matrix_map *_matrix_map;
    float angle;

    void draw_player();
    void clear_prev_position();
    bool check_collision();
    void check_if_overflow();

    public:
    Player_tft(Point starting_point, TFT_eSPI *tft, Matrix_map *matrix_map);
    void _init_();
    void move_left();
    void move_right();
    Point move_forward();
    Point move_backwards();
    float get_current_angle();
    Point get_current_player_position();
    void set_player_posistion(Point point);

};


#endif