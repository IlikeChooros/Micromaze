#include "Player_tft.h"

Player_tft::Player_tft(TFT_eSPI *tft, Matrix_map *matrix_map)
{
    _tft=tft;
    _matrix_map=matrix_map;
    angle = 0;

    this->_player_coordinates.x = 0;
    this->_player_coordinates.y = 0;
    this->_fl_player_x = 0;
    this->_fl_player_y = 0;
}

void Player_tft::_init_()
{
    _number_of_cols=_matrix_map->get_current_map_cols();
    _number_of_rows = _matrix_map->get_current_map_rows();
}


Point Player_tft::move(uint8_t dir)
{
    Point collision_point;
    _prev_player_coordinates = _player_coordinates;
    switch (dir)
    {
        case 0: // UP
            move_upward_downward(true);
            break;
        case 1: // DOWN
            move_upward_downward(false);
            break;
        case 2: // LEFT
            this->angle -= ANGLE_ITERATOR;
            check_if_overflow();
            break;
        case 3: // RIGHT
            this->angle += ANGLE_ITERATOR;
            check_if_overflow();
            break;
        default:
            // nothing
            break;
    }

    if(check_collision())
    {
        collision_point = _player_coordinates;
        _player_coordinates = _prev_player_coordinates;
        this->_fl_player_x = _player_coordinates.x;
        this->_fl_player_y = _player_coordinates.y;
    }

    return collision_point;
}

Point Player_tft::get_current_player_position()
{
    return _player_coordinates;
}

float Player_tft::get_player_angle()
{
    return this->angle;
}

void Player_tft::set_player_posistion(Point point)
{
    _player_coordinates = point;
    _prev_player_coordinates = _player_coordinates;
}

void Player_tft::move_upward_downward(bool upward)
{
    float sinus = sinf(angle),
        cosinus = cosf(angle);

    if (upward)
    {
        _fl_player_x += cosinus;
        _fl_player_y += sinus;
    }
    else
    {
        _fl_player_x -= cosinus;
        _fl_player_y -= sinus;
    }

    this->_player_coordinates.x = roundf(_fl_player_x);
    this->_player_coordinates.y = roundf(_fl_player_y);
}

// private
bool Player_tft::check_collision()
{
    return _matrix_map->get_current_map()[_player_coordinates.y*_number_of_cols + _player_coordinates.x] != 0;
}

void Player_tft::check_if_overflow()
{
    if (angle>=6.283f)
    {
        angle=0;
    }
    else if (angle<0)
    {
        angle += 6.283f;
    }
}