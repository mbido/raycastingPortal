#include <iostream>

#include <gf/Vector.h>
#include <gf/RenderWindow.h>
#include <gf/Shapes.h>

#include "Player.hpp"
#include "MapWalls.hpp"
#include "Game2D.hpp"


void Game2D::update(gf::Time dt) {
    m_player->update(dt);

    m_windowSize = m_renderer.getSize();
    m_scaleUnit = std::min((int)(m_windowSize[0] / m_walls->getNbRows()), (int)(m_windowSize[1] / m_walls->getNbColumns()));
}

gf::Vector2f castRay(gf::Vector2f position, double angle) {
    return gf::vec(0.0f, 0.0f);
}

void Game2D::render() {
    // render the map :
    m_walls->render(m_renderer, m_scaleUnit);

    // render the player :
    m_player->render(m_renderer, m_scaleUnit);
}