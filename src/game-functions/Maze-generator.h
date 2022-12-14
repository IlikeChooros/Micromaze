#ifndef _MAZE_GENERATOR_
#define _MAZE_GENERATOR_

#include "../data-sturctures/LinkedList.h"

#define UNUSED_PIN_ANALOG 26

class Maze_generator
{
    LinkedList *four_generators;

    uint8_t *_map;

    uint16_t _number_of_rows;
    uint16_t _number_of_cols;

    uint8_t _number_of_generations;

    uint8_t _starting_pos;

    bool finished;

    void check_possible_moves(uint8_t dir, uint8_t generator, Node* node, uint8_t dist);
    
    void move_wall_left(Node *node, uint8_t dist, uint32_t _pos);
    void move_wall_right(Node *node, uint8_t dist, uint32_t _pos);
    void move_wall_up(Node *node, uint8_t dist, uint32_t _pos);
    void move_wall_down(Node *node, uint8_t dist, uint32_t _pos);

    void generate_part_maze(uint8_t generation, uint8_t min_dist, uint8_t max_dist);

    void delete_unused_nodes(uint16_t size, uint8_t generator);

    public:
    void _init_();
    void create_generators(uint8_t starting_pos, uint16_t number_of_col, uint16_t number_of_rows);
    void generate_maze(uint8_t number_of_generations, uint8_t min_dist, uint8_t max_dist);
    void delete_nodes();
    uint8_t * get_maze();
    void delete_map();
};


#endif