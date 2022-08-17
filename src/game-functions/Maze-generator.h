#ifndef _MAZE_GENERATOR_
#define _MAZE_GENERATOR_

#include "../data-sturctures/Stack.h"
#include "../data-sturctures/Point_extended.h"
#include "TFT_eSPI.h"
#include "SPI.h"

#define UNUSED_PIN_ANALOG 26
#define CELL_VISITED 5

enum Possible_dir {
    CELL_NORTH_CONNECTION = 1,
    CELL_SOUTH_CONNECTION = 2,
    CELL_WEST_CONNECTION = 3,
    CELL_EAST_CONNECTION = 4
};

class Maze_generator
{
    TFT_eSPI *_tft;
    Stack stack;

    uint8_t *_map;
    uint8_t *visited;
    uint8_t _number_of_visited;

    uint8_t _number_of_rows;
    uint8_t _number_of_cols;

    uint8_t path_width;
    Point_extended cell_scale;

    void draw_connection_ew(Point point, uint32_t color);
    void draw_connection_ns(Point point, uint32_t color);
    uint16_t convert_to_coordinates(Point *point);

    void generate(Point *point);


    public:
    Maze_generator(TFT_eSPI *tft);
    void load_map(uint8_t number_of_cols, uint8_t number_of_rows);
    void generate_maze(Point *starting_point);
    void draw_maze();
};


#endif