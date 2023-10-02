#include "MapWalls.hpp"

#include <gf/RenderWindow.h>
#include <gf/Shapes.h>

#include <algorithm>

void MapWalls::render(gf::RenderWindow& renderer, int sizeUnit) {
    gf::Vector2u windowSize = renderer.getSize();
    for (int i = 0; i < m_nbRows; ++i) {
        for (int j = 0; j < m_nbColumns; ++j) {
            if (m_map[i][j] == 1) {
                gf::RectangleShape rectangle(gf::Vector2f((float)sizeUnit - 1.0f, (float)sizeUnit) - 1.0f);
                rectangle.setPosition(gf::Vector2f(i * (float)sizeUnit,j * (float)sizeUnit));
                rectangle.setColor(gf::Color::Black);
                renderer.draw(rectangle);
            }
        }
    }
}