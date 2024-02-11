#ifndef MAPWALLS_HPP
#define MAPWALLS_HPP

#include "MapTemplate.hpp"

#include <gf/RenderWindow.h>
#include <gf/Image.h>

class MapWalls : public MapTemplate {
    public :
        MapWalls(std::string address = "../sources/map/image4.png")
        : MapTemplate(address)
        {
        }
        void render(gf::RenderWindow& renderer, int sizeUnit);
        std::vector<gf::Vector2f> getVertices();

    private :
        gf::Image m_image;
};

#endif
