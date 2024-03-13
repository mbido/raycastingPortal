#ifndef WALL_HPP
#define WALL_HPP

#include <gf/Vector.h>
#include <gf/RenderTarget.h>
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
    Wall(std::vector<gf::Vector2f> vertices, std::vector<gf::Vector2f> occupiedCells, std::vector<std::size_t> perimetersSize);

    // std::vector<gf::Vector2f> getSortedVertices(gf::Vector2f playerPositions);

    std::vector<gf::Vector2f> getVertices();
    std::vector<gf::Vector2f> getOccupiedCells();

    void render(gf::RenderTarget &window, int scale);

    bool getSegments(gf::Vector2f point, std::vector<std::pair<gf::Vector2f, gf::Vector2f>> &segments);

    std::vector<gf::Vector2f> getWallCells();
private:
    // void setSortedVertices(const gf::Vector2f from, std::vector<gf::Vector2f> usefulVertices);

    std::vector<gf::Vector2f> vertices;
    std::vector<gf::Vector2f> wallCells;
    std::vector<std::size_t> perimetersSize;
};



#endif // WALL_HPP
