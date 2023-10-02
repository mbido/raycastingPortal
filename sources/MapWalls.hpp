#ifndef MAPWALLS_HPP
#define MAPWALLS_HPP

#include "MapTemplate.hpp"

#include <gf/RenderWindow.h>

class MapWalls : public MapTemplate {
    public :
        MapWalls(int nbRows = 10, int nbColumns = 10)
        : MapTemplate(nbRows, nbColumns)
        {
        }
        void render(gf::RenderWindow& renderer, int sizeUnit);
};

#endif