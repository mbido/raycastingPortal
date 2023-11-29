#ifndef WALL_HPP
#define WALL_HPP

#include <gf/Vector.h>
#include <gf/RenderWindow.h>
#include <gf/Shapes.h>
#include <gf/Curves.h>

#include <vector>

#include "CompareVerticesAngle.hpp"

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
    Wall(std::vector<gf::Vector2i> occupiedCells);

    std::vector<gf::Vector2i> getSortedVertices(gf::Vector2f playerPositions);

    void render(gf::RenderWindow& window, int scale);

private:
    std::vector<gf::Vector2i> vertices;
    std::vector<gf::Vector2i> wallCells;
};

#endif // WALL_HPP
