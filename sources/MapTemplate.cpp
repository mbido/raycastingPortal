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

    std::map<int, std::vector<gf::Vector2i>> cellsWalls;
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

            if (static_cast<int>(pixelColor.r) == 0 && static_cast<int>(pixelColor.g) == 0 && static_cast<int>(pixelColor.b) == 0)
            {
                gf::Vector2i current = gf::Vector2i(i, j);
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
                    std::vector<gf::Vector2i> visited;
                    std::queue<gf::Vector2i> queue;
                    depthFirstSearch(current, visited, queue);

                    for (const auto &vertex : visited)
                    {
                        cellsWalls[nbWall].push_back(vertex);
                        m_map[vertex.x][vertex.y] = 1;
                    }
                    ++nbWall;
                }
            }
        }
    }

    for (auto cellWallsEntry : cellsWalls)
    {
        m_walls.push_back(Wall(cellWallsEntry.second));
    }
}

void MapTemplate::depthFirstSearch(gf::Vector2i vertex, std::vector<gf::Vector2i> &visited, std::queue<gf::Vector2i> queue)
{
    queue.push(vertex);

    while (!queue.empty())
    {
        gf::Vector2i current = queue.back();
        visited.push_back(current);
        queue.pop();

        for (int x = -1; x < 2; ++x)
        {
            for (int y = -1; y < 2; ++y)
            {
                if (abs(x) == abs(y))
                {
                    continue;
                }
                gf::Vector2i neighbor = gf::Vector2i(current.x + x, current.y + y);
                gf::Color4u pixelColor = m_image.getPixel({(neighbor.x), (neighbor.y)});

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

bool MapTemplate::getSegment(gf::Vector2f point, std::pair<gf::Vector2i, gf::Vector2i> &segment)
{
    for (auto wall : m_walls)
    {
        if (wall.getSegment(point, segment))
        {
            return true;
        }
    }
    std::cout << std::endl << "point : (" << point.x << ", " << point.y << ")" << std::endl;
    return false;
}