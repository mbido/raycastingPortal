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

            // If the pixel is black
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
                    std::cout << "For wall n°" << nbWall << " :" << std::endl;
                    for (const auto &vertex : visited)
                    {
                        if(std::find(cellsWalls[nbWall].begin(), cellsWalls[nbWall].end(), vertex) == cellsWalls[nbWall].end()){
                            std::cout << "(" << vertex.x << ", " << vertex.y << ")" << std::endl;
                            cellsWalls[nbWall].push_back(vertex);
                            m_map[vertex.x][vertex.y] = 1;
                        }
                    }
                    ++nbWall;
                }
            }
        }
    }

    std::cout << "nbWall = " << nbWall << std::endl;

    for (auto cellWallsEntry : cellsWalls)
    {
        std::cout << "Before wall push_back" << std::endl;

        std::cout << "Vertices  in cellWalls" << std::endl;
        for(auto v : cellWallsEntry.second){
            std::cout << "(" << v.x << ", " << v.y << ")" << std::endl;
        }
        std::cout << std::endl;

        m_walls.push_back(Wall(cellWallsEntry.second));
        std::cout << "After wall push_back" << std::endl;
    }

    
}

void MapTemplate::depthFirstSearch(gf::Vector2i vertex, std::vector<gf::Vector2i> &visited, std::queue<gf::Vector2i> queue)
{
    // std::cout << "depthFirstSearch" << std::endl;
    queue.push(vertex);
    visited.push_back(vertex);
    while (!queue.empty())
    {
        gf::Vector2i current = queue.front();
        // std::cout << "current = (" << current.x << ", " << current.y << ")" << std::endl;
        // visited.push_back(current);
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
                    
                    // std::cout << "visited :" << std::endl;
                    // for(auto v : visited){
                    //     std::cout << "(" << v.x << ", " << v.y << ")" << std::endl;
                    // }
                    // std::cout << std::endl;
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
                        // if(std::find(visited.begin(), visited.end(), neighbor) != visited.end()){
                        //     std::cout << "neighbor (" << neighbor.x << ", " << neighbor.y <<  ") is in visited" << std::endl;
                        // }
                    }
                }
            }
        }
    }

    // std::cout << "visited :" << std::endl;
    // for(auto v : visited){
    //     std::cout << "(" << v.x << ", " << v.y << ")" << std::endl;
    // }
    // std::cout << std::endl;

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

bool MapTemplate::getSegments(gf::Vector2f point, std::vector<std::pair<gf::Vector2i, gf::Vector2i>> &segments)
{
    for (auto wall : m_walls)
    {
        if (wall.getSegments(point, segments))
        {
            return true;
        }
    }
    std::cout << std::endl << "point : (" << point.x << ", " << point.y << ")" << std::endl;
    return false;
}