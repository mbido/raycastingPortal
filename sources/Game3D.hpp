#include <gf/Vector.h>
#include <gf/RenderWindow.h>
#include <gf/Shapes.h>

#include "Player.hpp"
#include "MapWalls.hpp"


class Game3D {
    public:
        Game3D(Player *player, MapWalls *walls, gf::RenderWindow& renderer)
        : m_player(player)
        , m_walls(walls)
        , m_renderer(renderer)
        {
            m_windowSize = m_renderer.getSize();
            m_scaleUnit = std::min((int)(m_windowSize[0] / m_walls->getNbRows()), (int)(m_windowSize[1] / m_walls->getNbColumns()));
        }

        void update(gf::Time dt);
        void render();
    private:
        Player *m_player;
        MapWalls *m_walls;
        gf::RenderWindow& m_renderer;
        int m_scaleUnit;
        gf::Vector2u m_windowSize;
};