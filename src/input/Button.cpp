#include "Button.h"

void button_do_nothing() {
	return;
}

Button::Button (uint8_t pin)
{
    _pin = pin;
    _state = HIGH;
    _lastState = _state;
    _pressed = _state == LOW;
    _lastDebounceTime = 0;
    
    this->on_press(button_do_nothing);
    this->on_release(button_do_nothing);
}

void Button::_init_()
{
    pinMode(_pin, INPUT_PULLUP);
}

void Button::on_press(void(*on_press)(void)) {
	this->_on_press = on_press;
}

void Button::on_release(void(*on_release)(void)) {
	this->_on_release = on_release;
}

uint8_t Button::state()
{
    return _state;
}

void Button::reset()
{
    _state = HIGH;
}


void Button::read()
{
    uint8_t state = readState();
    if (_lastState!=state)
    {
        _lastDebounceTime = millis();
    }

    if (_state!=state && (millis()-_lastDebounceTime)> 30)
    {
        _state=state;
        if (_state == LOW)
        {
            this->_on_press();
        }
        else{
            this->_on_release();
        }
    }
    _lastState = state;
}

// private methods

uint8_t Button::readState()
{
    return digitalRead(_pin);
}
