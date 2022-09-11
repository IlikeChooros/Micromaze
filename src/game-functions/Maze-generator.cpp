#include "Maze-generator.h"

void Maze_generator::_init_()
{
    four_generators = new LinkedList [4];
    randomSeed(analogRead(UNUSED_PIN_ANALOG));
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


void Maze_generator::create_generators(uint8_t starting_pos, uint8_t number_of_col, uint8_t number_of_rows)
{
    _number_of_cols = number_of_col;
    _number_of_rows = number_of_rows;
    _starting_pos = starting_pos;
    _map = new uint8_t[number_of_col*number_of_rows];

    for (uint16_t i=0;i<number_of_rows;i++)
    {
        for (uint16_t j=0;j<number_of_col;j++)
        {
            _map[i*number_of_col + j]=0;
        }
    }

    for (uint16_t y = 0; y<number_of_rows;y++)
    {
        _map[y * number_of_col] = 1;
        _map[y*number_of_col + number_of_col - 1] = 1;
    }

    uint16_t p = number_of_col*number_of_rows;
    for (uint16_t x = 0; x<number_of_col;x++)
    {
        _map[x]=1;
        _map[p - 1 - x] = 1;
    }

    Node *head;

// ------------     1   --  UP    ------------
    uint8_t rand_num = random(7, _number_of_cols-6);
    for(uint8_t i = 0;i<4;i++)
    {
        four_generators[i].set_size(1);
    }
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

uint8_t *Maze_generator::get_maze()
{
    return _map;
}

void Maze_generator::delete_map()
{
    delete [] _map;
}


void Maze_generator::generate_maze(uint8_t num_of_gen, uint8_t min_dist, uint8_t max_dist)
{
    _number_of_generations = num_of_gen;

    if (!_map)
    {
        return;
    }
    
    uint8_t i=0;
    for (;i<num_of_gen;)
    {
        generate_part_maze(i,min_dist, max_dist);
        i++;
    }
}

void Maze_generator::delete_nodes()
{
    for (uint8_t i=0;i<4;i++)
    {
        //Serial.println("GEN: "+String(i));
        //Serial.println("BEFORE SIZE: "+String(four_generators[i].size()));
        delete_unused_nodes(four_generators[i].size(),i);
        //Serial.println("AFTER SIZE: "+ String(four_generators[i].size()));
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
                if (!i)
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
                if (!i)
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
                    if (!i)
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
                if (!i)
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
                node->up = false;
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
                node->down = false;
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
                node->left = false;
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
                node->right = false;
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

void Maze_generator::generate_part_maze(uint8_t generation, uint8_t min_dist, uint8_t max_dist)
{
    uint16_t counter = 1;
    for (uint8_t i = 0; i<4;i++)
    {
        Node *node = four_generators[i].get_node_head();
        while(node)
        {
            if ( !((node->left && node->right) && (node->up && node->down))) // if the node couldnt 'move' in last iteration, ignore it, move to the next one
            {
                //Serial.print("GENERATOR: "+String(i)+"  size: "+String(four_generators[i].size()) +  " --- deleting node: " + String(counter));
                Point* point;
                point = four_generators[i].poll_last();
                //Serial.println("  ("+String(point->x)+", "+String(point->y));
                if(point)
                {
                    four_generators[i].set_size(four_generators[i].size()-1);
                    delete point; 
                }
                counter++;
            }
            else{
                for (uint8_t j=0;j<4;j++)
                {
                    check_possible_moves(j, i,node, random(min_dist,max_dist));
                }
            }
            node = node->next;
        }
   }
}

void Maze_generator::delete_unused_nodes(uint16_t size, uint8_t generator)
{
    uint16_t new_size =four_generators[generator].size();

    //Serial.println("Before: "+String(new_size));
    //Serial.println("DELETING: "+ String(size) + ", generator: "+ String(generator));
    for (uint32_t j = 0; j<size;j++)
    {
        Point *temp_point = four_generators[generator].poll_last();
        if (!temp_point)
        {
            //Serial.println("Succesfully delted: " + String(j));
            //Serial.println("After: "+ String(new_size));
            //Serial.println("");
            four_generators[generator].set_size(new_size);
            return;
        }
        new_size--;
        delete temp_point;
    }
    //Serial.println("Succesfully delted: " + String(size));
    //Serial.println("After: "+ String(new_size));
    //Serial.println("");
    four_generators[generator].set_size(new_size);
}



