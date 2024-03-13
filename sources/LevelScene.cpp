#include <gf/Coordinates.h>
#include <gf/Sprite.h>

#include "GameHub.hpp"

namespace gh
{

	LevelScene::LevelScene(GameHub &game)
		: gf::Scene(game.getRenderer().getSize()), m_game(game), m_back2menuAction("Menu"), m_map("../sources/map/image.png"), m_player(gf::Vector2f(1.5, 1.5), 0, 2), m_game3D(&m_player, &m_map, game.getRenderer()), m_direction(0.0f, 0.0f), m_angularVelocity(0), m_upAction("UpAction"), m_downAction("DownAction"), m_rightAction("RightAction"), m_leftAction("LeftAction"), m_lookLeftAction("LookLeft"), m_lookRightAction("LookRight")
	{
		setClearColor(gf::Color::Black);

		m_back2menuAction.addScancodeKeyControl(gf::Scancode::Escape);
		addAction(m_back2menuAction);

		m_upAction.addScancodeKeyControl(gf::Scancode::Up);
		m_upAction.addScancodeKeyControl(gf::Scancode::W);
		m_upAction.setContinuous();
		addAction(m_upAction);

		m_downAction.addScancodeKeyControl(gf::Scancode::Down);
		m_downAction.addScancodeKeyControl(gf::Scancode::S);
		m_downAction.setContinuous();
		addAction(m_downAction);

		// gf::Action fullscreenAction("Fullscreen");
		// fullscreenAction.addKeycodeKeyControl(gf::Keycode::F);
		// actions.addAction(fullscreenAction);

		m_rightAction.addScancodeKeyControl(gf::Scancode::D);
		m_rightAction.setContinuous();
		addAction(m_rightAction);

		m_leftAction.addScancodeKeyControl(gf::Scancode::A);
		m_leftAction.setContinuous();
		addAction(m_leftAction);

		m_lookLeftAction.addScancodeKeyControl(gf::Scancode::Left);
		m_lookLeftAction.setContinuous();
		addAction(m_lookLeftAction);

		m_lookRightAction.addScancodeKeyControl(gf::Scancode::Right);
		m_lookRightAction.setContinuous();
		addAction(m_lookRightAction);
	}

	LevelScene::LevelScene(GameHub &game, int level)
		: gf::Scene(game.getRenderer().getSize()), m_game(game), m_back2menuAction("Menu"), m_map("../sources/map/image" + std::to_string(level) + ".png"), m_player(gf::Vector2f(1.5, 1.5), 0, 2), m_game3D(&m_player, &m_map, game.getRenderer()), m_direction(0.0f, 0.0f), m_angularVelocity(0), m_upAction("UpAction"), m_downAction("DownAction"), m_rightAction("RightAction"), m_leftAction("LeftAction"), m_lookLeftAction("LookLeft"), m_lookRightAction("LookRight")
	{
		

		setClearColor(gf::Color::Black);

		m_back2menuAction.addScancodeKeyControl(gf::Scancode::Escape);
		addAction(m_back2menuAction);

		m_upAction.addScancodeKeyControl(gf::Scancode::Up);
		m_upAction.addScancodeKeyControl(gf::Scancode::W);
		m_upAction.setContinuous();
		addAction(m_upAction);

		m_downAction.addScancodeKeyControl(gf::Scancode::Down);
		m_downAction.addScancodeKeyControl(gf::Scancode::S);
		m_downAction.setContinuous();
		addAction(m_downAction);

		// gf::Action fullscreenAction("Fullscreen");
		// fullscreenAction.addKeycodeKeyControl(gf::Keycode::F);
		// actions.addAction(fullscreenAction);

		m_rightAction.addScancodeKeyControl(gf::Scancode::D);
		m_rightAction.setContinuous();
		addAction(m_rightAction);

		m_leftAction.addScancodeKeyControl(gf::Scancode::A);
		m_leftAction.setContinuous();
		addAction(m_leftAction);

		m_lookLeftAction.addScancodeKeyControl(gf::Scancode::Left);
		m_lookLeftAction.setContinuous();
		addAction(m_lookLeftAction);

		m_lookRightAction.addScancodeKeyControl(gf::Scancode::Right);
		m_lookRightAction.setContinuous();
		addAction(m_lookRightAction);
	}

	void LevelScene::doHandleActions([[maybe_unused]] gf::Window & window)
	{
		gf::Time time = m_clock.restart();

		if (!isActive())
		{
			return;
		}

		if (m_rightAction.isActive())
		{
			m_direction += gf::Vector2f(1.0f, 0.0f);
			printf("d pressed\n");
		}
		if (m_leftAction.isActive())
		{
			m_direction += gf::Vector2f(-1.0f, 0.0f);
			printf("q pressed\n");
		}
		if (m_upAction.isActive())
		{
			m_direction += gf::Vector2f(0.0f, -1.0f);
			printf("z pressed\n");
		}
		if (m_downAction.isActive())
		{
			m_direction += gf::Vector2f(0.0f, 1.0f);
			printf("s pressed\n");
		}
		if (m_lookLeftAction.isActive())
		{
			m_angularVelocity -= gf::Pi / 2;
		}
		if (m_lookRightAction.isActive())
		{
			m_angularVelocity += gf::Pi / 2;
		}

		if (m_back2menuAction.isActive())
		{
			m_game.replaceAllScenes(m_game.selectLevel);
		}

		m_player.setVelocity(m_direction);
		m_player.setAngularVelocity(m_angularVelocity);

		m_game3D.update(time);

		m_direction = gf::Vector2f(0.0f, 0.0f);
		m_angularVelocity = 0;
	}

	void LevelScene::doProcessEvent(gf::Event &event)
	{
		switch (event.type)
		{
		case gf::EventType::MouseButtonPressed:
			if (event.mouseButton.button == gf::MouseButton::Left || event.mouseButton.button == gf::MouseButton::Right)
			{
				gf::MouseButtonEvent mouseButtonEvent = event.mouseButton;
				if (mouseButtonEvent.button == gf::MouseButton::Left)
					m_game3D.castPortal(true);
				else if (mouseButtonEvent.button == gf::MouseButton::Right)
					m_game3D.castPortal(false);
			}
			break;
		default:
			break;
		}
	}

	void LevelScene::doRender(gf::RenderTarget &target, const gf::RenderStates &states)
	{
		m_game3D.render();
	}

	void LevelScene::doShow()
	{
		m_game3D.render();
	}

}