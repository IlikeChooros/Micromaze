#include "Maze-generator.h"

Maze_generator::Maze_generator(uint8_t number_of_c,uint8_t number_of_r, uint8_t *matrix, uint8_t starting_pos)
{
    _map = matrix;
    _number_of_cols = number_of_c;
    _number_of_rows = number_of_r;

    _starting_pos = starting_pos;

    four_generators = new LinkedList [4];
    randomSeed(analogRead(UNUSED_PIN_ANALOG));
    finished = false;
}

// 0     X     1    X
// Y ------------------
// ||                ||
// ||                ||
// ||                ||
// || 4              ||  2
// ||                ||
// ||                ||
// ||                ||
// ||                ||
// Y ------------------
//            3


void Maze_generator::_init_()
{

    Node *head;

// ------------     1   --  UP    ------------
    uint8_t rand_num = random(7, _number_of_cols-6);
    four_generators[0].add_first(new Point(rand_num,1)); //1

    _starting_pos++;
    head = four_generators[0].get_node_head();
    // _map[head->point->y*_number_of_cols + head->point->x]=2;
    move_wall_down(head,_starting_pos,(head->point->y)*_number_of_cols + head->point->x);


// ------------     2   --  ON RIGHT     ------------
    rand_num = random(7, _number_of_rows-6);
    
    four_generators[1].add_first(new Point(_number_of_cols - 2,rand_num));  //  2
    head = four_generators[1].get_node_head();
    //_map[head->point->y*_number_of_cols + head->point->x]=2;
    move_wall_left(head, _starting_pos, head->point->y * _number_of_cols + head->point->x);


// ------------     3   --  DOWN     ------------
    rand_num = random(7, _number_of_cols-6);

    four_generators[2].add_first(new Point(rand_num, _number_of_rows - 2));//  3
    head = four_generators[2].get_node_head();
    //_map[head->point->y*_number_of_cols + head->point->x]=2;
    move_wall_up(head, _starting_pos, (head->point->y) * _number_of_cols + head->point->x);


// ------------     4   --  ON LEFT     ------------
    rand_num = random(7, _number_of_rows-6);

    four_generators[3].add_first(new Point(1, rand_num)); //  4
    head = four_generators[3].get_node_head();
    //_map[head->point->y*_number_of_cols + head->point->x]=2;
    move_wall_right(head, _starting_pos, head->point->y* _number_of_cols + head->point->x);
}

void Maze_generator::generate_maze()
{
    uint8_t i=0;
    for (;i<5;)
    {
        generate_part_maze(i);
        i++;
    }
}

// private

void Maze_generator::move_wall_down(Node *node, uint8_t dist, uint16_t _pos)
{
    //  (x,y)
    //    O
    //  # X #
    //  # # #
    _pos -= _number_of_cols;

    if (_map[_pos -2 + 2*_number_of_cols] || _map[_pos-1 + 2*_number_of_cols] || _map[_pos + 2 + 2*_number_of_cols] || _map[_pos +1 + 2*_number_of_cols])
    {
        node->down = false;
        goto jump_down;
    }

    for (uint8_t i = 0 ;i<dist;i++)
    {
        for (int8_t j=-2;j<3;j++)
        {
            if (_map[_pos +j + 3*_number_of_cols])
            {
                if (!j)
                {
                    node->up=false;
                }
                goto jump_down;
            }
        }
        _pos += _number_of_cols;
        _map[_pos]=1;
    }

jump_down:
    node->point->y = _pos/_number_of_cols;
    node->point->x = _pos%_number_of_cols;

    _map[_pos] = 3;
    return;
}

void Maze_generator::move_wall_up(Node *node, uint8_t dist, uint16_t _pos)
{
    _pos+=_number_of_cols; 

    if (_map[_pos - 2 - 2*_number_of_cols] || _map[_pos + 2 -2*_number_of_cols] || _map[_pos - 1 - 2*_number_of_cols] || _map[_pos +1 - 2*_number_of_cols])
    {
        node->up = false;
        goto jump_up;
    }

    for (uint8_t i = 0 ;i<dist;i++)
    {
        for (int8_t j=-2;j<3;j++)
        {
            if (_map[_pos +j - 3*_number_of_cols])
            {
                if (!j)
                {
                    node->up=false;
                }
                goto jump_up;
            }
        }
        _pos -= _number_of_cols;
        _map[_pos]=1;
    }

jump_up:
    node->point->y = _pos/_number_of_cols;
    node->point->x = _pos%_number_of_cols;

    _map[_pos]=3;
    return;

    //  _map[_pos -1]=2;
    //  _map[_pos] =2;
    //  _map[_pos + 1]=2;
    // _map[_pos - 1 + _number_of_cols]=2;
    // _map[_pos +1 +_number_of_cols]=2;
}
void Maze_generator::move_wall_left(Node *node, uint8_t dist, uint16_t _pos)
{
    _pos++;

    if (_map[_pos - 2*_number_of_cols - 1] || _map[_pos + 2*_number_of_cols - 1] ||_map[_pos + 2*_number_of_cols - 2]|| _map[_pos - 2*_number_of_cols - 2])
    {
        node->left = false;
        goto jump_left;
    }
        for (uint8_t i = 0 ;i<dist;i++)
        {
            for (int8_t j=-2; j<3;j++)
            {
                if (_map[_pos + j*_number_of_cols - 3]) 
                {
                    if (!j)
                    {
                        node->left=false;
                    }
                    goto jump_left;
                }
            }
            _pos--;
            _map[_pos]=1;

        }

jump_left:
    node->point->y = _pos/_number_of_cols;
    node->point->x = _pos%_number_of_cols;

    _map[_pos]=3;
    return;

}
void Maze_generator::move_wall_right(Node *node, uint8_t dist, uint16_t _pos)
{
    _pos--;


    if ( _map[_pos - 2*_number_of_cols + 1] || _map[_pos + 2*_number_of_cols + 1] || _map[_pos - 2*_number_of_cols + 2] || _map[_pos + 2*_number_of_cols + 2])
    {
        node->right = false;
        goto jump_right;
    }
    for (uint8_t i = 0 ;i<dist;i++)
    {
        for (int8_t j = -2; j<3; j++)
        {
            if (_map[_pos + j*_number_of_cols + 3])
            {
                if (!j)
                {
                    node->right=false;
                }
                goto jump_right;
            }
        }
        _pos++;
        _map[_pos]=1;
    }

jump_right:
    node->point->y = _pos/_number_of_cols;
    node->point->x = _pos%_number_of_cols;

    _map[_pos]=3;
    return;
}

void Maze_generator::check_possible_moves(uint8_t dir, uint8_t generator, Node* node, uint8_t dist)
{
    //   x   x  x
    // y  / / /  
    // y  / X /  
    // y  / / / 
    uint16_t _pos=0;
    Point *temp_point = new Point(0,0);
    switch(dir)
    {                                                                       
        case 0:          // UP               
            _pos = (node->point->y-1) * _number_of_cols + node->point->x;    
            if (_map[_pos]==1)
            {
                delete temp_point;
                return;
            }  
            temp_point->x = node->point->x;
            temp_point->y = node->point->y - 1;

            four_generators[generator].set_size(four_generators[generator].size() + 1);
            four_generators[generator].add_first(temp_point);
            move_wall_up( four_generators[generator].get_node_head(), dist, _pos);
            break;                           
        case 1:         //DOWN
            _pos = (node->point->y+1)*_number_of_cols + node->point->x;
            if (_map[_pos]==1)
            {
                delete temp_point;
                return;
            }
            temp_point->x = node->point->x;
            temp_point->y = node->point->y + 1;

            four_generators[generator].set_size(four_generators[generator].size() + 1);
            four_generators[generator].add_first(temp_point);
            move_wall_down( four_generators[generator].get_node_head(), dist, _pos);
            break;
        case 2:        // LEFT
            _pos = node->point->y * _number_of_cols + node->point->x - 1;
            if (_map[_pos]==1)
            {
                delete temp_point;
                return;
            }
            temp_point->x = node->point->x - 1;
            temp_point->y = node->point->y;

            four_generators[generator].set_size(four_generators[generator].size() + 1);
            four_generators[generator].add_first(temp_point);
            move_wall_left( four_generators[generator].get_node_head(), dist, _pos);
            break;
        case 3:       // RIGHT
            _pos = node->point->y* _number_of_cols + node->point->x + 1;
            if (_map[_pos]==1)
            {
                delete temp_point;
                return;
            }
            temp_point->x = node->point->x + 1;
            temp_point->y = node->point->y;

            four_generators[generator].set_size(four_generators[generator].size() + 1);
            four_generators[generator].add_first(temp_point);
            move_wall_right( four_generators[generator].get_node_head(), dist, _pos);
            return;
    }
}                                                                           

void Maze_generator::generate_part_maze(uint8_t generation)
{

    for (uint8_t i = 0; i<4;i++)
    {
        Node *node = four_generators[i].get_node_head();
        while(node)
        {
            if (!(node->left || node->right || node->up || node->down)) // if the node couldnt 'move' in last iteration, ignore it, move to the next one
            {
                node = node->next;
                break;
            }
            //_map[node->point->y*_number_of_cols + node->point->x]=1;
            for (uint8_t j=0;j<4;j++)
            {
                check_possible_moves(j, i,node, random(3,10));
            }
            node = node->next;
        }
        delete_unused_nodes(generation, i);
   }
}


uint32_t Maze_generator::to_power_3(uint8_t num)
{
    uint32_t n=1;
    if (num != 0)
    {
        for (uint8_t i = 0; i<num;i++)
        {
            n*=3;
        }
    }
    return n;
}

void Maze_generator::delete_unused_nodes(uint8_t generation, uint8_t generator)
{
    uint16_t new_size =four_generators[generator].size();
    
    uint32_t num = to_power_3(generation);

    Serial.println("GEN: "+ String(generation) + ", generator: "+ String(generator) + " to_power: "+ String(num));
    for (uint32_t j = 0; j<num;j++)
    {
        new_size--;
        Point *temp_point = four_generators[generator].poll_last();
        if (!temp_point)
        {
            Serial.println("WARNING!!! -- "+ String(j));
            return;
        }
        Serial.println("    --- deleting point ("+String(temp_point->x)+", "+String(temp_point->y));
        delete temp_point;
    }
    four_generators[generator].set_size(new_size);
}

