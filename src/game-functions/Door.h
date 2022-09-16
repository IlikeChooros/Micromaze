#ifndef _DOOR_H
#define _DOOR_H

#include "../output/Matrix_map.h"
#include "../output/Player_tft.h"

#define FREE_ANALOG_PIN 28

enum Wall_dir {
    TOP_L,
    TOP_R,
    DOWN_L,
    DOWN_R,
    MIDDLE
};

class Door
{

    TFT_eSPI *_tft;
    Matrix_map *_map;

    uint16_t *door_positions;
    uint8_t _number_of_doors;

    uint16_t num_of_rows;
    uint16_t num_of_cols;

    uint8_t doors_nearby(uint16_t x, uint16_t y, uint8_t *matrix);

    public:
    Door(TFT_eSPI *tft, Matrix_map *_matrix_map);
    void load_map(uint8_t num_of_doors);
    bool check_collision_with_player(Point player_pos);
    void clear_map();
    Wall_dir generate_door(uint8_t approx);
};


#endif