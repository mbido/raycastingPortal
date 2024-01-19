#ifndef MAPTEMPLATE_HPP
#define MAPTEMPLATE_HPP

#include <iostream>
#include <gf/Image.h>
#include <vector>
#include <set>
#include <queue>
#include <unordered_set>
#include "Wall.hpp"

class MapTemplate {
    public :
        MapTemplate(std::string adress = "../sources/map/image5.png")
        :m_image(gf::Image(adress))
        ,m_nbRows(m_image.getSize().width)
        ,m_nbColumns(m_image.getSize().height)
        ,m_map(new int*[m_nbRows])
        ,m_walls({})
        {
            for (int i = 0; i < m_nbRows; ++i) {
                m_map[i] = new int[m_nbColumns];
            }
            init();
        }
        int setTile(int i, int j, int value);
        
        ~MapTemplate() {
            for (int i = 0; i < m_nbRows; ++i) {
                delete[] m_map[i];
            }
            delete[] m_map;
        }

        int getNbRows() const {
            return m_nbRows;
        }

        int getNbColumns() const {
            return m_nbColumns;
        }

        int getTile(int i, int j) const {
        if(i >= 0 && i < m_nbColumns && j >= 0 && j < m_nbRows)
            return m_map[i][j];
        return -1;
        }

        std::vector<Wall> getWalls() const {
            return m_walls;
        }

        

        void depthFirstSearch(gf::Vector2i vertex, std::vector<gf::Vector2i>& visited, std::queue<gf::Vector2i> queue);

        bool getSegments(gf::Vector2f point, std::vector<std::pair<gf::Vector2i, gf::Vector2i>> &segments);

    protected :
        gf::Image m_image;
        int m_nbRows;
        int m_nbColumns;
        int **m_map;
        std::vector<Wall> m_walls;
    
    private :
        /**
         * @brief Initialize the map by setting all the values to 0 and the boundaries to 1
        */
        void init();

        void createWall(std::vector<gf::Vector2i> occupiedCells);
        void setSortedVertices(const gf::Vector2i from, std::vector<gf::Vector2i> usefulVertices, std::vector<gf::Vector2i>& sortedVertices);
};

#endif
