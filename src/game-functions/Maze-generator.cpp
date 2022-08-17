#include "Maze-generator.h"

Maze_generator::Maze_generator(TFT_eSPI *tft)
{
    this->_tft = tft;
    randomSeed(analogRead(UNUSED_PIN_ANALOG));
}

void Maze_generator::load_map(uint8_t number_of_cols, uint8_t number_of_rows)
{
    this->_map = new uint8_t [number_of_cols*number_of_rows];
    memset(_map, 0, number_of_cols*number_of_rows*sizeof(uint8_t));

    this->_number_of_cols = number_of_cols;
    this->_number_of_rows = number_of_rows;

    _number_of_visited = 0;

    path_width = 4 + 1;

    cell_scale.fl_y = _tft->height()/(_number_of_rows);
    cell_scale.fl_y /= path_width;
    cell_scale.fl_x = _tft->width()/(_number_of_cols ); 
    cell_scale.fl_x /= path_width;

    cell_scale.x = cell_scale.fl_x;
    cell_scale.y = cell_scale.fl_y;
}

void Maze_generator::generate_maze(Point *starting_point)
{
    stack.push(starting_point);

    _number_of_visited=1;
    _map[starting_point->y * _number_of_cols + starting_point->x] = CELL_VISITED;

    

}

void Maze_generator::draw_maze()
{
    Point_extended pointer;
    uint32_t color;

    for (uint8_t y=0;y<_number_of_rows;y++)
    {
        for(uint8_t x=0;x<_number_of_cols;x++)
        {
            if (_map[y*_number_of_cols + x])
            {
                color = TFT_DARKCYAN;
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

            Point point;

            if (_map[y*_number_of_cols + x] == CELL_EAST_CONNECTION)
            {
                point.x = pointer.fl_x; 
                point.y = pointer.fl_y + (path_width -1)*cell_scale.y;
                draw_connection_ew(point, color);
            }

            else if (_map[y*_number_of_cols + x] == CELL_WEST_CONNECTION)
            {
                point.x = pointer.fl_x; 
                point.y = pointer.fl_y - cell_scale.y;
                draw_connection_ew(point, color);
            }

            else if (_map[y*_number_of_cols + x] == CELL_NORTH_CONNECTION)
            {
                point.x = pointer.fl_x + (path_width-1)*cell_scale.x; 
                point.y = pointer.fl_y;
                //_tft->fillRect(point.x, point.y, cell_scale.fl_x, cell_scale.fl_y, color);

                draw_connection_ns(point, color);
            }

            else if (_map[y*_number_of_cols + x] == CELL_SOUTH_CONNECTION)
            {
                point.x = pointer.fl_x - cell_scale.x; 
                point.y = pointer.fl_y;
                draw_connection_ns(point, color);
            }


        }
    }
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