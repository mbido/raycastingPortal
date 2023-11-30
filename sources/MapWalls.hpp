#ifndef MAPWALLS_HPP
#define MAPWALLS_HPP

#include "MapTemplate.hpp"

#include <gf/RenderWindow.h>
#include <gf/Image.h>

class MapWalls : public MapTemplate {
    public :
        MapWalls(std::string address = "image.png"/*int nbRows = 10, int nbColumns = 10*/)
        : MapTemplate(address)
        {
        }
        void render(gf::RenderWindow& renderer, int sizeUnit);
    private :
        gf::Image m_image;
};

#endif
