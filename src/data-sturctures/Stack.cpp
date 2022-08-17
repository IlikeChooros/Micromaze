#include "Stack.h"

Stack_node::Stack_node(Point* point)
{
    this->point = point;
    this->prev = 0;
}

Stack::Stack()
{
    this->top_node = 0;
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
    top_node = node;               // top_node = 0x02, therefore we succesfully made a 'new' top node
}

Point* Stack::pop()
{
    if (!top_node)
    {
        return 0;
    }

    Stack_node *node = top_node;
    Point* point = node->point;
    top_node = node->prev;
    node->prev=0;

    delete node;
    return point;
}

uint16_t Stack::size()
{
    uint16_t size=0;
    Stack_node *node = top_node;
    while(node)
    {
        size++;
        node = node->prev;
    }
    return size;
}

