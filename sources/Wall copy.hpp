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

    // std::vector<gf::Vector2i> getSortedVertices(gf::Vector2f playerPositions);

    std::vector<gf::Vector2i> getVertices();

    void render(gf::RenderWindow &window, int scale);

    bool getSegments(gf::Vector2f point, std::vector<std::pair<gf::Vector2i, gf::Vector2i>> &segments);

    std::vector<gf::Vector2i> getWallCells();

private:
    void setSortedVertices(const gf::Vector2i from, std::vector<gf::Vector2i> usefulVertices);

    std::vector<gf::Vector2i> vertices;
    std::vector<gf::Vector2i> wallCells;
    std::vector<std::size_t> perimetersSize;
};

#endif // WALL_HPP
