#ifndef WALL_HPP
#define WALL_HPP

#include <vector>

struct Vertex
{
    int x;
    int y;
    Vertex *next;
    Vertex *prev;
};


class Wall
{
public:
    // Constructor
    Wall(std::vector<std::pair<int, int>> occupiedCells);

    // Destructor
    ~Wall();

private:
    std::vector<std::pair<int, int>> vertices;
    std::vector<std::pair<int, int>> wallCells;
};

#endif // WALL_HPP
