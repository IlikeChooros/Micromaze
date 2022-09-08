#include "Stack.h"

Stack_node::Stack_node(Point* point)
{
    this->point = point;
    this->prev = 0;
}

Stack::Stack()
{
    this->top_node = 0;
    this->_size = 0;
}


//  ---------
//  |  top  |
//  ---------
//      | 
//      | prev
//      | 
//  ---------
//  | node  |
//  ---------
//      |
//     ...

void Stack::push(Point *point)
{
    Stack_node *node = new Stack_node(point);    
    node->prev = top_node;         // let top_node = 0x01, node = 0x02, then:   node->prev = 0x01,
    top_node = node;               // top_node = 0x02, succesfully made a 'new' top node
    _size++;
}

Point Stack::pop()
{
    if (!top_node)
    {
        return 0;
    }
    _size--;
    Stack_node *node = top_node;
    Point point (node->point->x,node->point->y);
    top_node = node->prev;
    node->prev=0;

    delete node->point;
    delete node;
    return point;
}

uint16_t Stack::size()
{
    return _size;
}

