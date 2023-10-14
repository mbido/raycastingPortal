#include "MapWalls.hpp"
#include "Player.hpp"

class Renderer{
    public:
        Renderer(MapWalls walls, Player player)
        :m_walls(walls)
        ,m_player(player)
        {
        }
        void render(gf::RenderWindow& renderer, int sizeUnit);
    private:
        MapWalls m_walls;
        Player m_player;
};