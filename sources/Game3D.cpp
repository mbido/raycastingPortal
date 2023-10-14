#include <iostream>
#include <cmath>

#include <gf/Vector.h>
#include <gf/RenderWindow.h>
#include <gf/Shapes.h>


#include "Player.hpp"
#include "MapWalls.hpp"
#include "Game3D.hpp"


void Game3D::update(gf::Time dt) {
    m_player->update(dt);

    m_windowSize = m_renderer.getSize();
    m_scaleUnit = std::min((int)(m_windowSize[0] / m_walls->getNbRows()), (int)(m_windowSize[1] / m_walls->getNbColumns()));
}

gf::Vector2f castRay3D(gf::Vector2f position, gf::Vector2f direction, MapWalls *m_walls) {

    // the tiles position we want to check if it's a wall or not
    int tileX = (int) position[0];
    int tileY = (int) position[1];

    // the direction of the ray
    double vX = direction[0];
    double vY = direction[1];

    // the distance traveled along the direction when offsetting the x or y axis by one
    double unitX = std::sqrt(1 + (vY / vX) * (vY / vX));
    double unitY = std::sqrt(1 + (vX / vY) * (vX / vY));    

    // the distance traveled when following the x or y axis
    double distX;
    double distY;

    if(direction[0] < 0) {
        // looking left
        distX = (position[0] - (double)tileX) * unitX;
    }else {
        // looking right
        distX = ((double)(tileX + 1) - position[0]) * unitX;
    }
    if(direction[1] < 0) {
        // looking up
        distY = (position[1] - (double)tileY) * unitY;
    }else {
        // looking down
        distY = ((double)(tileY + 1) - position[1]) * unitY;
    }

    for(int i = 0; i < 100; i++) {
        if(tileX < 0 || tileX >= m_walls->getNbColumns() || tileY < 0 || tileY >= m_walls->getNbRows()) {
            // we are outside of the map
            return position + 100 * direction;
        }
        if(distX <= distY) {
            tileX += (vX > 0)? 1 : -1;
            if (m_walls->getTile(tileX, tileY) == 1) {
                return position + distX * direction;
            }
            distX += unitX;
        } else {
            tileY += (vY > 0)? 1 : -1;
            if (m_walls->getTile(tileX, tileY) == 1) {
                return position + distY * direction;
            }
            distY += unitY;
        }
    }

    return position + 100 * direction;
}



void Game3D::render() {

    int nbRays = m_renderer.getSize()[0];
    double fov = 60 * gf::Pi / 180;
    double angle = m_player->getAngle() - fov / 2;
    for(int i = 0 ; i < nbRays ; i++) {
        angle = std::fmod(angle, 2 * gf::Pi);
        if (angle < 0) {
            angle += 2 * gf::Pi;
        }
        gf::Vector2f direction = gf::normalize(gf::Vector2f(std::cos(angle), std::sin(angle)));

        // render the ray :
        gf::Vector2f position = m_player->getPosition();
        //gf::Vector2f endPoint = castRay(position, angle, m_walls);

        gf::Vector2f endPoint = castRay3D(position, direction, m_walls);

        // the distance of the wall to the player : 
        double falseDistance = std::sqrt((endPoint[0] - position[0]) * (endPoint[0] - position[0]) 
                                + (endPoint[1] - position[1]) * (endPoint[1] - position[1]));
        // the real distance is the projection of the ray on the direction vector :
        double realDistance = falseDistance * std::cos(angle - m_player->getAngle());

        // the height of the projected wall :
        // Projected Slice Height = realSlideHeight / Distance to the Slice * Distance to the Projection Plane * scaling
        double height = (1 / realDistance) * 277 * m_scaleUnit / 32;
        

        gf::v1::Color4f color = gf::Color::fromRgba32(255, 50, 50);
        // defining gradients and colors of the walls 
        if(endPoint[0] == (int)endPoint[0]) {
            color = gf::Color::fromRgba32(50, 50, 255);
        }
        

        // putting a pixel wide rectangle at i-th pixel of the screen :
        gf::RectangleShape wall(gf::Vector2f(1, height));
        wall.setPosition(gf::Vector2f(i, (m_windowSize[1] - height) / 2));
        wall.setColor(color);
        m_renderer.draw(wall);

        angle += fov / nbRays;   
    }
}