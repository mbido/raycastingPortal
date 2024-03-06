#include <iostream>
#include <algorithm>
#include <vector>
#include <set>

#include "Wall.hpp"

#define EPSILON 0.001

std::vector<gf::Vector2f> Wall::getWallCells()
{
    return wallCells;
}

Wall::Wall(std::vector<gf::Vector2f> vertices, std::vector<gf::Vector2f> occupiedCells, std::vector<std::size_t> perimetersSize) : wallCells(occupiedCells), vertices(vertices), perimetersSize(perimetersSize)
{
}

void Wall::render(gf::RenderWindow &window, int scale)
{
    gf::Vector2f origin = {0, 0};
    if (std::find(vertices.begin(), vertices.end(), origin) != vertices.end())
    {
        for (std::size_t i = 0; i < 4; ++i)
        {
            gf::Vector2f currentVertex = vertices[i];
            gf::Vector2f prevVertex = vertices[(i + 1) % 4];
            gf::Line line(currentVertex * scale, prevVertex * scale);
            line.setColor(gf::Color::White);
            window.draw(line);
        }
        for (std::size_t i = 4; i < vertices.size(); ++i)
        {
            gf::Vector2f currentVertex = vertices[i];
            gf::Vector2f prevVertex = ((i + 1) % vertices.size() == 0) ? vertices[4] : vertices[(i + 1)];
            gf::Line line(currentVertex * scale, prevVertex * scale);
            line.setColor(gf::Color::White);
            window.draw(line);
        }
    }
    else
    {
        for (std::size_t i = 0; i < vertices.size(); ++i)
        {
            gf::Vector2f currentVertex = vertices[i];
            gf::Vector2f prevVertex = vertices[(i + 1) % vertices.size()];
            gf::Line line(currentVertex * scale, prevVertex * scale);
            line.setColor(gf::Color::White);
            window.draw(line);
        }
    }
}

std::vector<gf::Vector2f> Wall::getVertices()
{
    return vertices;
}

std::vector<gf::Vector2f> Wall::getOccupiedCells()
{
    return wallCells;
}

bool Wall::getSegments(gf::Vector2f point, std::vector<std::pair<gf::Vector2f, gf::Vector2f>> &segments)
{
    gf::Vector2f origin = {0, 0};
    segments.clear();
    if (std::find(vertices.begin(), vertices.end(), origin) != vertices.end())
    {
        for (std::size_t i = 0; i < 4; ++i)
        {
            gf::Vector2f currentVertex = vertices[i];
            gf::Vector2f prevVertex = vertices[(i + 1) % 4];
            bool isVertical = std::abs(point.x - currentVertex.x) < EPSILON && std::abs(point.x - prevVertex.x) < EPSILON;
            bool isHorizontal = std::abs(point.y - currentVertex.y) < EPSILON && std::abs(point.y - prevVertex.y) < EPSILON;
            if (isVertical && ((point.y <= currentVertex.y && point.y >= prevVertex.y) || (point.y >= currentVertex.y && point.y <= prevVertex.y)))
            {
                segments.push_back({currentVertex, prevVertex});
            }
            else if (isHorizontal && ((point.x <= currentVertex.x && point.x >= prevVertex.x) || (point.x >= currentVertex.x && point.x <= prevVertex.x)))
            {
                segments.push_back({currentVertex, prevVertex});
            }
        }
        for (std::size_t i = 4; i < vertices.size(); ++i)
        {
            gf::Vector2f currentVertex = vertices[i];
            gf::Vector2f prevVertex = ((i + 1) % vertices.size() == 0) ? vertices[4] : vertices[(i + 1)];
            bool isVertical = std::abs(point.x - currentVertex.x) < EPSILON && std::abs(point.x - prevVertex.x) < EPSILON;
            bool isHorizontal = std::abs(point.y - currentVertex.y) < EPSILON && std::abs(point.y - prevVertex.y) < EPSILON;
            if (isVertical && ((point.y <= currentVertex.y && point.y >= prevVertex.y) || (point.y >= currentVertex.y && point.y <= prevVertex.y)))
            {
                segments.push_back({currentVertex, prevVertex});
            }
            else if (isHorizontal && ((point.x <= currentVertex.x && point.x >= prevVertex.x) || (point.x >= currentVertex.x && point.x <= prevVertex.x)))
            {
                segments.push_back({currentVertex, prevVertex});
            }
        }
    }
    else
    {
        for (std::size_t i = 0; i < vertices.size(); ++i)
        {
            gf::Vector2f currentVertex = vertices[i];
            gf::Vector2f prevVertex = vertices[(i + 1) % vertices.size()];
            bool isVertical = std::abs(point.x - currentVertex.x) < EPSILON && std::abs(point.x - prevVertex.x) < EPSILON;
            bool isHorizontal = std::abs(point.y - currentVertex.y) < EPSILON && std::abs(point.y - prevVertex.y) < EPSILON;
            if (isVertical && ((point.y <= currentVertex.y && point.y >= prevVertex.y) || (point.y >= currentVertex.y && point.y <= prevVertex.y)))
            {
                segments.push_back({currentVertex, prevVertex});
            }
            else if (isHorizontal && ((point.x <= currentVertex.x && point.x >= prevVertex.x) || (point.x >= currentVertex.x && point.x <= prevVertex.x)))
            {
                segments.push_back({currentVertex, prevVertex});
            }
        }
    }
    return !segments.empty();
}
