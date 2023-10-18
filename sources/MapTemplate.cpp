#include "MapTemplate.hpp"
#include <gf/Image.h>


void MapTemplate::init() {
    // Initialize the map by setting all the values to 0 and the boundaries to 1
    for (int i = 0; i < m_nbRows; ++i) {
        for (int j = 0; j < m_nbColumns; ++j) {
            
            if (i == 0 || j == 0 || i == m_nbRows - 1 || j == m_nbColumns - 1) {
                m_map[i][j] = 1;
            }
            else {
                gf::Color4u pixelColor = m_image.getPixel({static_cast<unsigned>(i-1), static_cast<unsigned>(j-1)});
                if(static_cast<int>(pixelColor.r) == 0 && static_cast<int>(pixelColor.g) == 0 && static_cast<int>(pixelColor.b) == 0){
                    m_map[i][j] = 1;
                }else{
                    m_map[i][j] = 0;
                }
            }
        }
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
