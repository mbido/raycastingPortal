#ifndef LVL_END_SCENE
#define LVL_END_SCENE

#include <gf/Scene.h>
#include <gf/Action.h>
#include <gf/WidgetContainer.h>
#include <gf/Widgets.h>
#include <gf/Event.h>
#include <gf/Text.h>

namespace gh {

    struct GameHub;

    class LevelEndScene : public gf::Scene {
    public:
        LevelEndScene(GameHub& game);

    private:
        void doProcessEvent(gf::Event& event) override;
        void doHandleActions(gf::Window& window) override;
        void doRender(gf::RenderTarget& target, const gf::RenderStates &states) override;
        void doShow() override;
    
    private:
        GameHub& m_game;
        gf::Texture& m_backgroundTexture;

        gf::Action m_quitAction;

        gf::Action m_upAction;
        gf::Action m_downAction;
        gf::Action m_triggerAction;

        gf::TextButtonWidget m_menu;
        gf::TextButtonWidget m_quit;

        gf::WidgetContainer m_widgets;
    };
}

#endif // LVL_END_SCENE