#include <iostream>

#include "Wall.hpp"

Wall::Wall(std::vector<std::pair<int, int>> occupiedCells) : wallCells(occupiedCells)
{
    head = new Vertex;
    tail = new Vertex;
    head->next = tail;
    head->prev = tail;
    tail->next = head;
    tail->prev = head;

    // step 1: sorting the occupied cells by x and y (cf : notes)
    // step 2: creating a list of the useful vertices (cf : notes)
    // step 3: creating the wall's sides by flowing the next algorithm :
    //          a)-we start at the top left vertex of the first cell
    //          b)-we follow the next direction by priority : Right > Down > Left > Up
    //          c)-we repeat the step b) until we reach the top left vertex of the first cell again
}

Wall::~Wall()
{
    // Destructor implementation
}

// Other member function implementations
