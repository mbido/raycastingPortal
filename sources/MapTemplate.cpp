#include "MapTemplate.hpp"
#include <gf/Image.h>
#include <vector>
#include <map>
#include "Wall.hpp"


void MapTemplate::init() {
    // Initialize the map by setting all the values to 0 and the boundaries to 1

    std::map<int ,std::vector<gf::Vector2f>> cellsWalls;
    std::size_t nbWall = 0;

    for (int i = 0; i < m_nbRows; ++i) {
        for (int j = 0; j < m_nbColumns; ++j) {
            
            if (i == 0 || j == 0 || i == m_nbRows - 1 || j == m_nbColumns - 1) {
                m_map[i][j] = 1;
                cellsWalls[0].push_back(gf::Vector2f(i,j));
                if(nbWall == 0){
                    nbWall++;
                }

            }
            else {
                gf::Color4u pixelColor = m_image.getPixel({(i-1), (j-1)});
                bool newWall = false;
                if(static_cast<int>(pixelColor.r) == 0 && static_cast<int>(pixelColor.g) == 0 && static_cast<int>(pixelColor.b) == 0){
                    for(std::size_t i = 0; i < nbWall+1; ++i){
                        std::pair<int, int> cellToFindUp{i, j-1};
                        std::pair<int, int> cellToFindRight{i-1, j};
                        auto itUp = std::find(cellsWalls[i].begin(), cellsWalls[i].end(), cellToFindUp);
                        auto itRight = std::find(cellsWalls[i].begin(), cellsWalls[i].end(), cellToFindRight);
                        if(itUp != cellsWalls[i].end() || itRight != cellsWalls[i].end()){
                            cellsWalls[i].push_back(gf::Vector2f(i,j));
                            newWall = true;
                            break;
                        }
                    }
                    if(!newWall){
                        nbWall++;
                    }
                    m_map[i][j] = 1;
                }else{
                    m_map[i][j] = 0;
                }
            }
        }
    }

    for(auto cellWallsEntry : cellsWalls){
        m_walls.push_back(Wall(cellWallsEntry.second));
    }
}

int MapTemplate::setTile(int i, int j, int value) {
    if (i >= 0 && i < m_nbColumns && j >= 0 && j < m_nbColumns) {
        m_map[j][i] = value;
        return 0;
    } else {
        return -1;
    }
}
