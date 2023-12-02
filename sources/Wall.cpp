#include <iostream>
#include <algorithm>
#include <vector>
#include <set>

#include "Wall.hpp"

bool canGo(gf::Vector2i from, char direction, const std::vector<gf::Vector2i> &usefulVertices, gf::Vector2i &where)
{
    int deltaX = 0;
    int deltaY = 0;
    bool isHorizontal; // true -> horizontal, false -> vertical

    switch (direction)
    {
    case 'r':
        deltaX = 1;
        isHorizontal = true;
        break;
    case 'd':
        deltaY = 1;
        isHorizontal = false;
        break;
    case 'l':
        deltaX = -1;
        isHorizontal = true;
        break;
    case 'u':
        deltaY = -1;
        isHorizontal = false;
        break;
    default:
        // Handle invalid direction
        return false;
    }

    where = {-1, -1};

    for (const auto &vx : usefulVertices)
    {
        if (vx == from)
        {
            continue;
        }

        int axisIndex = isHorizontal ? 0 : 1;
        if (vx[axisIndex] != from[axisIndex])
        {
            continue;
        }

        if (((deltaX < 0 || deltaY < 0) && vx[1 - axisIndex] > from[1 - axisIndex]) ||
            ((deltaX > 0 || deltaY > 0) && vx[1 - axisIndex] < from[1 - axisIndex]))
        {
            continue;
        }

        if ((where.x == -1 && where.y == -1) ||
            ((deltaX < 0 || deltaY < 0) && vx[1 - axisIndex] > where[1 - axisIndex]) ||
            ((deltaX > 0 || deltaY > 0) && vx[1 - axisIndex] < where[1 - axisIndex]))
        {
            where = vx;
        }
    }

    return where.x != -1 || where.y != -1;
}

void Wall::setSortedVertices(gf::Vector2i from, std::vector<gf::Vector2i> usefulVertices)
{
    gf::Vector2i currentVertex = from;
    gf::Vector2i nextVertex = {0, 0};
    std::vector<char> directions = {'r', 'd', 'l', 'u'};
    int lastDirection = -1;
    int currentDirection = 0;
    int nbTries = 0;
    do
    {
        if (canGo(currentVertex, directions[currentDirection], usefulVertices, nextVertex))
        {
            if (std::find(vertices.begin(), vertices.end(), nextVertex) != vertices.end())
            {
                break;
            }
            currentVertex = nextVertex;
            vertices.push_back(currentVertex);
            lastDirection = currentDirection;
            nbTries = 0;
        }
        else
        {
            ++nbTries;
        }
        currentDirection = (currentDirection + 1) % 4;
        // Skip the last successful direction
        if (currentDirection == lastDirection)
        {
            currentDirection = (currentDirection + 1) % 4;
        }
    } while (nbTries < 4);
}

Wall::Wall(std::vector<gf::Vector2i> occupiedCells) : wallCells(occupiedCells)
{

    // step 1: creating a list of the useful vertices (cf : notes)
    // step 2: creating the wall's sides by flowing the next algorithm :
    //          a)-we start at the top left vertex of the first cell
    //          b)-we follow the next direction by priority : Right > Down > Left > Up
    //          c)-we repeat the step b) until we reach the top left vertex of the first cell again

    // step 1
    perimetersStart = {};
    std::vector<gf::Vector2i> usefulVertices;

    for (auto cell : occupiedCells)
    {

        // a vertex is useful if he is in a corner => and so if the number of occupied cells around him is 1 or 3
        for (int x = 0; x < 2; ++x)
        {
            for (int y = 0; y < 2; ++y)
            {
                gf::Vector2i vertex = gf::Vector2i(cell.x + x, cell.y + y);

                gf::Vector2i cellUpLeft = gf::Vector2i(vertex.x - 1, vertex.y - 1);
                gf::Vector2i cellUpRight = gf::Vector2i(vertex.x, vertex.y - 1);
                gf::Vector2i cellDownLeft = gf::Vector2i(vertex.x - 1, vertex.y);
                gf::Vector2i cellDownRight = gf::Vector2i(vertex.x, vertex.y);

                int count = 0;
                if (std::find(occupiedCells.begin(), occupiedCells.end(), cellUpLeft) != occupiedCells.end())
                {
                    ++count;
                }
                if (std::find(occupiedCells.begin(), occupiedCells.end(), cellUpRight) != occupiedCells.end())
                {
                    ++count;
                }
                if (std::find(occupiedCells.begin(), occupiedCells.end(), cellDownLeft) != occupiedCells.end())
                {
                    ++count;
                }
                if (std::find(occupiedCells.begin(), occupiedCells.end(), cellDownRight) != occupiedCells.end())
                {
                    ++count;
                }

                if (count == 1 || count == 3)
                {
                    usefulVertices.push_back(vertex);
                }
            }
        }
    }

    int counter = 0;
    // step 2
    while (usefulVertices.size() > vertices.size())
    {

        gf::Vector2i vertex;
        bool found = false;
        for (auto &vx : usefulVertices)
        {
            if (std::find(vertices.begin(), vertices.end(), vx) == vertices.end())
            {
                vertex = vx;
                found = true;
                break;
            }
        }

        if (found)
        {
            perimetersStart.push_back(vertices.size());
            setSortedVertices(vertex, usefulVertices);
        }
        else
        {
            break;
        }
    }

    std::cout
        << "les sommets pour un bloc de murs : " << std::endl;

    for (const auto &vertice : vertices)
    {

        std::cout << "(" << vertice.x << ", " << vertice.y << ")\n";
    }

    std::cout << "\n\n\n";
}

void Wall::render(gf::RenderWindow &window, int scale)
{

    std::size_t size = vertices.size();
    int endAt = size - 1;
    for (int i = perimetersStart.size() - 1; i >= 0; --i)
    {
        int start = perimetersStart[i];
        int subSize = endAt - start + 1;
        for (int j = endAt; j >= start; --j)
        {
            gf::Vector2i currentVertex = vertices[j];
            gf::Vector2i nextVertex = vertices[(j + 1) % subSize + start];
            gf::Line line(currentVertex * scale, nextVertex * scale);
            line.setColor(gf::Color::White);
            window.draw(line);
        }
        endAt = start - 1;
    }
    // for (std::size_t i = 0; i < size; ++i)
    // {
    //     gf::Vector2i currentVertex = vertices[i];
    //     gf::Vector2i nextVertex = vertices[(i + 1) % size];
    //     gf::Line line(currentVertex * scale, nextVertex * scale);
    //     line.setColor(gf::Color::White);
    //     window.draw(line);
    // }
}

std::vector<gf::Vector2i> Wall::getSortedVertices(gf::Vector2f playerPositions)
{
    std::vector<gf::Vector2i> sortedVertices = vertices;
    std::sort(sortedVertices.begin(), sortedVertices.end(), CompareVerticesAngle(playerPositions));
    return sortedVertices;
}

bool Wall::getSegment(gf::Vector2f point, std::pair<gf::Vector2i, gf::Vector2i> &segment)
{
    std::size_t size = vertices.size();
    int endAt = size - 1;
    for (int i = perimetersStart.size() - 1; i >= 0; --i)
    {
        int start = perimetersStart[i];
        int subSize = endAt - start + 1;
        for (int j = endAt; j >= start; --j)
        {
            gf::Vector2i currentVertex = vertices[j];
            gf::Vector2i nextVertex = vertices[(j + 1) % subSize + start];
            if (currentVertex.x == nextVertex.x && currentVertex.x == point.x)
            {
                // the segment is vertical
                if (point.y <= currentVertex.y && point.y >= nextVertex.y || point.y >= currentVertex.y && point.y <= nextVertex.y)
                {
                    segment = {currentVertex, nextVertex};
                    return true;
                }
            }
            else if (currentVertex.y == nextVertex.y && currentVertex.y == point.y)
            {
                // the segment is vertical
                if (point.x <= currentVertex.x && point.x >= nextVertex.x || point.x >= currentVertex.x && point.x <= nextVertex.x)
                {
                    segment = {currentVertex, nextVertex};
                    return true;
                }
            }
        }
        endAt = start - 1;
    }
    return false;
}