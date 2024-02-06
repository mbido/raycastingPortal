#include <gf/Action.h>
#include <gf/Clock.h>
#include <gf/Color.h>
#include <gf/Cursor.h>
#include <gf/EntityContainer.h>
#include <gf/Event.h>
#include <gf/GameManager.h>
#include <gf/Math.h>
#include <gf/RenderTarget.h>
#include <gf/RenderWindow.h>
#include <gf/RenderTexture.h>
#include <gf/Shapes.h>
#include <gf/Texture.h>
#include <gf/Vector.h>
#include <gf/ViewContainer.h>
#include <gf/Views.h>
#include <gf/Window.h>
#include <gf/Sprite.h>

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <unistd.h>

#include "GameHub.hpp"
#include "Game2D.hpp"
#include "Game3D.hpp"
#include "MapTemplate.hpp"
#include "MapWalls.hpp"
#include "Player.hpp"

#define RENDER_IN_3D
// #define WORKING_ON_SCENES

gf::Vector2f linesIntersectionTEST(gf::Vector2f A, gf::Vector2f B, gf::Vector2f C, gf::Vector2f D)
{
    float a1 = B.y - A.y;
    float b1 = A.x - B.x;
    float c1 = a1 * A.x + b1 * A.y;

    float a2 = D.y - C.y;
    float b2 = C.x - D.x;
    float c2 = a2 * C.x + b2 * C.y;

    float delta = a1 * b2 - a2 * b1;

    if (delta == 0)
    {
        return gf::Vector2f(0, 0);
    }
    else
    {
        return gf::Vector2f((b2 * c1 - b1 * c2) / delta, (a1 * c2 - a2 * c1) / delta);
    }
}

void renderAWallTexturedTEST(gf::Vector2f start, gf::Vector2f end, std::vector<gf::Vector2f> trapezeVertices, gf::Texture &texture, gf::RenderWindow &renderer)
{
    // trapezeVertices is a vector of the trapeze's vertices from left to right, top to bottom

    // lets determine the center of the texture on the trapeze
    // => the center is the intersection of the diagonals of the trapeze

    gf::Vector2f center = linesIntersectionTEST(trapezeVertices[0], trapezeVertices[3], trapezeVertices[1], trapezeVertices[2]);

    gf::Vector2f topCenter = linesIntersectionTEST(center, gf::Vector2f(center.x, center.y + 1), trapezeVertices[0], trapezeVertices[1]);
    gf::Vector2f bottomCenter = linesIntersectionTEST(center, gf::Vector2f(center.x, center.y + 1), trapezeVertices[2], trapezeVertices[3]);

    float textureCenter = (start.x + end.x) / 2;

    gf::VertexArray trapeze(gf::PrimitiveType::Triangles, 24);
    // First triangle (Top-Left, Center, Top-Center)
    trapeze[0].position = trapezeVertices[0];  // Top-Left
    trapeze[1].position = center;  // Center
    trapeze[2].position = topCenter; // Top-Center

    // Deuxième triangle (Top-Center, Center, Top-Right)
    trapeze[3].position = topCenter; // Top-Center
    trapeze[4].position = center; // Center
    trapeze[5].position = trapezeVertices[1];  // Top-Right

    // Troisième triangle (Bottom-gauche, Center, Bottom-Center)
    trapeze[6].position = trapezeVertices[2];   // Bottom-gauche
    trapeze[7].position = center;   // Center
    trapeze[8].position = bottomCenter; // Bottom-Center

    // Quatrième triangle (Bottom-Center, Center, Bottom-Right)
    trapeze[9].position = bottomCenter; // Bottom-Center
    trapeze[10].position = center;  // Center
    trapeze[11].position = trapezeVertices[3];  // Bottom-Right

    // Cinquième triangle (Top-Left, Center, Bottom-Gauche)
    trapeze[12].position = trapezeVertices[0]; // Top-Left
    trapeze[13].position = center; // Center
    trapeze[14].position = trapezeVertices[2]; // Bottom-gauche

    // Sixième triangle (Top-Right, Center, Bottom-Right)
    trapeze[15].position = trapezeVertices[1];  // Top-Right
    trapeze[16].position = center; // Center
    trapeze[17].position = trapezeVertices[3]; // Bottom-Right

    // Texture coordinates
    int scale = 3;
    trapeze[0].texCoords = {start.x, 0.0f}; // Top-Left
    trapeze[1].texCoords = {textureCenter, 0.5f}; // Center
    trapeze[2].texCoords = {textureCenter, 0.0f}; // Top-Center

    trapeze[3].texCoords = {textureCenter, 0.0f}; // Top-Center
    trapeze[4].texCoords = {textureCenter, 0.5f}; // Center
    trapeze[5].texCoords = {end.x, 0.0f}; // Top-Right

    trapeze[6].texCoords = {start.x, 1.0f}; // Bottom-left
    trapeze[7].texCoords = {textureCenter, 0.5f}; // Center
    trapeze[8].texCoords = {textureCenter, 1.0f}; // Bottom-Center

    trapeze[9].texCoords = {textureCenter, 1.0f};  // Bottom-Center
    trapeze[10].texCoords = {textureCenter, 0.5f}; // Center
    trapeze[11].texCoords = {end.x, 1.0f}; // Bottom-Right

    trapeze[12].texCoords = {start.x, 0.0f}; // Top-Left
    trapeze[13].texCoords = {textureCenter, 0.5f}; // Center
    trapeze[14].texCoords = {start.x, 1.0f}; // Bottom-left

    trapeze[15].texCoords = {end.x, 0.0f}; // Top-Right
    trapeze[16].texCoords = {textureCenter, 0.5f}; // Center
    trapeze[17].texCoords = {end.x, 1.0f}; // Bottom-Right

    texture.setRepeated(true);

    gf::RenderStates states;
    states.texture[0] = &texture;

    renderer.draw(trapeze, states);
}


int main()
{

#ifdef WORKING_ON_SCENES

  gh::GameHub hub;
  hub.run();

#else

  // initialization

  static constexpr gf::Vector2i ScreenSize(1024, 576);
  static constexpr gf::Vector2f ViewSize(1024.0f, 576.0f);
  static constexpr gf::Vector2f ViewCenter(512.0f, 278.0f);

  gf::Window window("Game", ScreenSize);
  window.setVerticalSyncEnabled(true);
  window.setFramerateLimit(0);

  gf::RenderWindow renderer(window);

#ifdef RENDER_IN_3D

  // gf::Cursor cursor(gf::Cursor::Type::Cross);
  // window.setMouseCursor(cursor);
  // window.setMouseCursorGrabbed(true);

  window.setMouseRelative(true);
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

  //-----------------------------------------------

  // // testing the textures :
  // gf::VertexArray trapeze(gf::PrimitiveType::Triangles, 24);
  // // Premier triangle (Haut-gauche, Milieu, Haut-Milieu)
  // trapeze[0].position = {500.0f, 100.0f}; // Haut-gauche
  // trapeze[1].position = {550.0f, 150.0f}; // Milieu
  // trapeze[2].position = {550.0f, 83.333f};  // Haut-Milieu

  // // Deuxième triangle (Haut-Milieu, Milieu, Haut-droite)
  // trapeze[3].position = {550.0f, 83.333}; // Haut-Milieu
  // trapeze[4].position = {550.0f, 150.0f}; // Milieu
  // trapeze[5].position = {650.0f, 50.0f}; // Haut-droite

  // // Troisième triangle (Bas-gauche, Milieu, Bas-Milieu)
  // trapeze[6].position = {500.0f, 200.0f}; // Bas-gauche
  // trapeze[7].position = {550.0f, 150.0f}; // Milieu
  // trapeze[8].position = {550.0f, 216.666f}; // Bas-Milieu

  // // Quatrième triangle (Bas-Milieu, Milieu, Bas-droite)
  // trapeze[9].position = {550.0f, 216.666f}; // Bas-Milieu
  // trapeze[10].position = {550.0f, 150.0f}; // Milieu
  // trapeze[11].position = {650.0f, 250.0f}; // Bas-droite

  // // Cinquième triangle (Haut-gauche, Milieu, Bas-Gauche)
  // trapeze[12].position = {500.0f, 100.0f}; // Haut-gauche
  // trapeze[13].position = {550.0f, 150.0f}; // Milieu
  // trapeze[14].position = {500.0f, 200.0f}; // Bas-gauche

  // // Sixième triangle (Haut-droite, Milieu, Bas-droite)
  // trapeze[15].position = {650.0f, 50.0f}; // Haut-droite
  // trapeze[16].position = {550.0f, 150.0f}; // Milieu
  // trapeze[17].position = {650.0f, 250.0f}; // Bas-droite


  gf::VertexArray trapeze(gf::PrimitiveType::Triangles, 3);
  // Premier triangle (Haut-gauche, Milieu, Haut-Milieu)
  trapeze[0].position = {500.0f, 100.0f}; // B
  trapeze[1].position = {450.0f, 200.0f}; // F
  trapeze[2].position = {362.5f, 200.0f}; // E


  // Texture coordinates
  int scale = 1;
  trapeze[0].texCoords = {1.0f * scale, 0.0f}; // B
  trapeze[1].texCoords = {0.83f * scale, 0.5f}; // F
  trapeze[2].texCoords = {0.16f * scale, 0.5f}; // E


  gf::Texture texture(std::filesystem::path("../sources/resources/bricks-01.png"));
  texture.setRepeated(true);

  gf::RenderStates states;
  states.texture[0] = &texture;
  

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

    if (mouseCursorCurr.x == -1 && mouseCursorCurr.y == -1)
    {
      mouseCursorCurr = event.mouseCursor.motion;
    }
    if (event.type == gf::EventType::MouseMoved)
    {
      mouseCursorNext = event.mouseCursor.motion;

      gf::Vector2f mouseDelta;
      if (mouseCursorCurr.x != mouseCursorNext.x)
      {
        if (mouseCursorNext.x < 0)
        {
          mouseDelta = mouseCursorCurr + mouseCursorNext;
        }
        else if (mouseCursorNext.x > 0)
        {
          mouseDelta = mouseCursorNext + mouseCursorCurr;
        }

        yaw = mouseDelta.x * sensivity;
        pitch = mouseDelta.y * sensivity;

        // pitch = std::clamp(pitch, -89.0f, 89.0f);
        // std::cout << yaw << std::endl;

        if (yaw < 0)
        {
          std::cout << "Mouse cursor position curr : (" << mouseCursorCurr.x << ";" << mouseCursorCurr.y << ")" << std::endl
                    << "Mouse cursor position next : (" << mouseCursorNext.x << ";" << mouseCursorNext.y << ")" << std::endl;
          std::cout << yaw << std::endl;
          std::cout << "Look at left" << std::endl;
          angularVelocity -= (gf::Pi / 2) * (1 + std::abs(yaw));
        }
        if (yaw > 0)
        {
          std::cout << "Mouse cursor position curr : (" << mouseCursorCurr.x << ";" << mouseCursorCurr.y << ")" << std::endl
                    << "Mouse cursor position next : (" << mouseCursorNext.x << ";" << mouseCursorNext.y << ")" << std::endl;
          std::cout << yaw << std::endl;
          angularVelocity += (gf::Pi / 2) * (1 + std::abs(yaw));
          std::cout << "Look at right" << std::endl;
        }
      }
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
    game.render();
    // game.render(true, std::make_pair(gf::Vector2i(3, 2), gf::Vector2i(3, 3)));
    //renderer.draw(trapeze, states);


    renderAWallTexturedTEST({0.0f, 0.0f}, {1.0f, 0.0f}, {{500.0f, 100.0f}, {650.0f, 50.0f}, {500.0f, 200.0f}, {650.0f, 250.0f}}, texture, renderer);

#endif
    renderer.display();
    actions.reset();
  }

#endif

  return 0;
}
