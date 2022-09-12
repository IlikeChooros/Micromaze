#include "Joystick_esp.h"

void joystick_do_nothing()
{
    return;
}

Joystick::Joystick(uint8_t analog_x, uint8_t analog_y)
{
    _angalog_x = analog_x;
    _angalog_y = analog_y;
}

void Joystick::_init_()
{
    time=0;
    _ignore_val = 50;
    interval_time=100;

    this->on_dir_left(joystick_do_nothing);
    this->on_dir_right(joystick_do_nothing);
    this->on_dir_up(joystick_do_nothing);
    this->on_dir_down(joystick_do_nothing);
}

//          Postition of the joystick:
//          (pins up)                  
//               |||||      x
//              |=====|     |0
//              |  O  |     |       
//              | O#O |     |     
//              |  O  |     |
//              |=====|     |_ _ _ _ _ _ y 
//                          4096        0
//  LEFT: Xmax =~ 4000, DOWN: Ymax =~ 4000 
//  There are 2 functions describing the field which I will be using to determine wheter the direction is: LEFT, RIGHT, UP or DOWN
//  1. f(x) = x, 2. f(x) = -x + 4096
// Also ignoring joystick position for x<=2500 & x>=1500 & y <=2500 & y >=1500

void Joystick::read()
{
    if(millis()-time > interval_time)
    {
        uint16_t x = analogRead(_angalog_x);
        uint16_t y = analogRead(_angalog_y);

        //Serial.print("MOVE: ( x = "+String(x)+" , y = "+ String(y));

        if ((x<=1500 && x>=0) && (x<=y + _ignore_val) && (x<= -y + 4096 - _ignore_val))
        {
            // Serial.println(" UP (FORWARD) ");
            // Serial.println(""); 
            this->_on_up(); // in kartesian coordinate system it would be 'left' triagnle
        }

        else if ((x>=2500 && x<=4096) && (x>=y - _ignore_val) && (x>= - y +4096 + _ignore_val))
        {
            // Serial.println(" DOWN (BACKWARD) ");
            // Serial.println(""); 
            this->_on_down(); // 'right' triagnle
        }

        else if ((y>=2500 && y<=4096) && (x> -y +4096 - _ignore_val) && (x<y + _ignore_val))
        {
            // Serial.println(" LEFT ");
            // Serial.println(""); 
            this->_on_left(); // 'up' triagnle
        }

        else if ((y<=1500 && y>=0) && (x < -y + 4096 + _ignore_val) && (x>y - _ignore_val))
        {
            // Serial.println(" RIGHT ");
            // Serial.println("");
            this->_on_right();
        }

        time=millis();
    }

}

void Joystick::set_interval(uint16_t interval)
{
    interval_time = interval;
}

void Joystick::on_dir_left(void (*on_change)())
{
    this->_on_left = on_change;
}

void Joystick::on_dir_right(void (*on_change)())
{
    this->_on_right = on_change;
}

void Joystick::on_dir_down(void (*on_change)())
{
    this->_on_down = on_change;
}

void Joystick::on_dir_up(void (*on_change)())
{
    this->_on_up = on_change;
}

