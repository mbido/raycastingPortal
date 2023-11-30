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
            setSortedVertices(vertex, usefulVertices);
        }
        else
        {
            break;
        }
    }

    // int counter = 0;
    // // step 2
    // while(usefulVertices.size() > vertices.size())
    // {
    //     if(counter++ > 10000)
    //     {
    //         std::cout << "counter = " << counter << std::endl;
    //         std::cout << "usefulVertices.size() = " << usefulVertices.size() << std::endl;
    //         std::cout << "vertices.size() = " << usefulVertices.size() << std::endl;
    //         break;
    //     }
    //     gf::Vector2i vertex;
    //     for(auto &vx : usefulVertices)
    //     {
    //         if(std::find(vertices.begin(), vertices.end(), vx) == vertices.end())
    //         {
    //             vertex = vx;
    //             break;
    //         }
    //     }
    //     setSortedVertices(vertex, usefulVertices);
    // }

    // std::cout << "current x : " << currentVertex.x << std::endl;
    // std::cout << "current y : " << currentVertex.y << std::endl;

    // gf::Vector2i newVertex;
    // std::cout << "can go right ? : " << canGo(currentVertex, 'r', usefulVertices, newVertex) << std::endl;
    // std::cout << "new x : " << newVertex.x << std::endl;
    // std::cout << "new y : " << newVertex.y << std::endl;

    // int currentX = occupiedCells.front().x;
    // int currentY = occupiedCells.front().y;

    // vertices.push_back(gf::Vector2i(currentX, currentY));

    // std::size_t size = occupiedCells.size();
    // char dir = 'r';
    // currentX++;

    // for(std::size_t i = 1; i < size; ++i){
    //     gf::Vector2i currentVertex = {currentX, currentY};
    //     gf::Vector2i currentVertexUp = {currentX, currentY - 1};
    //     gf::Vector2i currentVertexUpLeft = {currentX - 1, currentY - 1};
    //     gf::Vector2i currentVertexLeft = {currentX - 1, currentY};

    //     int j = 0;

    //     if(std::find(occupiedCells.begin(), occupiedCells.end(), currentVertex) != occupiedCells.end()){
    //         ++j;
    //     }
    //     if(std::find(occupiedCells.begin(), occupiedCells.end(), currentVertexUp) != occupiedCells.end()){
    //         ++j;
    //     }
    //     if(std::find(occupiedCells.begin(), occupiedCells.end(), currentVertexUpLeft) != occupiedCells.end()){
    //         ++j;
    //     }
    //     if(std::find(occupiedCells.begin(), occupiedCells.end(), currentVertexLeft) != occupiedCells.end()){
    //         ++j;
    //     }

    //     if(j == 1 || j == 3){
    //         vertices.push_back(currentVertex);
    //         // if(dir == 'r'){
    //         //     if(j == 1){
    //         //         dir = 'd';
    //         //     }else if(j == 3){
    //         //         dir = 'u';
    //         //     }
    //         // }else if(dir == 'd'){
    //         //     if(j == 1){
    //         //         dir = 'l';
    //         //     }else if(j == 3){
    //         //         dir = 'r';
    //         //     }
    //         // }else if(dir == 'l'){
    //         //     if(j == 1){
    //         //         dir = 'd';
    //         //     }else if(j == 3){
    //         //         dir = 'u';
    //         //     }
    //         // }else if(dir == 'u'){
    //         //     if(j == 1){
    //         //         dir = 'r';
    //         //     }else if(j == 3){
    //         //         dir = 'l';
    //         //     }
    //         // }
    //     }

    //     // if(dir == 'u'){
    //     //     currentY--;
    //     // }else if(dir == 'd'){
    //     //     currentY++;
    //     // }else if(dir == 'r'){
    //     //     currentX++;
    //     // }else if(dir == 'l'){
    //     //     currentX--;
    //     // }
    // }

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
    for (std::size_t i = 0; i < size; ++i)
    {
        gf::Vector2i currentVertex = vertices[i];
        gf::Vector2i nextVertex = vertices[(i + 1) % size];
        gf::Line line(currentVertex * scale, nextVertex * scale);
        line.setColor(gf::Color::White);
        window.draw(line);
    }
}

std::vector<gf::Vector2i> Wall::getSortedVertices(gf::Vector2f playerPositions)
{
    std::vector<gf::Vector2i> sortedVertices = vertices;
    std::sort(sortedVertices.begin(), sortedVertices.end(), CompareVerticesAngle(playerPositions));
    return sortedVertices;
}