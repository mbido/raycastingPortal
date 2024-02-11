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
                    // std::cout << "For wall n°" << nbWall << " :" << std::endl;
                    for (const auto &vertex : visited)
                    {
                        if(std::find(cellsWalls[nbWall].begin(), cellsWalls[nbWall].end(), vertex) == cellsWalls[nbWall].end()){
                            // std::cout << "(" << vertex.x << ", " << vertex.y << ")" << std::endl;
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
                m_map[i][j] = 2;
                m_begin = gf::Vector2f(i,j);
            }

            // If the pixel is red
            else if (static_cast<int>(pixelColor.r) == 255 && static_cast<int>(pixelColor.g) == 0 && static_cast<int>(pixelColor.b) == 0)
            {
                m_map[i][j] = 3;
                m_arrival = gf::Vector2f(i,j);
            }


        }
    }

    // std::cout << "nbWall = " << nbWall << std::endl;

    for (auto cellWallsEntry : cellsWalls)
    {
        // std::cout << "Before wall push_back" << std::endl;

        // std::cout << "Vertices  in cellWalls" << std::endl;
        // for(auto v : cellWallsEntry.second){
        //     std::cout << "(" << v.x << ", " << v.y << ")" << std::endl;
        // }
        // std::cout << std::endl;
        createWall(cellWallsEntry.second);
        // m_walls.push_back(Wall(cellWallsEntry.second));
        // std::cout << "After wall push_back" << std::endl;
    }

    // for(std::size_t i = 0; i < m_walls.size(); ++i){
    //     std::cout << "In Wall n°" << i << std::endl;
    //     std::cout << "Vertices  in wall" << std::endl;
    //     for(auto v : m_walls[i].getVertices()){
    //         std::cout << "(" << v.x << ", " << v.y << ")" << std::endl;
    //     }
    //     std::cout << std::endl;
    // }    
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
        // std::cout << "vx: (" << vx.x << ", " << vx.y << ")" << std::endl;
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
            //break;
        }
    }
    // std::cout << "from (" << from.x << ", " << from.y << ")" << std::endl
    //           << "direction: " << direction << std::endl
    //           << "where (" << where.x << ", " << where.y << ")" << std::endl;
    // std::cout << "canGo?: " << (where.x != -1 || where.y != -1) << std::endl;
    return where.x != -1 || where.y != -1;
}

void ICanGo(std::vector<gf::Vector2f>& vertices, gf::Vector2f& currentVertex, gf::Vector2f& nextVertex, int& nbTries, int& currentDirection, int& lastDirection){
    currentVertex = nextVertex;
    vertices.push_back(currentVertex);
    lastDirection = currentDirection;
    nbTries = 0;
}

bool changeDir(gf::Vector2f from, int **m_map, char currentDir, char currentDir2){
    // std::cout << "currentDir: " << currentDir << std::endl
    //             << "currentDir2: " << currentDir2 << std::endl;
    if(m_map[(int)from.x][(int)from.y] == 1){
        return (currentDir2 == 'd' && currentDir == 'u') || (currentDir2 == 'r' && currentDir == 'l');
    }
    if(m_map[(int)from.x][(int)from.y] == 0){
        return (currentDir2 == 'l' && currentDir == 'r') || (currentDir2 == 'u' && currentDir == 'd');
    }
    return false;
}

void MapTemplate::setSortedVertices(gf::Vector2f from, std::vector<gf::Vector2f> usefulVertices, std::vector<gf::Vector2f>& vertices)
{
    gf::Vector2f currentVertex = from;
    gf::Vector2f nextVertex = {0.0f, 0.0f};
    gf::Vector2f nextVertex2 = {0.0f, 0.0f};
    // gf::Vector2f lastVertex = {0, 0};
    std::vector<char> directions = {'r', 'd', 'l', 'u'};
    int lastDirection = -1;
    int currentDirection = 0;
    int nbTries = 0;
    // std::cout << "FROM: (" << from.x << ", " << from.y << ") usefulVertices.size() = " << usefulVertices.size() << std::endl;
    do
    {
        bool canGo = fctCanGo(currentVertex, directions[currentDirection], usefulVertices, nextVertex);
        bool canGo2 = fctCanGo(currentVertex, directions[(currentDirection + 2) % 4], usefulVertices, nextVertex2);
        int currentDirection2 = (currentDirection + 2) % 4;
        bool br = (canGo && std::find(vertices.begin(), vertices.end(), nextVertex) != vertices.end());
        bool br2 = (canGo2 && std::find(vertices.begin(), vertices.end(), nextVertex2) != vertices.end());
        // std::cout << "br = " << br << std::endl << "br2 = " << br2 << std::endl;
        if(br || br2){
            // std::cout << "break" << std::endl;
            // break;
            if((vertices.size() == 4 && from.x == 0 && from.y == 0) || (vertices.size() == usefulVertices.size() && from.x == 1 && from.y == 1)){
                // std::cout << "break" << std::endl;
                // std::cout << "vertices.size() = " << vertices.size() << std::endl;
                // std::cout << "EXTERIEUR" << std::endl;
                break;
            }

            if((vertices.size() == usefulVertices.size() && from.x != 0 && from.y != 0)){
                // std::cout << "break" << std::endl;
                // std::cout << "NON EXTERIEUR" << std::endl;
                // vertices.push_back(vertices.at(0));
                break;
            }
        }
        // if((br && !br2 && canGo2) || (br2 && !br && canGo)){
            if(canGo && !canGo2){
                ICanGo(vertices, currentVertex, nextVertex, nbTries, currentDirection, lastDirection);
            }else if(!canGo && canGo2){
                ICanGo(vertices, currentVertex, nextVertex2, nbTries, currentDirection2, lastDirection);
                currentDirection = currentDirection2;
                //patterDirectionModify = true;
            }else if(canGo2 && canGo){
                // std::cout << "Two direction possible" << std::endl;
                // std::cout << "nextVertex: (" << nextVertex.x << ", " << nextVertex.y << ")" << std::endl
                //           << "nextVertex2: (" << nextVertex2.x << ", " << nextVertex2.y << ")" << std::endl;
                if((/*nextVertex2.x != 1 && nextVertex2.y != 1 && */
                std::find(vertices.begin(), vertices.end(), nextVertex2) == vertices.end()) && changeDir(currentVertex, m_map, directions[currentDirection], directions[currentDirection2])
                || (!changeDir(currentVertex, m_map, directions[currentDirection], directions[currentDirection2]) && std::find(vertices.begin(), vertices.end(), nextVertex) != vertices.end())){
                    // std::cout << "change dir to nextVertex2: (" << nextVertex2.x << ", " << nextVertex2.y << ")" << std::endl;
                    ICanGo(vertices, currentVertex, nextVertex2, nbTries, currentDirection2, lastDirection);
                    currentDirection = currentDirection2;
                    //patterDirectionModify = true;
                }else{
                    ICanGo(vertices, currentVertex, nextVertex, nbTries, currentDirection, lastDirection);
                }     
                // std::cout << "currentDirection: " << currentDirection << std::endl;         
            }else{
                ++nbTries;
            }
        currentDirection = (currentDirection + 1) % 4;
        // Skip the last successful direction
        if (currentDirection == lastDirection)
        {
            currentDirection = (currentDirection + 1) % 4;
        }
        
    } while (nbTries < 4);
    // std::cout << "nbTries: " << nbTries << std::endl;
    // std::cout << "setSortedVertices, vertices.size() = " << vertices.size() << std::endl;
}

/**
 * creation of a wall from a set of occupied cells
 * with sorted useful vertices
*/
void MapTemplate::createWall(std::vector<gf::Vector2f> occupiedCells){
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
                    if(std::find(usefulVertices.begin(), usefulVertices.end(), vertex) == usefulVertices.end()){
                        usefulVertices.push_back(vertex);
                    }
                }
            }
        }
    }

    // std::cout << "usefulVertices :" << std::endl;

    // for(auto v : usefulVertices){
    //     std::cout << "(" << v.x << ", " << v.y << ")" << std::endl;
    // }

    // std::cout << "Wall before step 2" << std::endl;

    int counter = 0;
    // step 2
    while (usefulVertices.size() > vertices.size())
    {
        // std::cout << "usefulVertices.size() = " << usefulVertices.size() << std::endl << "vertices.size() = " << vertices.size() << std::endl;
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

    // std::cout << "Wall after step 2 " << "usefulVertices.size() = " << usefulVertices.size() << std::endl << "vertices.size() = " << vertices.size() << std::endl;

    // std::cout << "Nombre de sommets: " << vertices.size() << std::endl;
    // // std::cout << "Nombre de title: " << wallCells.size() << std::endl;
    // std::cout << "Nombre de perimetre: " << perimetersSize.size() << std::endl;

    // std::cout << "les sommets pour un bloc de murs : " << std::endl;

    // for (const auto &vertice : vertices)
    // {

    //     std::cout << "(" << vertice.x << ", " << vertice.y << ")\n";
    // }

    // std::cout << "\n\n\n";
    m_walls.push_back(Wall(vertices, occupiedCells, perimetersSize));
}

void MapTemplate::depthFirstSearch(gf::Vector2f vertex, std::vector<gf::Vector2f> &visited, std::queue<gf::Vector2f> queue)
{
    // std::cout << "depthFirstSearch" << std::endl;
    queue.push(vertex);
    visited.push_back(vertex);
    while (!queue.empty())
    {
        gf::Vector2f current = queue.front();
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
                gf::Vector2f neighbor = gf::Vector2f(current.x + x, current.y + y);
                
                gf::Color4u pixelColor = m_image.getPixel({((int)neighbor.x), ((int)neighbor.y)});

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

bool MapTemplate::getSegments(gf::Vector2f point, std::vector<std::pair<gf::Vector2f, gf::Vector2f>> &segments)
{
    for (auto wall : m_walls)
    {
        if (wall.getSegments(point, segments))
        {
            // std::cout << "segments :" << std::endl;
            // for (auto segment : segments)
            // {
            //     std::cout << "(" << segment.first.x << ", " << segment.first.y << ") -> (" << segment.second.x << ", " << segment.second.y << ")" << std::endl;
            // }
            return true;
        }
    }
    // std::cout << std::endl << "point : (" << point.x << ", " << point.y << ")" << std::endl;
    return false;
}