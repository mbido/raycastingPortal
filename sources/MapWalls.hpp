#ifndef MAPWALLS_HPP
#define MAPWALLS_HPP

#include "MapTemplate.hpp"

#include <gf/RenderWindow.h>
#include <gf/Image.h>

class MapWalls : public MapTemplate {
    public :
        MapWalls(std::string adress = "image.png"/*int nbRows = 10, int nbColumns = 10*/)
        : MapTemplate(adress)
        {
        }
        void render(gf::RenderWindow& renderer, int sizeUnit);
};

#endif
