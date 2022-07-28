#include "Door.h"

Door::Door(TFT_eSPI *tft, Matrix_map *_matrix_map)
{
    _tft = tft;
    _map = _matrix_map;
}

void Door::load_map(uint8_t num_of_doors)
{
    _number_of_doors = num_of_doors;

    num_of_rows = _map->get_current_map_rows();
    num_of_cols = _map->get_current_map_cols();

    uint8_t *matrix_m = _map->get_current_map();
    uint16_t *door_points = new uint16_t [_number_of_doors];

    uint8_t counter = 0;
    uint16_t pos_point;

    for (uint8_t y = 0; y<num_of_rows;y++)
    {
        for (uint8_t x=0; x<num_of_cols; x++)
        {
            pos_point = y*num_of_cols+x;
            if (matrix_m[pos_point]==2)
            {
                door_points[counter]=pos_point;
                counter++;
            }
        }
    }
    door_positions = door_points;
}

void Door::clear_map()
{
    delete door_positions;
}

bool Door::check_collision_with_player(Point player_pos)
{
    for (uint8_t i = 0; i<_number_of_doors; i++)
    {
        if (player_pos.y*num_of_cols+player_pos.x == door_positions[i])
        {
            return true;
        }
    }
    return false;
}