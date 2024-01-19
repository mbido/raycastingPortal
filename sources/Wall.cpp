#include <iostream>
#include <algorithm>
#include <vector>
#include <set>

#include "Wall.hpp"

#define EPSILON 0.001


// bool fctCanGo(gf::Vector2i from, char direction, const std::vector<gf::Vector2i> &usefulVertices, gf::Vector2i &where)
// {
//     int deltaX = 0;
//     int deltaY = 0;
//     bool isHorizontal; // true -> horizontal, false -> vertical

//     switch (direction)
//     {
//     case 'r':
//         deltaX = 1;
//         isHorizontal = true;
//         break;
//     case 'd':
//         deltaY = 1;
//         isHorizontal = false;
//         break;
//     case 'l':
//         deltaX = -1;
//         isHorizontal = true;
//         break;
//     case 'u':
//         deltaY = -1;
//         isHorizontal = false;
//         break;
//     default:
//         // Handle invalid direction
//         return false;
//     }

//     where = {-1, -1};

//     for (const auto &vx : usefulVertices)
//     {
//         // std::cout << "vx: (" << vx.x << ", " << vx.y << ")" << std::endl;
//         if (vx == from)
//         {
//             continue;
//         }

//         int axisIndex = isHorizontal ? 0 : 1;
//         if (vx[axisIndex] != from[axisIndex])
//         {
//             continue;
//         }

//         if (((deltaX < 0 || deltaY < 0) && vx[1 - axisIndex] > from[1 - axisIndex]) ||
//             ((deltaX > 0 || deltaY > 0) && vx[1 - axisIndex] < from[1 - axisIndex]))
//         {
//             continue;
//         }

//         if ((where.x == -1 && where.y == -1) ||
//             ((deltaX < 0 || deltaY < 0) && vx[1 - axisIndex] > where[1 - axisIndex]) ||
//             ((deltaX > 0 || deltaY > 0) && vx[1 - axisIndex] < where[1 - axisIndex]))
//         {
//             where = vx;
//             //break;
//         }
//     }
//     std::cout << "from (" << from.x << ", " << from.y << ")" << std::endl
//               << "direction: " << direction << std::endl
//               << "where (" << where.x << ", " << where.y << ")" << std::endl;
//     std::cout << "canGo?: " << (where.x != -1 || where.y != -1) << std::endl;
//     return where.x != -1 || where.y != -1;
// }

// void ICanGo(std::vector<gf::Vector2i>& vertices, gf::Vector2i& currentVertex, gf::Vector2i& nextVertex, int& nbTries, int& currentDirection, int& lastDirection){
//     currentVertex = nextVertex;
//     vertices.push_back(currentVertex);
//     lastDirection = currentDirection;
//     nbTries = 0;
// }

// bool changeDir(gf::Vector2i nextVertex, gf::Vector2i nextVertex2, char dir){
//     std::cout << dir << std::endl;
//     switch (dir)
//     {
//     case 'd':
//         return (nextVertex.y == nextVertex2.y && nextVertex.x > nextVertex2.x);
//     case 'l':
//         return (nextVertex.x == nextVertex2.x && nextVertex.y > nextVertex2.y);
//     case 'u':
//         return (nextVertex.y == nextVertex2.y && nextVertex.x < nextVertex2.x);
//     case 'r':
//         return (nextVertex.x == nextVertex2.x && nextVertex.y < nextVertex2.y);
//     default:
//         return false;
//     }
// }

// void Wall::setSortedVertices(gf::Vector2i from, std::vector<gf::Vector2i> usefulVertices)
// {
//     gf::Vector2i currentVertex = from;
//     gf::Vector2i nextVertex = {0, 0};
//     gf::Vector2i nextVertex2 = {0, 0};
//     // gf::Vector2i lastVertex = {0, 0};
//     std::vector<char> directions = {'r', 'd', 'l', 'u'};
//     int lastDirection = -1;
//     int currentDirection = 0;
//     int nbTries = 0;
//     std::cout << "FROM: (" << from.x << ", " << from.y << ") usefulVertices.size() = " << usefulVertices.size() << std::endl;
//     do
//     {
//         bool canGo = fctCanGo(currentVertex, directions[currentDirection], usefulVertices, nextVertex);
//         bool canGo2 = fctCanGo(currentVertex, directions[(currentDirection + 2) % 4], usefulVertices, nextVertex2);
//         int currentDirection2 = (currentDirection + 2) % 4;
//         bool br = (canGo && std::find(vertices.begin(), vertices.end(), nextVertex) != vertices.end());
//         bool br2 = (canGo2 && std::find(vertices.begin(), vertices.end(), nextVertex2) != vertices.end());
//         // std::cout << "br = " << br << std::endl << "br2 = " << br2 << std::endl;
//         if(br || br2){
//             // std::cout << "break" << std::endl;
//             // break;
//             if((vertices.size() == 4 && from.x == 0 && from.y == 0) || (vertices.size() == usefulVertices.size() && from.x == 1 && from.y == 1)){
//                 std::cout << "break" << std::endl;
//                 std::cout << "vertices.size() = " << vertices.size() << std::endl;
//                 std::cout << "EXTERIEUR" << std::endl;
//                 break;
//             }

//             if((vertices.size() == usefulVertices.size() && from.x != 0 && from.y != 0)){
//                 std::cout << "break" << std::endl;
//                 std::cout << "NON EXTERIEUR" << std::endl;
//                 // vertices.push_back(vertices.at(0));
//                 break;
//             }
//         }
//         // if((br && !br2 && canGo2) || (br2 && !br && canGo)){
//             if(canGo && !canGo2){
//                 ICanGo(vertices, currentVertex, nextVertex, nbTries, currentDirection, lastDirection);
//             }else if(!canGo && canGo2){
//                 ICanGo(vertices, currentVertex, nextVertex2, nbTries, currentDirection2, lastDirection);
//                 currentDirection = currentDirection2;
//                 //patterDirectionModify = true;
//             }else if(canGo2 && canGo){
//                 std::cout << "Two direction possible" << std::endl;
//                 std::cout << "nextVertex: (" << nextVertex.x << ", " << nextVertex.y << ")" << std::endl
//                           << "nextVertex2: (" << nextVertex2.x << ", " << nextVertex2.y << ")" << std::endl;
//                 if((nextVertex2.x != 1 && nextVertex2.y != 1 && std::find(vertices.begin(), vertices.end(), nextVertex2) == vertices.end()) && changeDir(nextVertex, nextVertex2, directions[currentDirection2])){
//                     // std::cout << "change dir to nextVertex2: (" << nextVertex2.x << ", " << nextVertex2.y << ")" << std::endl;
//                     ICanGo(vertices, currentVertex, nextVertex2, nbTries, currentDirection2, lastDirection);
//                     currentDirection = currentDirection2;
//                     //patterDirectionModify = true;
//                 }else{
//                     ICanGo(vertices, currentVertex, nextVertex, nbTries, currentDirection, lastDirection);
//                 }     
//                 // std::cout << "currentDirection: " << currentDirection << std::endl;         
//             }else{
//                 ++nbTries;
//             }
//         // }
//         // if (canGo)
//         // {
//         //     if (std::find(vertices.begin(), vertices.end(), nextVertex) != vertices.end())
//         //     {
//         //         std::cout << "break" << std::endl;
//         //         br = true;
//         //         //break;
//         //     }
//         //     if(!br){
//         //         currentVertex = nextVertex;
//         //         vertices.push_back(currentVertex);
//         //         lastDirection = currentDirection;
//         //         nbTries = 0;
//         //     }
            
//         // }else{
//         //     ++nbTries;
//         // }
//         currentDirection = (currentDirection + 1) % 4;
//         // Skip the last successful direction
//         if (currentDirection == lastDirection)
//         {
//             currentDirection = (currentDirection + 1) % 4;
//         }
        
//     } while (nbTries < 4);
//     // std::cout << "nbTries: " << nbTries << std::endl;
//     // std::cout << "setSortedVertices, vertices.size() = " << vertices.size() << std::endl;
// }

Wall::Wall(std::vector<gf::Vector2i> vertices, std::vector<gf::Vector2i> occupiedCells, std::vector<std::size_t> perimetersSize) : wallCells(occupiedCells), vertices(vertices), perimetersSize(perimetersSize)
{
    std::cout << "Wall creation vertices: " << std::endl;
    for(const auto& vertice : vertices){
        std::cout << "(" << vertice.x << ", " << vertice.y << ")" << std::endl;
    }
}

// Wall::Wall(std::vector<gf::Vector2i> occupiedCells) : wallCells(occupiedCells)
// {

//     // step 1: creating a list of the useful vertices (cf : notes)
//     // step 2: creating the wall's sides by flowing the next algorithm :
//     //          a)-we start at the top left vertex of the first cell
//     //          b)-we follow the next direction by priority : Right > Down > Left > Up
//     //          c)-we repeat the step b) until we reach the top left vertex of the first cell again

//     // step 1
//     perimetersSize = {};
//     std::vector<gf::Vector2i> usefulVertices;

//     for (auto cell : occupiedCells)
//     {

//         // a vertex is useful if he is in a corner => and so if the number of occupied cells around him is 1 or 3
//         for (int x = 0; x < 2; ++x)
//         {
//             for (int y = 0; y < 2; ++y)
//             {
//                 gf::Vector2i vertex = gf::Vector2i(cell.x + x, cell.y + y);

//                 gf::Vector2i cellUpLeft = gf::Vector2i(vertex.x - 1, vertex.y - 1);
//                 gf::Vector2i cellUpRight = gf::Vector2i(vertex.x, vertex.y - 1);
//                 gf::Vector2i cellDownLeft = gf::Vector2i(vertex.x - 1, vertex.y);
//                 gf::Vector2i cellDownRight = gf::Vector2i(vertex.x, vertex.y);

//                 int count = 0;
//                 if (std::find(occupiedCells.begin(), occupiedCells.end(), cellUpLeft) != occupiedCells.end())
//                 {
//                     ++count;
//                 }
//                 if (std::find(occupiedCells.begin(), occupiedCells.end(), cellUpRight) != occupiedCells.end())
//                 {
//                     ++count;
//                 }
//                 if (std::find(occupiedCells.begin(), occupiedCells.end(), cellDownLeft) != occupiedCells.end())
//                 {
//                     ++count;
//                 }
//                 if (std::find(occupiedCells.begin(), occupiedCells.end(), cellDownRight) != occupiedCells.end())
//                 {
//                     ++count;
//                 }

//                 if (count == 1 || count == 3)
//                 {
//                     if(std::find(usefulVertices.begin(), usefulVertices.end(), vertex) == usefulVertices.end()){
//                         usefulVertices.push_back(vertex);
//                     }
//                 }
//             }
//         }
//     }

//     std::cout << "usefulVertices :" << std::endl;

//     for(auto v : usefulVertices){
//         std::cout << "(" << v.x << ", " << v.y << ")" << std::endl;
//     }

//     std::cout << "Wall before step 2" << std::endl;

//     int counter = 0;
//     // step 2
//     while (usefulVertices.size() > vertices.size())
//     {
//         std::cout << "usefulVertices.size() = " << usefulVertices.size() << std::endl << "vertices.size() = " << vertices.size() << std::endl;
//         gf::Vector2i vertex;
//         bool found = false;
//         for (auto &vx : usefulVertices)
//         {
//             if (std::find(vertices.begin(), vertices.end(), vx) == vertices.end())
//             {
//                 vertex = vx;
//                 found = true;
//                 break;
//             }
//         }

//         if (found)
//         {
//             std::size_t prevSize = vertices.size();
//             setSortedVertices(vertex, usefulVertices);
//             perimetersSize.push_back(vertices.size() - prevSize);
//         }
//         else
//         {
//             break;
//         }
//     }

//     std::cout << "Wall after step 2 " << "usefulVertices.size() = " << usefulVertices.size() << std::endl << "vertices.size() = " << vertices.size() << std::endl;

//     std::cout << "Nombre de sommets: " << vertices.size() << std::endl;
//     std::cout << "Nombre de title: " << wallCells.size() << std::endl;
//     std::cout << "Nombre de perimetre: " << perimetersSize.size() << std::endl;

//     std::cout << "les sommets pour un bloc de murs : " << std::endl;

//     for (const auto &vertice : vertices)
//     {

//         std::cout << "(" << vertice.x << ", " << vertice.y << ")\n";
//     }

//     std::cout << "\n\n\n";
// }

void Wall::render(gf::RenderWindow &window, int scale)
{
    gf::Vector2i origin = {0, 0};
    if(std::find(vertices.begin(), vertices.end(), origin) != vertices.end()){
        for(std::size_t i = 0; i < 4; ++i){
            gf::Vector2i currentVertex = vertices[i];
            gf::Vector2i prevVertex = vertices[(i+1)%4];
            gf::Line line(currentVertex * scale, prevVertex * scale);
            line.setColor(gf::Color::White);
            window.draw(line);
        }
        for(std::size_t i = 4; i < vertices.size(); ++i){
            gf::Vector2i currentVertex = vertices[i];
            gf::Vector2i prevVertex = ((i+1)%vertices.size() == 0) ? vertices[4] : vertices[(i+1)];
            gf::Line line(currentVertex * scale, prevVertex * scale);
            line.setColor(gf::Color::White);
            window.draw(line);
        }
    }else{
        for(std::size_t i = 0; i < vertices.size(); ++i){
            gf::Vector2i currentVertex = vertices[i];
            gf::Vector2i prevVertex = vertices[(i+1)%vertices.size()];
            gf::Line line(currentVertex * scale, prevVertex * scale);
            line.setColor(gf::Color::White);
            window.draw(line);
        }
    }
    // std::size_t size = vertices.size();
    // int endAt = size - 1;
    // for (size_t i = perimetersSize.size() - 1; i < perimetersSize.size(); --i)
    // {
    //     size_t s = perimetersSize[i];
    //     for (size_t j = 0; j < s; ++j)
    //     {
    //         gf::Vector2i currentVertex = vertices[endAt - j];
    //         gf::Vector2i prevVertex = vertices[(endAt - j - 1) % s + endAt - s + 1];
    //         gf::Line line(currentVertex * scale, prevVertex * scale);
    //         line.setColor(gf::Color::White);
    //         window.draw(line);
    //     }
    //     endAt -= s;
    // }
}

std::vector<gf::Vector2i> Wall::getVertices()
{
    return vertices;
}

// std::vector<gf::Vector2i> Wall::getSortedVertices(gf::Vector2f playerPositions)
// {
//     std::vector<gf::Vector2i> sortedVertices = vertices;
//     std::sort(sortedVertices.begin(), sortedVertices.end(), CompareVerticesAngle(playerPositions));
//     return sortedVertices;
// }

bool Wall::getSegments(gf::Vector2f point, std::vector<std::pair<gf::Vector2i, gf::Vector2i>> &segments)
{
    segments.clear();
    std::size_t size = vertices.size();
    int endAt = size - 1;
    for (int i = static_cast<int>(perimetersSize.size()) - 1; i >= 0; --i)
    {   
        size_t s = perimetersSize[i];
        for (size_t j = 0; j < s; ++j)
        {
            gf::Vector2i currentVertex = vertices[endAt - j];
            gf::Vector2i prevVertex = vertices[(endAt - j - 1) % s + endAt - s + 1];
            bool isVertical = std::abs(point.x - currentVertex.x) < EPSILON && std::abs(point.x - prevVertex.x) < EPSILON;
            bool isHorizontal = std::abs(point.y - currentVertex.y) < EPSILON && std::abs(point.y - prevVertex.y) < EPSILON;
            // std::cout << "currentVertex: (" << currentVertex.x << ", " << currentVertex.y << ")" << std::endl
            //             << "prevVertex: (" << prevVertex.x << ", " << prevVertex.y << ")" << std::endl
            //             << "isVertical: " << isVertical << std::endl
            //             << "isHorizontal: " << isHorizontal << std::endl;
            if (isVertical && ((point.y <= currentVertex.y && point.y >= prevVertex.y) || (point.y >= currentVertex.y && point.y <= prevVertex.y)))
            {
                segments.push_back({currentVertex, prevVertex});
            }
            else if (isHorizontal && ((point.x <= currentVertex.x && point.x >= prevVertex.x) || (point.x >= currentVertex.x && point.x <= prevVertex.x)))
            {
                segments.push_back({currentVertex, prevVertex});
            }
        }
        endAt -= s;
    }
    return !segments.empty();
}
