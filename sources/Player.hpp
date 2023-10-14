#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <gf/Entity.h>
#include <gf/Vector.h>
#include <gf/RenderWindow.h>

#include "MapWalls.hpp"

class Player : public gf::Entity {
    public :
        Player(gf::Vector2f position, double angle = gf::Pi, int speed = 1, gf::Vector2f velocity = gf::Vector2f(0.0f, 0.0f))
        : Entity()
        , m_position(position)
        , m_velocity(velocity)
        , m_speed(speed)
        , m_angle(angle)
        , m_angularVelocity(0.0f)
        {
        }
        void setVelocity(const gf::Vector2f& velocity);
        void setAngularVelocity(double delta);
        double getAngle();
        const gf::Vector2f getPosition();
        void update(gf::Time dt);
        void render(gf::RenderWindow& renderer, int sizeUnit);
        gf::Vector2f castRay(gf::Vector2f position, double angle);
    private :
        gf::Vector2f m_position;
        gf::Vector2f m_velocity;
        int m_speed;
        double m_angle;
        double m_angularVelocity;
};

#endif