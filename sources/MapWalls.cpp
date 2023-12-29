#include "MapWalls.hpp"

#include <gf/RenderWindow.h>
#include <gf/Shapes.h>

#include <algorithm>

void MapWalls::render(gf::RenderWindow& renderer, int sizeUnit) {
    gf::Vector2u windowSize = renderer.getSize();
    for (int i = 0; i < m_nbColumns; ++i) {
        for (int j = 0; j < m_nbRows; ++j) {
            if (m_map[j][i] == 1) {
                gf::RectangleShape rectangle(gf::Vector2f((float)sizeUnit - 1.0f, (float)sizeUnit) - 1.0f);
                rectangle.setPosition(gf::Vector2f(j * (float)sizeUnit,i * (float)sizeUnit));
                rectangle.setColor(gf::Color::White);
                renderer.draw(rectangle);
            }
        }
    }
}

std::vector<gf::Vector2i> MapWalls::getVertices()
{
    std::vector<gf::Vector2i> vertices;
    for (auto wall : m_walls)
    {
        std::vector<gf::Vector2i> wallVertices = wall.getVertices();
        vertices.insert(vertices.end(), wallVertices.begin(), wallVertices.end());
    }
    return vertices;
}