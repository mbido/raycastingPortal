#include <iostream>

#include <gf/Vector.h>
#include <gf/RenderWindow.h>
#include <gf/Shapes.h>

#include "Player.hpp"
#include "MapWalls.hpp"
#include "Game2D.hpp"

#define DELTA 0.00001

struct PairComparator {
    bool operator() (const std::pair<gf::Vector<int, 2>, gf::Vector<int, 2>>& a, const std::pair<gf::Vector<int, 2>, gf::Vector<int, 2>>& b) const {
        if (a.first.x < b.first.x) return true;
        if (a.first.x > b.first.x) return false;
        if (a.first.y < b.first.y) return true;
        if (a.first.y > b.first.y) return false;
        if (a.second.x < b.second.x) return true;
        if (a.second.x > b.second.x) return false;
        return a.second.y < b.second.y;
    }
};

std::map<std::pair<gf::Vector<int, 2>, gf::Vector<int, 2>>, std::vector<gf::Vector<int, 2>>, PairComparator> segments;

void Game2D::update(gf::Time dt)
{
    m_player->update(dt);

    m_windowSize = m_renderer.getSize();
    m_scaleUnit = std::min((int)(m_windowSize[0] / m_walls->getNbRows()), (int)(m_windowSize[1] / m_walls->getNbColumns()));
}

gf::Vector2f castRay2D(gf::Vector2f position, gf::Vector2f direction, MapWalls *m_walls)
{

    // the tiles position we want to check if it's a wall or not
    int tileX = (int)position[0];
    int tileY = (int)position[1];

    // the direction of the ray
    double vX = direction[0];
    double vY = direction[1];

    // the distance traveled along the direction when offsetting the x or y axis by one
    double unitX = std::sqrt(1 + (vY / vX) * (vY / vX));
    double unitY = std::sqrt(1 + (vX / vY) * (vX / vY));

    // the distance traveled when following the x or y axis
    double distX;
    double distY;

    if (direction[0] < 0)
    {
        // looking left
        distX = (position[0] - (double)tileX) * unitX;
    }
    else
    {
        // looking right
        distX = ((double)(tileX + 1) - position[0]) * unitX;
    }
    if (direction[1] < 0)
    {
        // looking up
        distY = (position[1] - (double)tileY) * unitY;
    }
    else
    {
        // looking down
        distY = ((double)(tileY + 1) - position[1]) * unitY;
    }

    for (int i = 0; i < 100; i++)
    {
        if (distX <= distY)
        {
            tileX += (vX > 0) ? 1 : -1;
            if (m_walls->getTile(tileX, tileY) == 1)
            {
                return position + distX * direction;
            }
            distX += unitX;
        }
        else
        {
            tileY += (vY > 0) ? 1 : -1;
            if (m_walls->getTile(tileX, tileY) == 1)
            {
                return position + distY * direction;
            }
            distY += unitY;
        }
    }
    return position + 100 * direction;
}

void Game2D::render()
{

    std::vector<Wall> walls = m_walls->getWalls();
    std::map<std::pair<gf::Vector2i, gf::Vector2i>, std::vector<gf::Vector2f>, PairComparator> segments;

    // render the rays :
    for (auto wall : walls)
    {
        std::vector<gf::Vector2i> vertices = wall.getSortedVertices(m_player->getPosition());
        for (int i = 0; i < vertices.size(); i++)
        {

            gf::Vector2f position = m_player->getPosition();
            gf::Vector2f direction = gf::normalize(gf::Vector2f(vertices[i].x - position.x, vertices[i].y - position.y));

            gf::Rotation rotator1(DELTA);
            gf::Vector2f direction1 = gf::transform(rotator1, direction);
            gf::Vector2f endPoint1 = castRay2D(position, direction1, m_walls);

            gf::Rotation rotator2(-DELTA);
            gf::Vector2f direction2 = gf::transform(rotator2, direction);
            gf::Vector2f endPoint2 = castRay2D(position, direction2, m_walls);

            double dist1 = std::sqrt((endPoint1.x - position.x) * (endPoint1.x - position.x) + (endPoint1.y - position.y) * (endPoint1.y - position.y));
            double dist2 = std::sqrt((endPoint2.x - position.x) * (endPoint2.x - position.x) + (endPoint2.y - position.y) * (endPoint2.y - position.y));

            bool cond1 = std::abs(vertices[i].x - endPoint1.x) < 10 * DELTA * dist1 && std::abs(vertices[i].y - endPoint1.y) < 10 * DELTA * dist1;
            bool cond2 = std::abs(vertices[i].x - endPoint2.x) < 10 * DELTA * dist2 && std::abs(vertices[i].y - endPoint2.y) < 10 * DELTA * dist2;

            if (cond1 || cond2)
            {
                gf::VertexArray line(gf::PrimitiveType::Lines, 2);
                line[0].color = gf::Color::Blue;
                line[1].color = gf::Color::Blue;
                line[0].position = position * m_scaleUnit;
                line[1].position = endPoint1 * m_scaleUnit;
                m_renderer.draw(line);

                std::pair<gf::Vector2i, gf::Vector2i> segment;
                if (m_walls->getSegment(endPoint1, segment))
                {
                    segments[segment].push_back(endPoint1);
                }

                
                line[0].position = position * m_scaleUnit;
                line[1].position = endPoint2 * m_scaleUnit;
                m_renderer.draw(line);

                if (m_walls->getSegment(endPoint2, segment))
                {
                    segments[segment].push_back(endPoint2);
                }
            }
        }
    }

    // render the segments :
    for (auto segment : segments)
    {
        // draw the triangles :
        gf::VertexArray triangle(gf::PrimitiveType::Triangles, 3);
        triangle[0].color = gf::Color::Yellow;
        triangle[1].color = gf::Color::Yellow;

        gf::VertexArray line(gf::PrimitiveType::Lines, 2);
        line[0].color = gf::Color::Blue;
        line[1].color = gf::Color::Blue;

        for (std::size_t i = 0; i < segment.second.size() - 1; i += 2)
        {
            triangle[0].position = segment.second[i] * m_scaleUnit;
            triangle[1].position = segment.second[i + 1] * m_scaleUnit;
            triangle[2].position = m_player->getPosition() * m_scaleUnit;
            m_renderer.draw(triangle);

            line[0].position = segment.second[i] * m_scaleUnit;
            line[1].position = segment.second[i + 1] * m_scaleUnit;
            m_renderer.draw(line);

            line[0].position = segment.second[i + 1] * m_scaleUnit;
            line[1].position = m_player->getPosition() * m_scaleUnit;
            m_renderer.draw(line);
        }
    }

    // render the map :
    // m_walls->render(m_renderer, m_scaleUnit);
    for (auto wall : walls)
    {
        wall.render(m_renderer, m_scaleUnit);
    }

    // render the player :
    m_player->render(m_renderer, m_scaleUnit);
}