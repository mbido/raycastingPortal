#include "MapWalls.hpp"

#include <gf/RenderWindow.h>
#include <gf/Shapes.h>

#include <algorithm>

void MapWalls::render(gf::RenderWindow& renderer, int sizeUnit) {
    gf::Vector2u windowSize = renderer.getSize();
    /*for (int i = 0; i < m_nbColumns; ++i) {
        for (int j = 0; j < m_nbRows; ++j) {
            if (m_map[j][i] == 1) {
                gf::RectangleShape rectangle(gf::Vector2f((float)sizeUnit - 1.0f, (float)sizeUnit) - 1.0f);
                rectangle.setPosition(gf::Vector2f(j * (float)sizeUnit,i * (float)sizeUnit));
                rectangle.setColor(gf::Color::White);
                renderer.draw(rectangle);
            }
        }
    }*/

    gf::RectangleShape begin(gf::Vector2f((float)sizeUnit - 1.0f, (float)sizeUnit) - 1.0f);
    begin.setPosition(gf::Vector2f(m_begin.x * (float)sizeUnit, m_begin.y * (float)sizeUnit));
    begin.setColor(gf::Color::Transparent);
    begin.setOutlineColor(gf::Color::Green);
    begin.setOutlineThickness((float) sizeUnit / 100);
    renderer.draw(begin);
    
    gf::RectangleShape arrival(gf::Vector2f((float)sizeUnit - 1.0f, (float)sizeUnit) - 1.0f);
    arrival.setPosition(gf::Vector2f(m_arrival.x * (float)sizeUnit, m_arrival.y * (float)sizeUnit));
    arrival.setColor(gf::Color::Transparent);
    arrival.setOutlineColor(gf::Color::Red);
    arrival.setOutlineThickness((float) sizeUnit / 100);
    renderer.draw(arrival);
}

std::vector<gf::Vector2f> MapWalls::getVertices()
{
    std::vector<gf::Vector2f> vertices;
    for (auto wall : m_walls)
    {
        std::vector<gf::Vector2f> wallVertices = wall.getVertices();
        vertices.insert(vertices.end(), wallVertices.begin(), wallVertices.end());
    }
    return vertices;
}