#include "Door.h"

Door::Door(TFT_eSPI *tft, Matrix_map *_matrix_map)
{
    _tft = tft;
    _map = _matrix_map;
    randomSeed(analogRead(FREE_ANALOG_PIN));
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

void Door::generate_door(uint8_t approx)
{
    Wall_dir destinated_dir;
    Wall_dir current_dir;

    uint16_t rand_num = random(0,10001);

    if (rand_num<2500)
    {
        destinated_dir = TOP_L;
    }
    else if(rand_num < 5000)
    {
        destinated_dir = TOP_R;
    }
    else if(rand_num < 7500)
    {
        destinated_dir = DOWN_R;
    }
    else {
        destinated_dir = DOWN_L;
    }

    _number_of_doors = 1;
    uint16_t *door_pos = new uint16_t [_number_of_doors];
    num_of_cols = _map->get_current_map_cols();
    num_of_rows = _map->get_current_map_rows();

    uint8_t dist = random(5,11);
    double distance_from_node;
    int32_t _x,_y;
    uint16_t point_of_refrence_x,point_of_refrence_y;

    uint16_t max_number_of_doors=1;
    uint16_t max_coordinates[2];

    uint8_t *matrix = _map->get_current_map();

    uint8_t num_of_doors_nearby[4]={0,0,0,0}; 

    for(uint8_t y=1;y<num_of_rows-1;y++)
    {
        for(uint8_t x=1;x<num_of_cols-1;x++)
        {
            if (matrix[y*num_of_cols+x]==3)
            {
                _x = x;
                _y=y;

                if (x<num_of_cols/2 && y<num_of_rows/2)
                {
                    point_of_refrence_x = 1;
                    point_of_refrence_y = 1;
                    current_dir = DOWN_L;
                }
                else if (x<num_of_cols/2 && y>= num_of_rows/2)
                {
                    point_of_refrence_x = 1;
                    point_of_refrence_y = num_of_rows-1;
                    current_dir = DOWN_R;
                }

                else if( x>=num_of_cols/2 && y<num_of_rows/2)
                {
                    point_of_refrence_x = num_of_cols -1;
                    point_of_refrence_y = 1;
                    current_dir = TOP_L;
                }
                else{
                    point_of_refrence_x = num_of_cols - 1;
                    point_of_refrence_y = num_of_rows - 1;
                    current_dir = TOP_R;
                }
                _x -= point_of_refrence_x;
                _x *= _x;

                _y -= point_of_refrence_y;
                _y*=_y;

                distance_from_node = sqrt(_y+_x);
                distance_from_node = round(distance_from_node);
                if (dist+approx>= distance_from_node && dist-approx<=distance_from_node && current_dir == destinated_dir)
                {
                    matrix[y*num_of_cols + x]=3;
                    num_of_doors_nearby[0] = doors_nearby(x-1, y-1, matrix); // TOP LEFT
                    num_of_doors_nearby[1] = doors_nearby(x-1, y+1, matrix); // TOP RIGHT
                    num_of_doors_nearby[2] = doors_nearby(x+1, y-1, matrix); // DOWN LEFT
                    num_of_doors_nearby[3] = doors_nearby(x+1, y+1, matrix); // DOWN RIGHT
                    
                    for (uint8_t i=0;i<4;i++)
                    {
                        if (num_of_doors_nearby[i]>=max_number_of_doors)
                        {
                            if (i%2 == 0)
                            {
                                max_coordinates[1] = y-1;   
                            }
                            else{
                                max_coordinates[1] = y+1;  
                            }
                            if(i<2)
                            {
                                max_coordinates[0] = x-1;
                            }
                            else{
                                max_coordinates[0] = x+1;
                            }
                            max_number_of_doors = num_of_doors_nearby[i];
                        }
                    }

                }
            }
        }
    }

    door_pos[0] = max_coordinates[1]*num_of_cols+max_coordinates[0];
    door_positions = door_pos;
    matrix[max_coordinates[1]*num_of_cols+max_coordinates[0]] = 2;

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


uint8_t Door::doors_nearby(uint8_t x, uint8_t y, uint8_t *matrix)
{
    uint8_t num_of_doors_nearby=0;
    uint16_t pos = y*num_of_cols+x;
    if (matrix[pos])
    {
        return 0;
    }

    if (matrix[pos+1])
    {
        num_of_doors_nearby++;
    }
    if (matrix[pos-1])
    {
        num_of_doors_nearby++;
    }
    if (matrix[pos + num_of_cols])
    {
        num_of_doors_nearby++;
    }
    if (matrix[pos - num_of_cols])
    {
        num_of_doors_nearby++;
    }
    return num_of_doors_nearby;
}