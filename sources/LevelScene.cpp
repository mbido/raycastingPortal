#include <gf/Coordinates.h>
#include <gf/Sprite.h>

#include "GameHub.hpp"

namespace gh
{

	LevelScene::LevelScene(GameHub &game)
		: gf::Scene(game.getRenderer().getSize()), m_game(game), m_back2menuAction("Menu"), m_map("../sources/map/image8.png"), m_player(m_map.getBeginCell() + 0.5, 0, 2), m_game3D(&m_player, &m_map, game.getRenderer()), m_direction(0.0f, 0.0f), m_angularVelocity(0), m_upAction("UpAction"), m_downAction("DownAction"), m_rightAction("RightAction"), m_leftAction("LeftAction"), m_lookLeftAction("LookLeft"), m_lookRightAction("LookRight")
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

		m_mouseCursorCurr = {-1.0f, -1.0f}, m_mouseCursorNext = {-1.0f, -1.0f};
		m_sensivity = 0.05f;
	}

	LevelScene::LevelScene(GameHub &game, int level)
		: gf::Scene(game.getRenderer().getSize()), m_game(game), m_back2menuAction("Menu"), m_map("../sources/map/image" + std::to_string(level) + ".png"), m_player(m_map.getBeginCell() + 0.5, 0, 2), m_game3D(&m_player, &m_map, game.getRenderer()), m_direction(0.0f, 0.0f), m_angularVelocity(0), m_upAction("UpAction"), m_downAction("DownAction"), m_rightAction("RightAction"), m_leftAction("LeftAction"), m_lookLeftAction("LookLeft"), m_lookRightAction("LookRight")
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

		m_mouseCursorCurr = {-1.0f, -1.0f}, m_mouseCursorNext = {-1.0f, -1.0f};
		m_sensivity = 0.05f;
	}

	void LevelScene::doHandleActions([[maybe_unused]] gf::Window &window)
	{
		gf::Time time = m_clock.restart();

		if (!isActive())
		{
			return;
		}

		if (m_rightAction.isActive())
		{
			m_direction += gf::Vector2f(1.0f, 0.0f);
			// printf("d pressed\n");
		}
		if (m_leftAction.isActive())
		{
			m_direction += gf::Vector2f(-1.0f, 0.0f);
			// printf("q pressed\n");
		}
		if (m_upAction.isActive())
		{
			m_direction += gf::Vector2f(0.0f, -1.0f);
			// printf("z pressed\n");
		}
		if (m_downAction.isActive())
		{
			m_direction += gf::Vector2f(0.0f, 1.0f);
			// printf("s pressed\n");
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
			m_game.getWindow().setMouseRelative(false);
			m_game.replaceAllScenes(m_game.selectLevel);
		}

		m_player.setVelocity(m_direction);
		m_player.setAngularVelocity(m_angularVelocity);

		m_game3D.update(time);

		m_direction = gf::Vector2f(0.0f, 0.0f);
		m_angularVelocity = 0;

		if ((int)m_player.getPosition().x == (int)m_map.getArrivalCell().x && (int)m_player.getPosition().y == (int)m_map.getArrivalCell().y)
		{
			m_player.setPosition(m_map.getBeginCell() + 0.5);
			m_game.getWindow().setMouseRelative(false);
			m_game.replaceAllScenes(m_game.level1End);
		}
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
		case gf::EventType::MouseMoved:
			if (m_mouseCursorCurr.x == -1 && m_mouseCursorCurr.y == -1)
			{
				m_mouseCursorCurr = event.mouseCursor.motion;
			}
			m_mouseCursorNext = event.mouseCursor.motion;

			gf::Vector2f mouseDelta;
			if (m_mouseCursorCurr.x != m_mouseCursorNext.x)
			{
				if (m_mouseCursorNext.x < 0)
				{
					mouseDelta = m_mouseCursorCurr + m_mouseCursorNext;
				}
				else if (m_mouseCursorNext.x > 0)
				{
					mouseDelta = m_mouseCursorNext + m_mouseCursorCurr;
				}

				float yaw = mouseDelta.x * m_sensivity;
				float pitch = mouseDelta.y * m_sensivity;

				// pitch = std::clamp(pitch, -89.0f, 89.0f);
				// std::cout << yaw << std::endl;

				if (yaw < 0)
				{
					// std::cout << "Mouse cursor position curr : (" << m_mouseCursorCurr.x << ";" << m_mouseCursorCurr.y << ")" << std::endl
					//           << "Mouse cursor position next : (" << m_mouseCursorNext.x << ";" << m_mouseCursorNext.y << ")" << std::endl;
					// std::cout << yaw << std::endl;
					// std::cout << "Look at left" << std::endl;
					m_angularVelocity -= (gf::Pi / 2) * (1 + std::abs(yaw));
				}
				if (yaw > 0)
				{
					// std::cout << "Mouse cursor position curr : (" << m_mouseCursorCurr.x << ";" << m_mouseCursorCurr.y << ")" << std::endl
					//           << "Mouse cursor position next : (" << m_mouseCursorNext.x << ";" << m_mouseCursorNext.y << ")" << std::endl;
					// std::cout << yaw << std::endl;
					m_angularVelocity += (gf::Pi / 2) * (1 + std::abs(yaw));
					// std::cout << "Look at right" << std::endl;
				}
			}
			m_mouseCursorCurr = m_mouseCursorNext;

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