#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <gf/Entity.h>
#include <gf/Vector.h>
#include <gf/RenderTarget.h>

#include "MapWalls.hpp"

class Player : public gf::Entity
{
public:
    Player(gf::Vector2f position, double angle = gf::Pi, int speed = 1, gf::Vector2f velocity = gf::Vector2f(0.0f, 0.0f))
        : Entity(), m_position(position), m_velocity(velocity), m_speed(speed), m_angle(angle), m_angularVelocity(0.0f)
    {
        gf::Vector2f direction = gf::normalize(gf::Vector2f(std::cos(angle), std::sin(angle)));
        gf::Vector2f perpendicular = gf::normalize(gf::Vector2f(-direction.y, direction.x));
        m_projectionPlane = perpendicular * 0.66;
    }
    void setVelocity(const gf::Vector2f &velocity);
    void setAngularVelocity(double delta);
    void setPosition(const gf::Vector2f &position);
    const gf::Vector2f getPosition();
    void setAngle(double angle);
    double getAngle();
    void update(gf::Time dt);
    void render(gf::RenderTarget &renderer, int sizeUnit);

private:
    gf::Vector2f m_position;
    gf::Vector2f m_velocity;
    gf::Vector2f m_projectionPlane;
    int m_speed;
    double m_angle;
    double m_angularVelocity;
};

#endif