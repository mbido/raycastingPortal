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

#include <gf/Texture.h>
#include <gf/Cursor.h>
#include <filesystem>

#include <cstdio>

#include "Player.hpp"
#include "MapTemplate.hpp"
#include "MapWalls.hpp"
#include "Game2D.hpp"
#include "Game3D.hpp"
#include <unistd.h>

#define RENDER_IN_3D

int main()
{
  static constexpr gf::Vector2i ScreenSize(1024, 576);
  static constexpr gf::Vector2f ViewSize(1024.0f, 576.0f);
  static constexpr gf::Vector2f ViewCenter(512.0f, 278.0f);

  // initialization

  gf::Window window("Game", ScreenSize);
  window.setVerticalSyncEnabled(true);
  window.setFramerateLimit(0);

  gf::RenderWindow renderer(window);

  #ifdef RENDER_IN_3D
  gf::Cursor cursor(gf::Cursor::Type::Cross);
  window.setMouseCursor(cursor);
  window.setMouseCursorGrabbed(true);
  // window.setMouseCursorVisible(false);
  #endif

  // views

  gf::ViewContainer views;

  gf::FitView mainView(ViewCenter, ViewSize);
  views.addView(mainView);

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

  // mouse
  gf::MouseButtonEvent mouseButtonEvent;
  gf::Vector2f bluePortal(0.0f, 0.0f);
  gf::Vector2f orangePortal(0.0f, 0.0f);
  gf::Vector2f mouseCursorCurr = {-1.0f, -1.0f}, mouseCursorNext = {-1.0f, -1.0f};

  // entitie
  Player player(gf::Vector2f(1.5, 1.5), 0, 2);

  // map
  std::filesystem::path cheminRelatif("./sources/map/image.png");
  std::string cheminAbsolu = std::filesystem::absolute(cheminRelatif).string();

  // MapWalls map(cheminAbsolu);
  MapWalls map;

  /*const int nbRows = 10;
  const int nbColumns = 10;

  MapWalls map(nbRows, nbColumns);


  map.setTile(1, 2, 1);
  map.setTile(1, 6, 1);
  map.setTile(2, 2, 1);
  map.setTile(2, 4, 1);
  map.setTile(2, 5, 1);
  map.setTile(2, 6, 1);
  map.setTile(2, 8, 1);
  map.setTile(3, 6, 1);
  map.setTile(4, 1, 1);
  map.setTile(4, 2, 1);
  map.setTile(4, 3, 1);
  map.setTile(4, 4, 1);
  map.setTile(6, 3, 1);
  map.setTile(6, 4, 1);
  map.setTile(6, 5, 1);
  map.setTile(6, 6, 1);
  map.setTile(6, 7, 1);
  map.setTile(7, 6, 1);
  map.setTile(8, 6, 1);*/

  // game

#ifdef RENDER_IN_3D
  Game3D game(&player, &map, renderer);
#else
  Game2D game(&player, &map, renderer);
#endif

  // game loop

  renderer.clear(gf::Color::Black);

  gf::Clock clock;

  gf::Clock fpsClock;
  int frameCount = 0;

  while (window.isOpen())
  {

    // 1. input

    gf::Event event;

    while (window.pollEvent(event))
    {
      actions.processEvent(event);
      views.processEvent(event);
    }

    if (closeWindowAction.isActive())
    {
      window.close();
    }

    if (fullscreenAction.isActive())
    {
      window.toggleFullscreen();
    }

    gf::Vector2f direction(0.0f, 0.0f);
    if (rightAction.isActive())
    {
      direction += gf::Vector2f(1.0f, 0.0f);
    }
    if (leftAction.isActive())
    {
      direction += gf::Vector2f(-1.0f, 0.0f);
    }
    if (upAction.isActive())
    {
      direction += gf::Vector2f(0.0f, -1.0f);
    }
    if (downAction.isActive())
    {
      direction += gf::Vector2f(0.0f, 1.0f);
    }
    player.setVelocity(direction);

    float angularVelocity = 0.0f;
    float yaw = 0.0f, pitch = 0.0f, sensivity = 0.1f;

    if(mouseCursorCurr.x == -1 && mouseCursorCurr.y == -1){
      mouseCursorCurr = event.mouseCursor.coords;
    }else{
      mouseCursorNext = event.mouseCursor.coords;
      gf::Vector2f mouseDelta = mouseCursorNext - mouseCursorCurr;
      yaw = mouseDelta.x * sensivity;
      pitch = mouseDelta.y * sensivity;

      pitch = std::clamp(pitch, -89.0f, 89.0f);

      if(yaw < 0 && mouseCursorCurr.x != mouseCursorNext.x){
        // std::cout << "Look at left" << std::endl;
        angularVelocity -= gf::Pi / 2;
      }
      if(yaw > 0 && mouseCursorCurr.x != mouseCursorNext.x){
        angularVelocity += gf::Pi / 2;
        // std::cout << "Look at right" << std::endl;
      }
      // if(mouseCursorNext.x == mouseCursorCurr.x){
      //   std::cout << "Static" << std::endl;
      // }

      // std::cout << "Mouse cursor position curr : (" << mouseCursorCurr.x << ";" << mouseCursorCurr.y << ")" << std::endl
      //           << "Mouse cursor position next : (" << mouseCursorNext.x << ";" << mouseCursorNext.y << ")" << std::endl
      //           << "Mouse delta : (" << mouseDelta.x << ";" << mouseDelta.y << ")" << std::endl
      //           << "Yaw : " << yaw << std::endl
      //           << "Pitch : " << pitch << std::endl;
      mouseCursorCurr = mouseCursorNext;
    }
    if (lookLeftAction.isActive())
    {
      angularVelocity -= gf::Pi / 2;
    }
    if (lookRightAction.isActive())
    {
      angularVelocity += gf::Pi / 2;
    }
    player.setAngularVelocity(angularVelocity);

    // 2. update

    gf::Time time = clock.restart();

    // // <-- We test the collisions here !
    // float nextAngle = player.getAngle() + angularVelocity * time.asSeconds();

    // gf::Rotation nextRotator(nextAngle + gf::Pi / 2);
    // gf::Vector2f nextNewVelocity = gf::transform(nextRotator, direction);

    // if (nextNewVelocity != gf::vec(0.0f, 0.0f)) {
    //   nextNewVelocity = gf::normalize(nextNewVelocity);
    // }

    // gf::Vector2f nextPosition = player.getPosition() + (nextNewVelocity * time.asSeconds());

    // int intXPartNextPos = (int) floorf(nextPosition.x);
    // int intYPartNextPos = (int) floorf(nextPosition.y);
    // gf::Vector2f nextCorrectDir = gf::Vector2f(0.0f,1.0f);

    // if (map.getTile(intXPartNextPos,intYPartNextPos) != 0)
    // {
    //   // In a wall or outside of the map (same treatment)
    //   double currentAngle = player.getAngle();
    //   int currentIntXPart = (int) floorf(player.getPosition().x);
    //   int currentIntYPart = (int) floorf(player.getPosition().y);

    //   if (currentIntYPart == intYPartNextPos) //currentIntXPart != intXPartNextPos &&
    //   {
    //     if (currentAngle < gf::Pi / 2 || currentAngle >= 3 * gf::Pi / 2) {
    //       nextCorrectDir.x += 2*sinf(currentAngle);
    //     }
    //     else {
    //       nextCorrectDir.x += -2*sinf(currentAngle);
    //     }
    //   }

    //   if (currentIntXPart == intXPartNextPos) //currentIntYPart != intYPartNextPos &&
    //   {
    //     if (currentAngle < gf::Pi) {
    //       nextCorrectDir.x += -2*cosf(currentAngle);
    //     }
    //     else {
    //       nextCorrectDir.x += 2*cosf(currentAngle);
    //     }
    //   }
    //   player.setVelocity(nextCorrectDir);
    // }

    // fps :
    // frameCount++;
    // if (fpsClock.getElapsedTime() >= gf::seconds(1.0f)) {
    //   float fps = frameCount / fpsClock.restart().asSeconds();
    //   //std::cout << "FPS: " << fps << std::endl;
    //   //std::cout << "Player Position : (" << player.getPosition().x << ";" << player.getPosition().y << ")" << std::endl;
    //   //std::cout << "Next Player Position : (" << nextPosition.x << ";" << nextPosition.y << ")" << std::endl;
    //   //std::cout << "Direction : (" << direction.x << ";" << direction.y << ")" << std::endl;
    //   //std::cout << "Next Correct direction : (" << nextCorrectDir.x << ";" << nextCorrectDir.y << ")" << std::endl;
    //   //std::cout << "Angle : " << player.getAngle() << std::endl;
    //   frameCount = 0;
    // }

    if (event.type == gf::EventType::MouseButtonPressed && (event.mouseButton.button == gf::MouseButton::Left || event.mouseButton.button == gf::MouseButton::Right))
    {
      mouseButtonEvent = event.mouseButton;
      std::cout << "Mouse button pressed at (" << mouseButtonEvent.coords.x << ";" << mouseButtonEvent.coords.y << ")" << std::endl;
      if (mouseButtonEvent.button == gf::MouseButton::Left)
      {
        // bluePortal = mouseButtonEvent.coords;
        // std::cout << "Left button pressed" << std::endl;
        game.castPortal(true);
      }
      else if (mouseButtonEvent.button == gf::MouseButton::Right)
      {
        // orangePortal = mouseButtonEvent.coords;
        // std::cout << "Right button pressed" << std::endl;
        game.castPortal(false);
      }
      // std::cout << "Player Position : (" << player.getPosition().x << ";" << player.getPosition().y << ")" << std::endl
      //           << "Player Angle : " << player.getAngle() << std::endl
      //           << "Player direction : (" << std::cos(player.getAngle()) << ";" << std::sin(player.getAngle()) << ")" << std::endl;
    }

    game.update(time);

    // 3. draw

    renderer.clear();

#ifdef RENDER_IN_3D
    renderer.setView(mainView);
    game.render();
#else
    renderer.setView(hudView);
    // game.render(true, std::make_pair(gf::Vector2i(3, 2), gf::Vector2i(3, 3)));
    game.render();
#endif

    renderer.display();

    actions.reset();
  }

  return 0;
}
