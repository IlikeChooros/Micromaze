#include "Maze-generator.h"

Maze_generator::Maze_generator(uint8_t number_of_c,uint8_t number_of_r, uint8_t *matrix)
{
    _map = matrix;
    _number_of_cols = number_of_c;
    _number_of_rows = number_of_r;

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
    uint8_t rand_num = random(7, _number_of_cols-6);
    _map[_number_of_cols + rand_num]=1;
    _map[_number_of_cols*2+rand_num]=1;
    four_generators[0].add_first(new Point(rand_num,2));  // 1

    rand_num = random(7, _number_of_rows-6);
    _map[(rand_num+1)*_number_of_cols - 2]=1;
    _map[(rand_num+1)*_number_of_cols - 3]=1;
    four_generators[1].add_first(new Point(_number_of_cols - 3,rand_num));  //  2

    rand_num = random(7, _number_of_cols-6);
    _map[(_number_of_rows-2)*_number_of_cols + rand_num]=1;
    _map[(_number_of_rows-3)*_number_of_cols + rand_num]=1;
    four_generators[2].add_first(new Point(rand_num, _number_of_rows - 3));//  3

    rand_num = random(7, _number_of_rows-6);
    _map[rand_num*_number_of_cols + 1]=1;
    _map[rand_num*_number_of_cols + 2]=1;
    four_generators[3].add_first(new Point(2, rand_num)); //  4
}

void Maze_generator::generate_maze()
{
    for (uint8_t i = 0;i < 3; i++)
    {
        generate_part_maze();
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
    for (uint8_t i = 0 ;i<dist;i++)
    {
        if (_map[_pos - 1 + 2*_number_of_cols] || _map[_pos + 2*_number_of_cols] || _map[_pos+1+ 2*_number_of_cols] || _map[_pos - 1 + _number_of_cols] || _map[_pos + 1 + _number_of_cols])
        {
            if (!i)
            {
                node->down=false;
            }
            break;
        }
        else {
            _map[_pos]=1; 
            _pos += _number_of_cols;
        }
    }
    

    node->point->y = _pos/_number_of_cols;
    node->point->x = _pos%_number_of_cols;

    _map[node->point->y*_number_of_cols + node->point->x]=2;

    // _map[_pos -1]=2;
    // _map[_pos] =2;
    // _map[_pos + 1]=2;
    // _map[_pos - 1 - _number_of_cols]=2;
    // _map[_pos +1 - _number_of_cols]=2;
}

void Maze_generator::move_wall_up(Node *node, uint8_t dist, uint16_t _pos)
{
    _pos+=_number_of_cols; 
    for (uint8_t i = 0 ;i<dist;i++)
    {
        if ( (_map[_pos - 1 - 2*_number_of_cols] || _map[_pos - 2*_number_of_cols]) || (_map[_pos + 1 - 2*_number_of_cols] || _map[_pos - 1 - _number_of_cols]) || _map[_pos +1 - _number_of_cols] ) //
        {
            if (!i)
            {
                node->up=false;
            }
            break;
        }
        else {
            _pos -= _number_of_cols;
            _map[_pos]=1;
        }
    }

    node->point->y = _pos/_number_of_cols;
    node->point->x = _pos%_number_of_cols;

    _map[node->point->y*_number_of_cols + node->point->x]=2;

    //  _map[_pos -1]=2;
    //  _map[_pos] =2;
    //  _map[_pos + 1]=2;
    // _map[_pos - 1 + _number_of_cols]=2;
    // _map[_pos +1 +_number_of_cols]=2;
}
void Maze_generator::move_wall_left(Node *node, uint8_t dist, uint16_t _pos)
{
    _pos++;
    for (uint8_t i = 0 ;i<dist;i++)
    {
        if (_map[_pos - _number_of_cols - 2] || _map[_pos - 2] || _map[_pos + _number_of_cols - 2] || _map[_pos - _number_of_cols - 1] || _map[_pos + _number_of_cols - 1]) 
        {
            if (!i)
            {
                node->left=false;
            }
            break;
        }
        else {
            _pos--;
            _map[_pos]=1;
        }
    }

    node->point->y = _pos/_number_of_cols;
    node->point->x = _pos%_number_of_cols;

    _map[node->point->y*_number_of_cols + node->point->x]=2;

    // _map[_pos - _number_of_cols]=2;
    // _map[_pos]=2;
    // _map[_pos + _number_of_cols] = 2;
    // _map[_pos - _number_of_cols + 1] = 2;
    // _map[_pos + _number_of_cols + 1] = 2;
}
void Maze_generator::move_wall_right(Node *node, uint8_t dist, uint16_t _pos)
{
    _pos--;
    for (uint8_t i = 0 ;i<dist;i++)
    {
        if (_map[_pos - _number_of_cols + 2] || _map[_pos + 2] || _map[_pos +_number_of_cols + 2] || _map[_pos - _number_of_cols + 1] || _map[_pos + _number_of_cols + 1])
        {
            if (!i)
            {
                node->right=false;
            }
            break;
        }
        else {
            _pos++;
            _map[_pos]=1;
        }
    }

    node->point->y = _pos/_number_of_cols;
    node->point->x = _pos%_number_of_cols;

    _map[node->point->y*_number_of_cols + node->point->x]=2;

    // _map[_pos - _number_of_cols]=2;
    // _map[_pos]=2;
    // _map[_pos + _number_of_cols] = 2;
    // _map[_pos - _number_of_cols - 1] = 2;
    // _map[_pos + _number_of_cols - 1] = 2;       

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

void Maze_generator::generate_part_maze()
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
   }
}

