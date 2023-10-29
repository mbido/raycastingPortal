#include "Player.hpp"

#include <gf/Vector.h>
#include <gf/Shapes.h>
#include <gf/Geometry.h> 
#include <gf/Transform.h>

#include <cstdio>

void Player::setVelocity(const gf::Vector2f &velocity) {
    m_velocity = velocity;
}

void Player::setAngularVelocity(double delta) {
    m_angularVelocity = delta;
}

double Player::getAngle() {
    return m_angle;
}


const gf::Vector2f Player::getPosition(){
    return m_position;
}

void Player::update(gf::Time dt){
    m_angle += m_angularVelocity * dt.asSeconds();

    gf::Rotation rotator(m_angle + gf::Pi / 2);
    gf::Vector2f newVelocity = gf::transform(rotator, m_velocity);

    if (newVelocity != gf::vec(0.0f, 0.0f)) {
      newVelocity = gf::normalize(newVelocity);
    }

    m_position += newVelocity * dt.asSeconds() * m_speed;
    m_angle = std::fmod(m_angle, 2 * gf::Pi);

    if (m_angle < 0) {
        m_angle += 2 * gf::Pi;
    }

    gf::Vector2f direction = gf::normalize(gf::Vector2f(std::cos(m_angle), std::sin(m_angle)));
    gf::Vector2f perpendicular = gf::normalize(gf::Vector2f(-direction.y, direction.x));
    m_projectionPlane = perpendicular * 0.66;
}

void Player::render(gf::RenderWindow& renderer, int sizeUnit){

    // the direction is represented by a red ligne :
    double lineLength = 2.0f; // longueur fixe de la ligne
    gf::Vector2f direction(std::cos(m_angle), std::sin(m_angle));
    gf::Vector2f endPoint = m_position + lineLength * gf::normalize(direction);
    gf::VertexArray line(gf::PrimitiveType::Lines, 2);
    line[0].position = m_position * sizeUnit;
    line[1].position = endPoint * sizeUnit;
    line[0].color = gf::Color::Red;
    line[1].color = gf::Color::Red;
    renderer.draw(line);

    // the player is represented by a small circle :
    gf::CircleShape circle(sizeUnit / 16);
    circle.setPosition(m_position * sizeUnit);
    circle.setColor(gf::Color::Blue);
    circle.setAnchor(gf::Anchor::Center);
    renderer.draw(circle);
}
