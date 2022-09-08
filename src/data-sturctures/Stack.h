#ifndef _STACK_H
#define _STACK_H

#include "Arduino.h"
#include "Point.h"

class Stack_node
{
    public:
    Stack_node(Point* = 0);
    Stack_node *prev;
    Point *point;
};

class Stack
{
    Stack_node *top_node;
    uint16_t _size;

    public:
    Stack();
    void push(Point *point);
    Point pop();
    uint16_t size();
};


#endif