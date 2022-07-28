#ifndef _DOOR_H
#define _DOOR_H

#include "../output/Matrix_map.h"
#include "../output/Player_tft.h"

class Door
{

    TFT_eSPI *_tft;
    Matrix_map *_map;

    uint16_t *door_positions;
    uint8_t _number_of_doors;

    uint8_t num_of_rows;
    uint8_t num_of_cols;

    public:
    Door(TFT_eSPI *tft, Matrix_map *_matrix_map);
    void load_map(uint8_t num_of_doors);
    bool check_collision_with_player(Point player_pos);
    void clear_map();
};


#endif