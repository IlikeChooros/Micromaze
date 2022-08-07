#include "Player_tft.h"

Player_tft::Player_tft(TFT_eSPI *tft, Matrix_map *matrix_map)
{
    _tft=tft;
    _matrix_map=matrix_map;
}

void Player_tft::_init_()
{
    _number_of_cols=_matrix_map->get_current_map_cols();
    _number_of_rows = _matrix_map->get_current_map_rows();
}


Point Player_tft::move(uint8_t dir)
{
    Point collision_point;
    _prev_player_coordinates.x=_player_coordinates.x;
    _prev_player_coordinates.y=_player_coordinates.y;

    switch (dir)
    {
        case 0: // UP
            _player_coordinates.x++;
            break;
        case 1: // DOWN
            _player_coordinates.x--;
            break;
        case 2: // LEFT
            _player_coordinates.y--;
            break;
        case 3: // RIGHT
            _player_coordinates.y++;
            break;
        default:
            // nothing
            break;
    }

    if(check_collision())
    {
        collision_point.x = _player_coordinates.x;
        collision_point.y = _player_coordinates.y;
        _player_coordinates.x = _prev_player_coordinates.x;
        _player_coordinates.y = _prev_player_coordinates.y;
    }

    return collision_point;
}

Point Player_tft::get_current_player_position()
{
    return _player_coordinates;
}

void Player_tft::set_player_posistion(Point point)
{
    _player_coordinates = point;

    _prev_player_coordinates.x=_player_coordinates.x;
    _prev_player_coordinates.y=_player_coordinates.y;
}

// private

bool Player_tft::check_collision()
{
    return _matrix_map->get_current_map()[_player_coordinates.y*_number_of_cols + _player_coordinates.x] != 0;
}