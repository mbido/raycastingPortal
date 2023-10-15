#include <gf/Vector.h>
#include <gf/RenderWindow.h>
#include <gf/Shapes.h>

#include "Player.hpp"
#include "MapWalls.hpp"

struct castResult {
    gf::Vector2f endPoint;
    int tileX;
    int tileY;
    int tileSideHit; // 0 ->right, 1 -> bottom, 2 -> left, 3 -> top, -1 -> did not hit a wall
};
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
        
        struct castResult castRay(gf::Vector2f position, gf::Vector2f direction, MapWalls *m_walls);
};