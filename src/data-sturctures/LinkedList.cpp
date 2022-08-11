#include "LinkedList.h"

Node::Node(Point *point,bool left, bool right, bool up, bool down)
{
    this->point = point;
    this->next = 0;
    this->prev = 0;
    this->left = left;
    this->down = down;
    this->right = right;
    this->up = up;
}

LinkedList::LinkedList() // tail = next <- head -> prev NULL
{
    _max_size=1;
    this->_head = 0;
    this->_tail = 0;
}

Point* LinkedList::add_first(Point *point)
{
    Node *node = new Node(point, true, true, true, true);

    if (!(_head||_tail))
    {
        _head = node;
        _tail = node;
    }
    else
    {
        node->next = _head;
        _head->prev = node;
        _head = node;
    }

    if (this->size() > this->_max_size)
    {
        return this->poll_last();
    }
    return 0;
}

Point *LinkedList::poll_last()
{
    if (!_tail)
    {
        return 0;
    }
    Node *last = _tail;
    if (_tail == _head)
    {
        _tail = 0;
        _head = 0;
    }
    else
    {
        _tail = last->prev;
        _tail->next = 0;
        last->prev = 0;
    }
    Point* lastPoint = last->point;

    delete last;
    return lastPoint;
}

Point *LinkedList::delete_node(Node* node) // | tail | <- -> | last |  ...  | next | <- | node | -> | prev | ... | head |
{
    if (!_head)
    {
        return 0;
    }
    if (node == _head && !_tail)
    {
        _head = 0;
        return 0;
    }

    Point *point_to_delte;
    Node *next_node = node->next; 
    Node *prev_node = node->prev;

    if (!next_node)// node is tail
    {
        return poll_last();
    }

    if (node == _head)
    {
        _head = node->next;
        node->next = 0;
        point_to_delte = node->point;
        delete node;
        return point_to_delte;
    }

    next_node->prev = prev_node;
    prev_node->next = next_node;
    node->next = 0;
    node->prev = 0;
    point_to_delte = node->point;

    delete node;
    return point_to_delte;
}

uint16_t LinkedList::size()
{
    return !this->_head ? 0:1+this->count(this->_head->next);
}

uint16_t LinkedList::count(Node *node)
{
    return !node ? 0 : 1 + this->count(node->next);
}

void LinkedList::set_size(uint16_t max_size)
{
    this->_max_size = max_size;
}

Point *LinkedList::get_head()
{
    return !this->_head ? 0 : this->_head->point;
}

Node *LinkedList::get_node_head()
{
    return this->_head;
}

Node *LinkedList::get_node_tail()
{
    return this->_tail;
}
