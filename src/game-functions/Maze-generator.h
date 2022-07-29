#ifndef _MAZE_GENERATOR_
#define _MAZE_GENERATOR_

#include "LinkedList.h"

#define UNUSED_PIN_ANALOG 26

enum Wall_dir {
    UP = 0,
    DOWN = 1,
    RIGHT = 2,
    LEFT = 3
};

class Maze_generator
{
    LinkedList *four_generators;

    uint8_t *_map;

    uint8_t _number_of_rows;
    uint8_t _number_of_cols;

    bool finished;

    void check_possible_moves(uint8_t dir, uint8_t generator, Node* node, uint8_t dist);
    
    void move_wall_left(Node *node, uint8_t dist, uint16_t _pos);
    void move_wall_right(Node *node, uint8_t dist, uint16_t _pos);
    void move_wall_up(Node *node, uint8_t dist, uint16_t _pos);
    void move_wall_down(Node *node, uint8_t dist, uint16_t _pos);

    void generate_part_maze();

    public:
    Maze_generator(uint8_t number_of_col, uint8_t number_of_rows,uint8_t *matrix);
    void _init_();
    void generate_maze();
};


#endif