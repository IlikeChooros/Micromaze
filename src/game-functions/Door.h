#ifndef _DOOR_H
#define _DOOR_H

#include "../output/Matrix_map.h"
#include "../output/Player_tft.h"

#define FREE_ANALOG_PIN 28

enum Wall_dir {
    TOP_L = 0,
    TOP_R = 1,
    DOWN_L = 2,
    DOWN_R = 3
};

class Door
{

    TFT_eSPI *_tft;
    Matrix_map *_map;

    uint16_t *door_positions;
    uint8_t _number_of_doors;

    uint8_t num_of_rows;
    uint8_t num_of_cols;

    uint8_t doors_nearby(uint8_t x, uint8_t y, uint8_t *matrix);
    Wall_dir check_door_dir(uint8_t x, uint8_t y);

    public:
    Door(TFT_eSPI *tft, Matrix_map *_matrix_map);
    void load_map(uint8_t num_of_doors);
    bool check_collision_with_player(Point player_pos);
    void clear_map();
    void generate_door(uint8_t approx);
};


#endif