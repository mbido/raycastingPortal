#include <gf/Action.h>
#include <gf/Clock.h>
#include <gf/Color.h>
#include <gf/EntityContainer.h>
#include <gf/Event.h>
#include <gf/RenderWindow.h>
#include <gf/ViewContainer.h>
#include <gf/Views.h>
#include <gf/Window.h>
#include <gf/Shapes.h>
#include <gf/Vector.h>
#include <gf/RenderTarget.h>
#include <gf/Math.h>

#include <cstdio>

#include "Player.hpp"
#include "MapTemplate.hpp"
#include "MapWalls.hpp"
#include "Game2D.hpp"
 
int main() {
  static constexpr gf::Vector2i ScreenSize(1024, 576);
  static constexpr gf::Vector2f ViewSize(100.0f, 100.0f); // dummy values
  static constexpr gf::Vector2f ViewCenter(0.0f, 0.0f); // dummy values
 
  // initialization
 
  gf::Window window("Game", ScreenSize);
  window.setVerticalSyncEnabled(true);
  window.setFramerateLimit(60);
 
  gf::RenderWindow renderer(window);
 
  // views
 
  gf::ViewContainer views;
 
  gf::ScreenView hudView;
  views.addView(hudView);
 
  views.setInitialFramebufferSize(ScreenSize);
 
  // actions
 
  gf::ActionContainer actions;
 
  gf::Action closeWindowAction("Close window");
  closeWindowAction.addCloseControl();
  closeWindowAction.addKeycodeKeyControl(gf::Keycode::Escape);
  actions.addAction(closeWindowAction);
 
  gf::Action fullscreenAction("Fullscreen");
  fullscreenAction.addKeycodeKeyControl(gf::Keycode::F);
  actions.addAction(fullscreenAction);
 
  gf::Action leftAction("Left");
  leftAction.addScancodeKeyControl(gf::Scancode::A);
  leftAction.setContinuous();
  actions.addAction(leftAction);
 
  gf::Action rightAction("Right");
  rightAction.addScancodeKeyControl(gf::Scancode::D);
  rightAction.setContinuous();
  actions.addAction(rightAction);
 
  gf::Action upAction("Up");
  upAction.addScancodeKeyControl(gf::Scancode::W);
  upAction.addScancodeKeyControl(gf::Scancode::Up);
  upAction.setContinuous();
  actions.addAction(upAction);
 
  gf::Action downAction("Down");
  downAction.addScancodeKeyControl(gf::Scancode::S);
  downAction.addScancodeKeyControl(gf::Scancode::Down);
  downAction.setContinuous();
  actions.addAction(downAction);

  gf::Action lookLeftAction("LookLeft");
  lookLeftAction.addScancodeKeyControl(gf::Scancode::Left);
  lookLeftAction.setContinuous();
  actions.addAction(lookLeftAction);
 
  gf::Action lookRightAction("LookRight");
  lookRightAction.addScancodeKeyControl(gf::Scancode::Right);
  lookRightAction.setContinuous();
  actions.addAction(lookRightAction);
 
  // entities
  Player player(gf::Vector2f(5, 5));
 
  // map
  const int nbRows = 10;
  const int nbColumns = 10;
  MapWalls map(nbRows, nbColumns);
  map.setTile(3, 3, 1);
  map.setTile(9, 5, 0);

  // game
  Game2D game(&player, &map, renderer);

 
  // game loop
 
  renderer.clear(gf::Color::White);
 
  gf::Clock clock;
 
  while (window.isOpen()) {

    // 1. input
 
    gf::Event event;
 
    while (window.pollEvent(event)) {
      actions.processEvent(event);
      views.processEvent(event);
    }
 
    if (closeWindowAction.isActive()) {
      window.close();
    }
 
    if (fullscreenAction.isActive()) {
      window.toggleFullscreen();
    }

    gf::Vector2f direction(0.0f, 0.0f);
    if (rightAction.isActive()) {
      direction += gf::Vector2f(1.0f, 0.0f);
    }
    if (leftAction.isActive()) {
      direction += gf::Vector2f(-1.0f, 0.0f);
    }
    if (upAction.isActive()) {
      direction += gf::Vector2f(0.0f, -1.0f);
    }
    if (downAction.isActive()) {
      direction += gf::Vector2f(0.0f, 1.0f);
    }
    player.setVelocity(direction);

    float angularVelocity = 0.0f;
    if (lookLeftAction.isActive()) {
      angularVelocity -= gf::Pi / 4;
    }
    if (lookRightAction.isActive()) {
      angularVelocity += gf::Pi / 4;
    }
    player.setAngularVelocity(angularVelocity);
 
    // 2. update
 
    gf::Time time = clock.restart();

    game.update(time);
 
    // 3. draw
 
    renderer.clear();

    renderer.setView(hudView);

    game.render();

    renderer.display();
 
    actions.reset();
  }
 
  return 0;
}