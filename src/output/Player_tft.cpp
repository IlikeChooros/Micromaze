#include "Player_tft.h"

Player_tft::Player_tft(Point starting_point, TFT_eSPI *tft, Matrix_map *matrix_map)
{
    _player_coordinates = starting_point;

    _tft=tft;
    _matrix_map=matrix_map;
}

void Player_tft::_init_()
{
    _prev_player_coordinates.x=_player_coordinates.x;
    _prev_player_coordinates.y=_player_coordinates.y;

    _player_float_coordinates.fl_x=_player_coordinates.x;
    _player_float_coordinates.fl_y=_player_coordinates.y;

    _number_of_cols=_matrix_map->get_current_map_cols();
    _number_of_rows = _matrix_map->get_current_map_rows();

    _player_width = _tft->width()/ _number_of_cols;// 240/24
    _player_height= _tft->height()/_number_of_rows; // 320/32

    angle=0; // in radians
    draw_player();  //  UNCOMMENT  FOR 2D MODE
}

void Player_tft::move_left()
{
    Serial.print(" CHANGING ANGLE (LEFT) - : ");
    angle-=0.15;
    check_if_overflow();

    Serial.println(String(angle));
}

void Player_tft::move_right()
{
    Serial.print(" CHANGING ANGLE (RIGHT) + : ");
    angle+=0.15;
    check_if_overflow();

    Serial.println(String(angle));
}

Point Player_tft::move_forward()
{
    Serial.print("MOVE FORWARD curr_x: "+String(_player_coordinates.x)+" curr_y"+String(_player_coordinates.y)+" TO -> ");
    float x,y;

    Point collision_point = {-1,-1,-1,-1};

    y = cos(angle); 
    x = sin(angle);

    _prev_player_coordinates.x=_player_coordinates.x;
    _prev_player_coordinates.y=_player_coordinates.y;

    _player_float_coordinates.fl_x +=x;
    _player_float_coordinates.fl_y +=y;



    _player_coordinates.x=round(_player_float_coordinates.fl_x);
    _player_coordinates.y=round(_player_float_coordinates.fl_y);

    Serial.println("_x: "+String(_player_coordinates.x)+" _y: "+String(_player_coordinates.y));
    Serial.println("");

    if(!(_player_coordinates.x < _number_of_cols && _player_coordinates.y<_number_of_rows)||!(_player_coordinates.x > 0 && _player_coordinates.y > 0)||check_collision())
    {
        Serial.println("OUT OF MAP");
        collision_point = _player_coordinates;

        _player_coordinates.x=_prev_player_coordinates.x;
        _player_coordinates.y=_prev_player_coordinates.y;
        _player_float_coordinates.fl_x=_player_coordinates.x;
        _player_float_coordinates.fl_y=_player_coordinates.y;

    }
    else{
        draw_player();  //  UNCOMMENT  FOR 2D MODE
        clear_prev_position();
        Serial.println("CLEARING POSITION_PREV");
    }
    return collision_point;
}

Point Player_tft::move_backwards()
{
    float x,y;

    Point collision_point = {-1,-1,-1,-1};

    y = cos(angle); 
    x = sin(angle);

    Serial.print("MOVE BACKWARD curr_x: "+String(_player_coordinates.x)+" curr_y"+String(_player_coordinates.y)+" TO -> ");
    _prev_player_coordinates.x=_player_coordinates.x;
    _prev_player_coordinates.y=_player_coordinates.y;

    _player_float_coordinates.fl_x -=x;
    _player_float_coordinates.fl_y -=y;

    _player_coordinates.x=round(_player_float_coordinates.fl_x);
    _player_coordinates.y=round(_player_float_coordinates.fl_y);

    Serial.println("_x: "+String(_player_coordinates.x)+" _y: "+String(_player_coordinates.y));
    Serial.println("");

    if(!(_player_coordinates.x < _number_of_cols && _player_coordinates.y<_number_of_rows)||!(_player_coordinates.x > 0 && _player_coordinates.y > 0)||check_collision())
    {
        Serial.println("OUT OF MAP");
        collision_point = _player_coordinates;

        if (_prev_player_coordinates.x != _player_coordinates.x)
        {
            _player_coordinates.x=_prev_player_coordinates.x;
            _player_float_coordinates.fl_x=_player_coordinates.x;
        }
        if (_prev_player_coordinates.y != _player_coordinates.y)
        {
            _player_coordinates.y=_prev_player_coordinates.y;
            _player_float_coordinates.fl_y=_player_coordinates.y;
        }
    }
    else{
        draw_player(); //  UNCOMMENT  FOR 2D MODE
        clear_prev_position();
        Serial.println("CLEARING POSITION_PREV");
    }
    return collision_point;
}

Point Player_tft::get_current_player_position()
{
    return _player_coordinates;
}

float Player_tft::get_current_angle()
{
    return angle;
}

void Player_tft::set_player_posistion(Point point)
{
    _player_coordinates.x = point.x;
    _player_coordinates.y = point.y;

    _prev_player_coordinates.x = _player_coordinates.x;
    _prev_player_coordinates.y = _player_coordinates.y;

    _player_float_coordinates.fl_x = _player_coordinates.x;
    _player_float_coordinates.fl_y = _player_coordinates.y;
    
}

// private

void Player_tft::clear_prev_position()
{
   _tft->fillRect(_prev_player_coordinates.x*_player_width, _prev_player_coordinates.y*_player_height, _player_width, _player_height, TFT_BLACK); 
}

void Player_tft::draw_player()
{
    _tft->fillRect(_player_coordinates.x*_player_width, _player_coordinates.y*_player_height, _player_width, _player_height, TFT_WHITE);
}

void Player_tft::check_if_overflow()
{
    if (angle>=PI*2)
    {
        angle=0;
    }
    else if (angle<0)
    {
        angle = 2*3.14-angle;
    }
}

bool Player_tft::check_collision()
{
    return _matrix_map->get_current_map()[_player_coordinates.y*_number_of_cols + _player_coordinates.x] != 0;
}