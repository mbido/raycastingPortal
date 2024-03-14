#ifndef LEVEL_SCENE_HPP
#define LEVEL_SCENE_HPP

#include <gf/Scene.h>
#include <gf/Action.h>
#include <gf/WidgetContainer.h>
#include <gf/Widgets.h>
#include <gf/Event.h>
#include <gf/Text.h>

#include "MapWalls.hpp"
#include "Player.hpp"
#include "Game3D.hpp"

namespace gh {

    struct GameHub;

    class LevelScene : public gf::Scene {
    public:
        LevelScene(GameHub& game);
        LevelScene(GameHub& game, int level);

    private:
        void doProcessEvent(gf::Event& event) override;
        void doHandleActions(gf::Window& window) override;
        void doRender(gf::RenderTarget& target, const gf::RenderStates &states) override;
        void doShow() override;
    
        GameHub& m_game;
        
        gf::Action m_back2menuAction;

        gf::Clock m_clock;

        MapWalls m_map;

        Player m_player;
        
        Game3D m_game3D;

        gf::Vector2f m_direction;
        float m_angularVelocity;

        gf::Action m_upAction;
        gf::Action m_downAction;
        gf::Action m_rightAction;
        gf::Action m_leftAction;
        gf::Action m_lookLeftAction;
        gf::Action m_lookRightAction;

    };

}

#endif // LEVEL_SCENE_HPP