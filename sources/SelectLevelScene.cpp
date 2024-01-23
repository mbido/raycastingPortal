#include <gf/Coordinates.h>
#include <gf/Sprite.h>

#include "GameHub.hpp"


namespace gh {

    SelectLevelScene::SelectLevelScene(GameHub& game)
    : gf::Scene(game.getRenderer().getSize())
    , m_game(game)
    , m_backgroundTexture(game.resources.getTexture("bgTexture.png"))
    , m_upAction("UpAction")
    , m_downAction("DownAction")
    , m_triggerAction("TriggerAction")
    , m_back2menuAction("Menu")
    , m_level1("Level 1", game.resources.getFont("Underdog.otf"))
    , m_back2menu("Menu", game.resources.getFont("Underdog.otf"))
    {
        setClearColor(gf::Color::Black);

        m_back2menuAction.addKeycodeKeyControl(gf::Keycode::Escape);
        addAction(m_back2menuAction);

        m_upAction.addKeycodeKeyControl(gf::Keycode::Up);
        addAction(m_upAction);

        m_downAction.addKeycodeKeyControl(gf::Keycode::Down);
        addAction(m_downAction);

        m_triggerAction.addMouseButtonControl(gf::MouseButton::Left);
        addAction(m_triggerAction);

        auto setupButtonBlue = [&] (gf::TextButtonWidget& button, auto callback) {
            button.setDefaultTextColor(gf::Color::Black);
            button.setDefaultBackgroundColor(gf::Color::Gray(0.7f));
            button.setSelectedTextColor(gf::Color::Black);
            button.setSelectedBackgroundColor(gf::Color::Cyan);
            button.setDisabledTextColor(gf::Color::Black);
            button.setDisabledBackgroundColor(gf::Color::Red);
            button.setAnchor(gf::Anchor::TopLeft);
            button.setAlignment(gf::Alignment::Center);
            button.setCallback(callback);
            m_widgets.addWidget(button);
        };

        auto setupButtonOrange = [&] (gf::TextButtonWidget& button, auto callback) {
            button.setDefaultTextColor(gf::Color::Black);
            button.setDefaultBackgroundColor(gf::Color::Gray(0.7f));
            button.setSelectedTextColor(gf::Color::Black);
            button.setSelectedBackgroundColor(gf::Color::Orange);
            button.setDisabledTextColor(gf::Color::Black);
            button.setDisabledBackgroundColor(gf::Color::Red);
            button.setAnchor(gf::Anchor::TopLeft);
            button.setAlignment(gf::Alignment::Center);
            button.setCallback(callback);
            m_widgets.addWidget(button);
        };

        setupButtonBlue(m_level1, [&] () {
            m_game.replaceAllScenes(m_game.level);
        });

        setupButtonOrange(m_back2menu, [&] () {
            m_game.replaceAllScenes(m_game.menu);
        });
    }

    void SelectLevelScene::doHandleActions([[maybe_unused]] gf::Window& window) {
        if (!isActive()) {
            return;
        }

        if (m_upAction.isActive()) {
            m_widgets.selectPreviousWidget();
        }

        if (m_downAction.isActive()) {
            m_widgets.selectNextWidget();
        }

        if (m_triggerAction.isActive()) {
            m_widgets.triggerAction();
        }

        if (m_back2menuAction.isActive()) {
            m_game.popAllScenes();
        }
    } 

    void SelectLevelScene::doProcessEvent(gf::Event& event) {
        switch (event.type)
        {
            case gf::EventType::MouseMoved:
            m_widgets.pointTo(m_game.computeWindowToGameCoordinates(event.mouseCursor.coords, getHudView()));
            break;
        }
    }

    void SelectLevelScene::doRender(gf::RenderTarget& target, const gf::RenderStates &states) {
        gf::Coordinates coords(target);

        float backgroundHeight = coords.getRelativeSize(gf::vec(0.0f, 1.0f)).height;
        float backgroundScale = backgroundHeight / m_backgroundTexture.getSize().height;

        gf::Sprite background(m_backgroundTexture);
        background.setColor(gf::Color::Opaque(0.20f));
        background.setPosition(coords.getCenter());
        background.setAnchor(gf::Anchor::Center);
        background.setScale(backgroundScale);
        target.draw(background, states);

        target.setView(getHudView());

        unsigned titleCharacterSize = coords.getRelativeCharacterSize(0.1f);

        gf::Text title("Level selection", m_game.resources.getFont("Underdog.otf"), titleCharacterSize);
        title.setColor(gf::Color::White);
        title.setPosition(coords.getRelativePoint({ 0.5f, 0.1f }));
        title.setAnchor(gf::Anchor::TopCenter);
        target.draw(title, states);

        constexpr float characterSize = 0.075f;
        constexpr float spaceBetweenButton = 0.045f;
        constexpr gf::Vector2f backgroundSize(0.5f, 0.3f);

        const float paragraphWidth = coords.getRelativeSize(backgroundSize - 0.05f).x;
        const float paddingSize = coords.getRelativeSize({0.01f, 0.f}).x;
        const unsigned resumeCharacterSize = coords.getRelativeCharacterSize(characterSize);

        m_level1.setCharacterSize(resumeCharacterSize);
        m_level1.setPosition(coords.getRelativePoint({0.275f, 0.425f}));
        m_level1.setParagraphWidth(paragraphWidth);
        m_level1.setPadding(paddingSize);

        m_back2menu.setCharacterSize(resumeCharacterSize);
        m_back2menu.setPosition(coords.getRelativePoint({0.275f, 0.425f + (characterSize + spaceBetweenButton) * 3}));
        m_back2menu.setParagraphWidth(paragraphWidth);
        m_back2menu.setPadding(paddingSize);

        m_widgets.render(target, states);
    }

    void SelectLevelScene::doShow() {
        m_widgets.clear();

        m_level1.setDefault();
        m_widgets.addWidget(m_level1);

        m_back2menu.setDefault();
        m_widgets.addWidget(m_back2menu);

        m_widgets.selectNextWidget();
    } 


}   