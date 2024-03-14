#include "MapTemplate.hpp"
#include <gf/Image.h>
#include <vector>
#include <map>
#include <set>
#include <cmath>
#include <unordered_set>
#include "Wall.hpp"

void MapTemplate::init()
{
    // Initialize the map by setting all the values to 0 and the boundaries to 1

    std::map<int, std::vector<gf::Vector2f>> cellsWalls;
    std::size_t nbWall = 0;

    for (int i = 0; i < m_nbRows; ++i)
    {
        for (int j = 0; j < m_nbColumns; ++j)
        {
            m_map[i][j] = 0;
        }
    }

    for (int i = 0; i < m_nbRows; ++i)
    {
        for (int j = 0; j < m_nbColumns; ++j)
        {

            gf::Color4u pixelColor = m_image.getPixel({i, j});

            // If the pixel is black
            if (static_cast<int>(pixelColor.r) == 0 && static_cast<int>(pixelColor.g) == 0 && static_cast<int>(pixelColor.b) == 0)
            {
                gf::Vector2f current = gf::Vector2f(i, j);
                bool hasFound = false;
                for (int k = 0; k < nbWall; ++k)
                {
                    if (std::find(cellsWalls[k].begin(), cellsWalls[k].end(), current) != cellsWalls[k].end())
                    {
                        hasFound = true;
                        break;
                    }
                }

                if (!hasFound)
                {
                    std::vector<gf::Vector2f> visited;
                    std::queue<gf::Vector2f> queue;
                    depthFirstSearch(current, visited, queue);
                    for (const auto &vertex : visited)
                    {
                        if (std::find(cellsWalls[nbWall].begin(), cellsWalls[nbWall].end(), vertex) == cellsWalls[nbWall].end())
                        {
                            cellsWalls[nbWall].push_back(vertex);
                            m_map[(int)vertex.x][(int)vertex.y] = 1;
                        }
                    }
                    ++nbWall;
                }
            }

            // If the pixel is green
            else if (static_cast<int>(pixelColor.r) == 0 && static_cast<int>(pixelColor.g) == 255 && static_cast<int>(pixelColor.b) == 0)
            {
                m_begin = gf::Vector2f(i, j);
            }

            // If the pixel is red
            else if (static_cast<int>(pixelColor.r) == 255 && static_cast<int>(pixelColor.g) == 0 && static_cast<int>(pixelColor.b) == 0)
            {
                m_arrival = gf::Vector2f(i, j);
            }
        }
    }

    for (auto cellWallsEntry : cellsWalls)
    {
        createWall(cellWallsEntry.second);
    }
}

bool fctCanGo(gf::Vector2f from, char direction, const std::vector<gf::Vector2f> &usefulVertices, gf::Vector2f &where)
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
            // break;
        }
    }
    return where.x != -1 || where.y != -1;
}

void ICanGo(std::vector<gf::Vector2f> &vertices, gf::Vector2f &currentVertex, gf::Vector2f &nextVertex, int &nbTries, int &currentDirection, int &lastDirection)
{
    currentVertex = nextVertex;
    vertices.push_back(currentVertex);
    lastDirection = currentDirection;
    nbTries = 0;
}

bool changeDir(gf::Vector2f from, int **m_map, char currentDir, char currentDir2)
{
    if (m_map[(int)from.x][(int)from.y] == 1)
    {
        return (currentDir2 == 'd' && currentDir == 'u') || (currentDir2 == 'r' && currentDir == 'l');
    }
    if (m_map[(int)from.x][(int)from.y] == 0)
    {
        return (currentDir2 == 'l' && currentDir == 'r') || (currentDir2 == 'u' && currentDir == 'd');
    }
    return false;
}

void MapTemplate::setSortedVertices(gf::Vector2f from, std::vector<gf::Vector2f> usefulVertices, std::vector<gf::Vector2f> &vertices)
{
    gf::Vector2f currentVertex = from;
    gf::Vector2f nextVertex = {0.0f, 0.0f};
    gf::Vector2f nextVertex2 = {0.0f, 0.0f};
    std::vector<char> directions = {'r', 'd', 'l', 'u'};
    int lastDirection = -1;
    int currentDirection = 0;
    int nbTries = 0;
    do
    {
        bool canGo = fctCanGo(currentVertex, directions[currentDirection], usefulVertices, nextVertex);
        bool canGo2 = fctCanGo(currentVertex, directions[(currentDirection + 2) % 4], usefulVertices, nextVertex2);
        int currentDirection2 = (currentDirection + 2) % 4;
        bool br = (canGo && std::find(vertices.begin(), vertices.end(), nextVertex) != vertices.end());
        bool br2 = (canGo2 && std::find(vertices.begin(), vertices.end(), nextVertex2) != vertices.end());
        if (br || br2)
        {
            if ((vertices.size() == 4 && from.x == 0 && from.y == 0) || (vertices.size() == usefulVertices.size() && from.x == 1 && from.y == 1))
            {
                break;
            }

            if ((vertices.size() == usefulVertices.size() && from.x != 0 && from.y != 0))
            {
                break;
            }
        }
        if (canGo && !canGo2)
        {
            ICanGo(vertices, currentVertex, nextVertex, nbTries, currentDirection, lastDirection);
        }
        else if (!canGo && canGo2)
        {
            ICanGo(vertices, currentVertex, nextVertex2, nbTries, currentDirection2, lastDirection);
            currentDirection = currentDirection2;
        }
        else if (canGo2 && canGo)
        {
            if ((std::find(vertices.begin(), vertices.end(), nextVertex2) == vertices.end()) &&
                    changeDir(currentVertex, m_map, directions[currentDirection], directions[currentDirection2]) ||
                (!changeDir(currentVertex, m_map, directions[currentDirection], directions[currentDirection2]) && std::find(vertices.begin(), vertices.end(), nextVertex) != vertices.end()))
            {
                ICanGo(vertices, currentVertex, nextVertex2, nbTries, currentDirection2, lastDirection);
                currentDirection = currentDirection2;
            }
            else
            {
                ICanGo(vertices, currentVertex, nextVertex, nbTries, currentDirection, lastDirection);
            }
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

/**
 * creation of a wall from a set of occupied cells
 * with sorted useful vertices
 */
void MapTemplate::createWall(std::vector<gf::Vector2f> occupiedCells)
{
    // step 1: creating a list of the useful vertices (cf : notes)
    // step 2: creating the wall's sides by flowing the next algorithm :
    //          a)-we start at the top left vertex of the first cell
    //          b)-we follow the next direction by priority : Right > Down > Left > Up
    //          c)-we repeat the step b) until we reach the top left vertex of the first cell again

    // step 1
    std::vector<std::size_t> perimetersSize = {};
    std::vector<gf::Vector2f> vertices, usefulVertices;

    for (auto cell : occupiedCells)
    {

        // a vertex is useful if he is in a corner => and so if the number of occupied cells around him is 1 or 3
        for (int x = 0; x < 2; ++x)
        {
            for (int y = 0; y < 2; ++y)
            {
                gf::Vector2f vertex = gf::Vector2f(cell.x + x, cell.y + y);

                gf::Vector2f cellUpLeft = gf::Vector2f(vertex.x - 1, vertex.y - 1);
                gf::Vector2f cellUpRight = gf::Vector2f(vertex.x, vertex.y - 1);
                gf::Vector2f cellDownLeft = gf::Vector2f(vertex.x - 1, vertex.y);
                gf::Vector2f cellDownRight = gf::Vector2f(vertex.x, vertex.y);

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
                    if (std::find(usefulVertices.begin(), usefulVertices.end(), vertex) == usefulVertices.end())
                    {
                        usefulVertices.push_back(vertex);
                    }
                }
            }
        }
    }

    int counter = 0;
    // step 2
    while (usefulVertices.size() > vertices.size())
    {
        gf::Vector2f vertex;
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
            std::size_t prevSize = vertices.size();
            setSortedVertices(vertex, usefulVertices, vertices);
            perimetersSize.push_back(vertices.size() - prevSize);
        }
        else
        {
            break;
        }
    }
    m_walls.push_back(Wall(vertices, occupiedCells, perimetersSize));
}

void MapTemplate::depthFirstSearch(gf::Vector2f vertex, std::vector<gf::Vector2f> &visited, std::queue<gf::Vector2f> queue)
{
    queue.push(vertex);
    visited.push_back(vertex);
    while (!queue.empty())
    {
        gf::Vector2f current = queue.front();
        queue.pop();

        for (int x = -1; x < 2; ++x)
        {
            for (int y = -1; y < 2; ++y)
            {
                if (abs(x) == abs(y))
                {
                    continue;
                }
                gf::Vector2f neighbor = gf::Vector2f(current.x + x, current.y + y);

                gf::Color4u pixelColor = m_image.getPixel({((int)neighbor.x), ((int)neighbor.y)});

                if (neighbor.x >= 0 && neighbor.y >= 0 && neighbor.x < m_nbColumns && neighbor.y < m_nbRows && static_cast<int>(pixelColor.r) == 0 && static_cast<int>(pixelColor.g) == 0 && static_cast<int>(pixelColor.b) == 0)
                {
                    bool found = false;
                    for (auto it : visited)
                    {
                        if (it.x == neighbor.x && it.y == neighbor.y)
                        {
                            found = true;
                            break;
                        }
                    }

                    if (!found)
                    {
                        queue.push(neighbor);
                        visited.push_back(neighbor);
                    }
                }
            }
        }
    }
}

int MapTemplate::setTile(int i, int j, int value)
{
    if (i >= 0 && i < m_nbColumns && j >= 0 && j < m_nbColumns)
    {
        m_map[j][i] = value;
        return 0;
    }
    else
    {
        return -1;
    }
}

bool MapTemplate::getSegments(gf::Vector2f point, std::vector<std::pair<gf::Vector2f, gf::Vector2f>> &segments)
{
    for (auto wall : m_walls)
    {
        if (wall.getSegments(point, segments))
        {
            return true;
        }
    }
    return false;
}