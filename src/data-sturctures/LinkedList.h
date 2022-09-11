#ifndef _LINKED_LIST
#define _LINKED_LIST

#include "Arduino.h"
#include "Point.h"

class Node
{
    public:
    Point *point;
    Node *next;
    Node *prev;
    bool left;
    bool right;
    bool up;
    bool down;
    Node(Point *point, bool left, bool right, bool up, bool down);
};

 class LinkedList
 {
    Node *_head;
    Node *_tail;
    uint16_t _max_size;

    uint16_t count(Node *node);
    public:
    LinkedList();
    Point* add_first(Point *point);
    Point* poll_last();
    uint16_t size();
    void set_size(uint16_t max_size);
    Point *get_head();
    Node *get_node_head();
    Node *get_node_tail();
    Point* delete_node(Node *node);
 };

#endif