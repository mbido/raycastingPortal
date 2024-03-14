cmake_minimum_required(VERSION 3.10)

project(raycastingPortal
  VERSION 0.1.0
)

find_package(gf REQUIRED)

add_executable(mygame
  Main.cpp
  Player.cpp
  MapTemplate.cpp
  MapWalls.cpp
  Game2D.cpp
  Game3D.cpp
  Wall.cpp
  CompareVerticesAngle.cpp
  GameHub.cpp
  MenuScene.cpp
  SelectLevelScene.cpp
  LevelScene.cpp
  LevelEndScene.cpp
)

target_link_libraries(mygame
  PRIVATE 
    gf::graphics 
    gf::net
)
