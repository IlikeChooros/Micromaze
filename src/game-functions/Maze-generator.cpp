#include "Maze-generator.h"

Maze_generator::Maze_generator(TFT_eSPI *tft)
{
    this->_tft = tft;
    randomSeed(analogRead(UNUSED_PIN_ANALOG));
}

void Maze_generator::load_map(uint8_t number_of_cols, uint8_t number_of_rows)
{
    this->_map = new uint8_t* [number_of_cols*number_of_rows];
    for (uint16_t i=0;i<number_of_cols*number_of_rows;i++)
    {
        _map[i] = new uint8_t [3]; 
        for (uint8_t j=0;j<3;j++)
        {
            _map[i][j] = 0;
        }
    }

    this->_number_of_cols = number_of_cols;
    this->_number_of_rows = number_of_rows;

    path_width = 4 + 1;

    cell_scale.fl_y = _tft->height()/(_number_of_rows);
    cell_scale.fl_y /= path_width;
    cell_scale.fl_x = _tft->width()/(_number_of_cols ); 
    cell_scale.fl_x /= path_width;

    cell_scale.x = cell_scale.fl_x;
    cell_scale.y = cell_scale.fl_y;
}

void Maze_generator::clear()
{
    for (uint16_t i=0;i<_number_of_cols*_number_of_rows;i++)
    {
        delete [] _map[i];
    }
    delete [] _map;
}

void Maze_generator::generate_maze(Point *starting_point)
{
    stack.push(starting_point);

    uint8_t min, max, exit_possiblity;
    bool finish = false;

    uint8_t scale;
    switch(_number_of_rows)
    {
        case 64:
            scale = 200;
            min = 120;
            max = 200;
            break;
        case 32:
            scale = 32;
            min = 30;
            max = 50;
            break;
        case 16:
            scale = 8;
            min = 15;
            max = 25;
            break;
        case 8:
            min = 2;
            max = 4;
            scale = 2;
            break;
    }

    exit_possiblity = random(min, max);

    _number_of_visited=0;

    uint8_t possible_moves[4]={0};

    Point point (starting_point->x, starting_point->y);

    _map[convert_to_coordinates(point)][0] = CELL_START;

    Point *ptr_point;

    while(_number_of_visited < _number_of_cols*_number_of_rows)
    {

        //Serial.println("NUM_OF_VISITED: "+String(_number_of_visited) + "   SIZE: "+ String(stack.size()) +"   POINT: ("+String(point.x)+", "+ String(point.y)+ ")");
        uint16_t coordinates = convert_to_coordinates(point);

        if (stack.size()==0)
        {
            return;
        }

        for(uint8_t i=0;i<4;i++)
            possible_moves[i]=0;

        _map[coordinates][0] = _map[coordinates][0] == 0 ? CELL_VISITED : _map[coordinates][0];
        
        if (_number_of_visited%(scale)==0)
        {
            draw_maze();
        }
        // NORTH
        if (point.x<_number_of_cols -1)
        {
            point.x += 1;
            if (!_map[convert_to_coordinates(point)][0])
            {
                possible_moves[CELL_NORTH_CONNECTION-1]=1;
                //Serial.println("NORTH");
            }
                
            point.x -=1;
        }

        // SOUTH
        if(point.x>0)
        {
            point.x -= 1;
            if (!_map[convert_to_coordinates(point)][0])
            {
                possible_moves[CELL_SOUTH_CONNECTION-1]=1;
                //Serial.println("SOUTH");
            }
                
            point.x +=1;
        }

        //EAST
        if(point.y<_number_of_rows -1)
        {
            point.y += 1;
            if (!_map[convert_to_coordinates(point)][0])
            {
                possible_moves[CELL_EAST_CONNECTION-1]=1;
                //Serial.println("EAST");
            }
                
            point.y -=1;
        }

        //WEST
        if(point.y>0)
        {
            point.y -= 1;
            if (!_map[convert_to_coordinates(point)][0])
            {
                possible_moves[CELL_WEST_CONNECTION-1]=1;
                //Serial.println("WEST");
            }
                
            point.y +=1;
        }

        uint8_t counter=0;
        for (uint8_t i=0;i<4;i++)
        {
            if (possible_moves[i])
                counter++;
        }
        //Serial.println("COUNTER "+String(counter));
        if (counter>0)
        {
            int8_t random_num = random(counter);
            uint8_t next_dir;
            uint8_t x = point.x,y = point.y;

            for (uint8_t i=0;i<4;i++)
            {
                if (possible_moves[i])
                {
                    if (random_num == 0)
                    {
                        next_dir = i+1;
                        break;
                    }
                    random_num--;
                }
            }

            // Writing to cell its connection to a free cell
            for (uint8_t i=0;i<3;i++)
            {
                if (_map[coordinates][i] == 0 || _map[coordinates][i] == CELL_VISITED)
                {
                    _map[coordinates][i]=next_dir;
                    break;
                }
            }
            
            //Serial.print("NEXT_DIR "+String(next_dir));
            switch(next_dir)
            {
                case CELL_NORTH_CONNECTION:
                    //Serial.print("    NORTH");
                    ptr_point = new Point(x+1,y);
                    point.x++;
                    break;
                case CELL_SOUTH_CONNECTION:
                    //Serial.print("    SOUTH");
                    ptr_point = new Point(x-1,y);
                    point.x--;
                    break;
                case CELL_EAST_CONNECTION:
                    //Serial.print("    EAST");
                    ptr_point = new Point(x, y+1);
                    point.y++;
                    break;
                case CELL_WEST_CONNECTION:
                    //Serial.print("    WEST");
                    ptr_point = new Point(x, y-1);
                    point.y--;
                    break;
                default:
                    break;
            }


            //Serial.println("  XXX");
            stack.push(ptr_point);
            _number_of_visited++;
        }
        else{
            //Serial.print(String(_number_of_visited));
            if (_map[coordinates][0] == CELL_VISITED)
            {
                if (!finish)
                {
                    if (exit_possiblity == 0)
                    {
                        _map[coordinates][0] = CELL_FINISH;
                        finish = true;
                    }
                    exit_possiblity--;
                }
            }
            point = stack.pop();
            //Serial.println("     ---SUCCES");
        }
    }


    // Serial.println("AFTER CLEARING: "+String(stack.size()));
    // Serial.println("");
    // Serial.println("-------------------------------------------");
}

void Maze_generator::draw_maze()
{
    Point_extended pointer;
    uint32_t color;

    uint16_t coordinates;

    for (uint8_t y=0;y<_number_of_rows;y++)
    {
        for(uint8_t x=0;x<_number_of_cols;x++)
        {
            coordinates = y*_number_of_cols + x;

            //Checking if the cell is occupied, useless if theres no real time maze building display
            if(_map[coordinates][0] == CELL_FINISH)
            {
                color = TFT_RED;
            }
            else if (_map[coordinates][0] == CELL_START)
            {
                color = TFT_GREEN;
            }

            else if (_map[coordinates][0])
            {
                color = TFT_BLACK;
            }
            else{
                color = TFT_BLUE;
            }

            pointer.fl_x = x*cell_scale.fl_x*path_width;
            pointer.fl_x = round(pointer.fl_x);

            pointer.fl_y = y*cell_scale.fl_y*path_width;
            pointer.fl_y = round(pointer.fl_y);

            for (uint8_t y1 = 0; y1 < (path_width-1)*cell_scale.y; y1 += cell_scale.y)
            {
                for (uint8_t x1 = 0; x1 < (path_width-1)*cell_scale.x; x1 += cell_scale.x)
                {
                    _tft->fillRect(pointer.fl_x + x1, pointer.fl_y + y1, cell_scale.fl_x, cell_scale.fl_y, color);
                }
            }
            

            // Checking if there is any connection to another cell
            if (_map[coordinates][0] != CELL_VISITED && _map[coordinates][0] != 0) 
            {
                Point point;

                for (uint8_t i=0;i<3; i++)
                {
                    if (_map[coordinates][i] == CELL_EAST_CONNECTION)
                    {
                        point.x = pointer.fl_x; 
                        point.y = pointer.fl_y + (path_width -1)*cell_scale.y;
                        draw_connection_ew(point, color);
                    }

                    else if (_map[coordinates][i] == CELL_WEST_CONNECTION)
                    {
                        point.x = pointer.fl_x; 
                        point.y = pointer.fl_y - cell_scale.y;
                        draw_connection_ew(point, color);
                    }

                    else if (_map[coordinates][i] == CELL_NORTH_CONNECTION)
                    {
                        point.x = pointer.fl_x + (path_width-1)*cell_scale.x; 
                        point.y = pointer.fl_y;
                        draw_connection_ns(point, color);
                    }

                    else if (_map[coordinates][i] == CELL_SOUTH_CONNECTION)
                    {
                        point.x = pointer.fl_x - cell_scale.x; 
                        point.y = pointer.fl_y;
                        draw_connection_ns(point, color);
                    }
                }
            }
        }
    }
}

// private

uint16_t Maze_generator::convert_to_coordinates(Point point)
{
    return point.y*_number_of_cols+point.x;
}

void Maze_generator::draw_connection_ew(Point point, uint32_t color)
{
    for (uint8_t p = 0; p<(path_width-1)*cell_scale.x;p+=cell_scale.x)
    {
        _tft->fillRect(point.x+p, point.y, cell_scale.fl_x, cell_scale.fl_y, color);
    }
}

void Maze_generator::draw_connection_ns(Point point, uint32_t color)
{
    for (uint8_t p = 0; p<(path_width-1)*cell_scale.y; p+=cell_scale.y)
    {
        _tft->fillRect(point.x, point.y+p, cell_scale.fl_x, cell_scale.fl_y, color);
    }
}