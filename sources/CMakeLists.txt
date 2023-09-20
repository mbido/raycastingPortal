cmake_minimum_required(VERSION 3.10)

project(raycastingPortal
  VERSION 0.1.0
)

find_package(gf REQUIRED)

add_executable(mygame
  main.cpp
  # and the rest of your sources
)

target_link_libraries(mygame 
  PRIVATE 
    gf::graphics 
    gf::net
)