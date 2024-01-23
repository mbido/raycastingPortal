#include <gf/Coordinates.h>
#include <gf/Sprite.h>

#include "GameHub.hpp"


namespace gh {

    MenuScene::MenuScene(GameHub& game)
    : gf::Scene(game.getRenderer().getSize())
    , m_game(game)
    , m_backgroundTexture(game.resources.getTexture("bgTexture.png"))
    , m_upAction("UpAction")
    , m_downAction("DownAction")
    , m_triggerAction("TriggerAction")
    , m_quitAction("Quit")
    , m_newGame("New Game", game.resources.getFont("Underdog.otf"))
    , m_quit("Quit", game.resources.getFont("Underdog.otf"))
    {
        setClearColor(gf::Color::Black);

        m_quitAction.addKeycodeKeyControl(gf::Keycode::Escape);
        addAction(m_quitAction);

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
            button.setSelectedBackgroundColor(gf::Color::fromRgb(0.4, 0.8, 1.0));
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

        setupButtonBlue(m_newGame, [&] () {
            m_game.replaceAllScenes(m_game.selectLevel);
        });

        setupButtonOrange(m_quit, [&] () {
            m_game.popAllScenes();
        });
    }

    void MenuScene::doHandleActions([[maybe_unused]] gf::Window& window) {
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

        if (m_quitAction.isActive()) {
            m_game.popAllScenes();
        }
    } 

    void MenuScene::doProcessEvent(gf::Event& event) {
        switch (event.type)
        {
            case gf::EventType::MouseMoved:
            m_widgets.pointTo(m_game.computeWindowToGameCoordinates(event.mouseCursor.coords, getHudView()));
            break;
        }
    }

    void MenuScene::doRender(gf::RenderTarget& target, const gf::RenderStates &states) {
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

        gf::Text title("Portal 0.0", m_game.resources.getFont("Underdog.otf"), titleCharacterSize);
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

        m_newGame.setCharacterSize(resumeCharacterSize);
        m_newGame.setPosition(coords.getRelativePoint({0.275f, 0.425f}));
        m_newGame.setParagraphWidth(paragraphWidth);
        m_newGame.setPadding(paddingSize);

        m_quit.setCharacterSize(resumeCharacterSize);
        m_quit.setPosition(coords.getRelativePoint({0.275f, 0.425f + (characterSize + spaceBetweenButton) * 3}));
        m_quit.setParagraphWidth(paragraphWidth);
        m_quit.setPadding(paddingSize);

        m_widgets.render(target, states);
    }

    void MenuScene::doShow() {
        m_widgets.clear();

        m_newGame.setDefault();
        m_widgets.addWidget(m_newGame);

        m_quit.setDefault();
        m_widgets.addWidget(m_quit);

        m_widgets.selectNextWidget();
    } 


}   