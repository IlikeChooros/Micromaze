#ifndef WEAPON_H
#define WEAPON_H

#include "Arduino.h"

#include "../data-sturctures/Point.h"

class Weapon
{
    uint8_t _number_of_ammo;

    public:
    Weapon();
    bool shoot(Point player_position, float angle);
    void add_ammo(uint8_t number_of_ammo);
};


#endif