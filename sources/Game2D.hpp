#ifndef Game2D_HPP
#define Game2D_HPP

#include <gf/Vector.h>
#include <gf/RenderTarget.h>
#include <gf/Shapes.h>

#include "Player.hpp"
#include "MapWalls.hpp"
#include "Wall.hpp"


class Game2D {
    public:
        Game2D(Player *player, MapWalls *walls, gf::RenderTarget& renderer)
        : m_player(player)
        , m_walls(walls)
        , m_renderer(renderer)
        , m_firstPortal(NULL)
        , m_secondPortal(NULL)
        {
            m_windowSize = m_renderer.getSize();
            m_scaleUnit = std::min((int)(m_windowSize[0] / m_walls->getNbRows()), (int)(m_windowSize[1] / m_walls->getNbColumns()));
        }
        void castPortal(bool isFirstPortal);
        void update(gf::Time dt);
        void render(bool isPortal = false, std::pair<gf::Vector2f, gf::Vector2f> portalSegment = std::make_pair(gf::Vector2f(0, 0), gf::Vector2f(0, 0)));
    private:
        struct portal *m_firstPortal;
        struct portal *m_secondPortal;
        Player *m_player;
        MapWalls *m_walls;
        gf::RenderTarget& m_renderer;
        int m_scaleUnit;
        gf::Vector2u m_windowSize;
        bool getVisibleSegment(gf::Vector2f &start, gf::Vector2f &end, gf::Vector2f playerPosition, double playerAngle, double a, double b);
        bool isPartIncluded(std::vector<gf::Vector2f> subSegments);
};
#endif // Game2D_HPP