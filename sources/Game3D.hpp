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

struct portal {
    gf::Vector2f position;
    int facing; // 0 ->right, 1 -> bottom, 2 -> left, 3 -> top
    struct portal *linkedPortal = NULL;
    double width = 0.3f;
};

class Game3D {
    public:
        struct portal *m_firstPortal;
        struct portal *m_secondPortal;
        Player *m_player;
        MapWalls *m_walls;
        gf::RenderWindow& m_renderer;
        int m_scaleUnit;
        gf::Vector2u m_windowSize;
        gf::Texture m_texture;

        Game3D(Player *player, MapWalls *walls, gf::RenderWindow& renderer)
        : m_player(player)
        , m_walls(walls)
        , m_renderer(renderer)
        , m_firstPortal(NULL)
        , m_secondPortal(NULL)
        , m_texture(gf::Texture(std::filesystem::path("../sources/resources/bricks-01.png")))
        {
            m_texture.setRepeated(true);
            m_windowSize = m_renderer.getSize();
            m_scaleUnit = std::min((int)(m_windowSize[0] / m_walls->getNbRows()), (int)(m_windowSize[1] / m_walls->getNbColumns()));
        }
        void castPortal(bool isFirstPortal);
        void update(gf::Time dt);
        void render();
        
    private:
};