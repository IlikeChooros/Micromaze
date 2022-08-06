#ifndef _PLAYER_TFT
#define _PLAYER_TFT

#include "Matrix_map.h"
#include <math.h>
#include "../data-sturctures/Point_extended.h"

class Player_tft
{
    Point_extended _player_coordinates;
    Point_extended _prev_player_coordinates;
    Point_extended _player_float_coordinates;

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
    Player_tft(Point_extended starting_point, TFT_eSPI *tft, Matrix_map *matrix_map);
    void _init_();
    void move_left();
    void move_right();
    Point_extended move_forward();
    Point_extended move_backwards();
    float get_current_angle();
    Point_extended get_current_player_position();
    void set_player_posistion(Point_extended point);

};


#endif