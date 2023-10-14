#ifndef MAPTEMPLATE_HPP
#define MAPTEMPLATE_HPP

#include <iostream>

class MapTemplate {
    public :
        MapTemplate(int nbRows = 10, int nbColumns = 10)
        :m_nbRows(nbRows)
        ,m_nbColumns(nbColumns)
        ,m_map(new int*[m_nbRows])
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

    protected :
        int m_nbRows;
        int m_nbColumns;
        int **m_map;
    
    private :
        /**
         * @brief Initialize the map by setting all the values to 0 and the boundaries to 1
        */
        void init();
};

#endif