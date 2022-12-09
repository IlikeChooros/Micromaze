#ifndef _PLAYER_TFT
#define _PLAYER_TFT

#include "Matrix_map.h"
#include <math.h>
#include "../data-sturctures/Point.h"

#define ANGLE_ITERATOR 0.2f

class Player_tft
{
    Point _player_coordinates;
    Point _prev_player_coordinates;

    float _fl_player_x,
          _fl_player_y;

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

    void move_upward_downward(bool upward);

    public:
    Player_tft(TFT_eSPI *tft, Matrix_map *matrix_map);
    void _init_();
    Point move(uint8_t dir);
    Point get_current_player_position();
    float get_player_angle();
    void set_player_posistion(Point point);
};


#endif