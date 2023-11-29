#include <iostream>
#include <algorithm>
#include <vector>
#include <set>


#include "Wall.hpp"

Wall::Wall(std::vector<gf::Vector2i> occupiedCells) : wallCells(occupiedCells)
{

    // step 1: sorting the occupied cells by x and y (cf : notes)
    // step 2: creating a list of the useful vertices (cf : notes)
    // step 3: creating the wall's sides by flowing the next algorithm :
    //          a)-we start at the top left vertex of the first cell
    //          b)-we follow the next direction by priority : Right > Down > Left > Up
    //          c)-we repeat the step b) until we reach the top left vertex of the first cell again

    int currentX = occupiedCells.front().x;
    int currentY = occupiedCells.front().y;

    vertices.push_back(gf::Vector2i(currentX, currentY));

    std::size_t size = occupiedCells.size();
    char dir = 'r';
    currentX++;

    for(std::size_t i = 1; i < size; ++i){
        gf::Vector2i currentVertex = {currentX, currentY};
        gf::Vector2i currentVertexUp = {currentX, currentY - 1};
        gf::Vector2i currentVertexUpLeft = {currentX - 1, currentY - 1};
        gf::Vector2i currentVertexLeft = {currentX - 1, currentY};

        int j = 0;

        if(std::find(occupiedCells.begin(), occupiedCells.end(), currentVertex) != occupiedCells.end()){
            ++j;
        }
        if(std::find(occupiedCells.begin(), occupiedCells.end(), currentVertexUp) != occupiedCells.end()){
            ++j;
        }
        if(std::find(occupiedCells.begin(), occupiedCells.end(), currentVertexUpLeft) != occupiedCells.end()){
            ++j;
        }
        if(std::find(occupiedCells.begin(), occupiedCells.end(), currentVertexLeft) != occupiedCells.end()){
            ++j;
        }

        if(j == 1 || j == 3){
            vertices.push_back(currentVertex);
            // if(dir == 'r'){
            //     if(j == 1){
            //         dir = 'd';
            //     }else if(j == 3){
            //         dir = 'u';
            //     }
            // }else if(dir == 'd'){
            //     if(j == 1){
            //         dir = 'l';
            //     }else if(j == 3){
            //         dir = 'r';
            //     }
            // }else if(dir == 'l'){
            //     if(j == 1){
            //         dir = 'd';
            //     }else if(j == 3){
            //         dir = 'u';
            //     }
            // }else if(dir == 'u'){
            //     if(j == 1){
            //         dir = 'r';
            //     }else if(j == 3){
            //         dir = 'l';   
            //     }
            // }
        }

        // if(dir == 'u'){
        //     currentY--;
        // }else if(dir == 'd'){
        //     currentY++;
        // }else if(dir == 'r'){
        //     currentX++;
        // }else if(dir == 'l'){
        //     currentX--;
        // }
    }

    std::cout << "les sommets pour un bloc de murs : " << std::endl;

    for(const auto& vertice : vertices){

        std::cout << "(" << vertice.x << ", " << vertice.y << ")\n";

    }

    std::cout << "\n\n\n";

}

void Wall::render(gf::RenderWindow& window, int scale)
{
    std::size_t size = vertices.size();
    for(std::size_t i = 0; i < size; ++i){
        gf::Vector2i currentVertex = vertices[i];
        gf::Vector2i nextVertex = vertices[(i+1)%size];
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