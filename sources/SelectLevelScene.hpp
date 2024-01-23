#include <gf/Scene.h>
#include <gf/Action.h>
#include <gf/WidgetContainer.h>
#include <gf/Widgets.h>
#include <gf/Event.h>
#include <gf/Text.h>

namespace gh {

    struct GameHub;

    class SelectLevelScene : public gf::Scene {
    public:
        SelectLevelScene(GameHub& game);

    private:
        void doProcessEvent(gf::Event& event) override;
        void doHandleActions(gf::Window& window) override;
        void doRender(gf::RenderTarget& target, const gf::RenderStates &states) override;
        void doShow() override;
    
    private:
        GameHub& m_game;
        gf::Texture& m_backgroundTexture;

        gf::Action m_back2menuAction;

        gf::Action m_upAction;
        gf::Action m_downAction;
        gf::Action m_triggerAction;

        gf::TextButtonWidget m_level1;
        gf::TextButtonWidget m_back2menu;

        gf::WidgetContainer m_widgets;
    };

}