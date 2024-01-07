#include <iostream>

#include <gf/Vector.h>
#include <gf/RenderWindow.h>
#include <gf/Shapes.h>

#include "Player.hpp"
#include "MapWalls.hpp"
#include "Game2D.hpp"

#include <iomanip>

#define DELTA 0.00001
#define RENDER_BEHIND_PLAYER false

struct PairComparator
{
    bool operator()(const std::pair<gf::Vector2i, gf::Vector2i> &a, const std::pair<gf::Vector2i, gf::Vector2i> &b) const
    {
        if (a.first.x < b.first.x)
            return true;
        if (a.first.x > b.first.x)
            return false;
        if (a.first.y < b.first.y)
            return true;
        if (a.first.y > b.first.y)
            return false;
        if (a.second.x < b.second.x)
            return true;
        if (a.second.x > b.second.x)
            return false;
        return a.second.y < b.second.y;
    }
};

std::map<std::pair<gf::Vector2i, gf::Vector2i>, std::vector<gf::Vector2i>, PairComparator> segments;

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
            gf::Vector2f hitPoint = position + distX * direction;

            // is the point very close to an intersection on the grid ?
            double decX = hitPoint.x - (long)hitPoint.x;
            double decY = hitPoint.y - (long)hitPoint.y;
            if ((decX < DELTA || 1 - decX < DELTA) &&                                                     // the point X is very close to an intersection on the grid
                (decY < DELTA || 1 - decY < DELTA) &&                                                     // the point Y is very close to an intersection on the grid
                (std::abs(hitPoint.x - position.x) > DELTA || std::abs(hitPoint.y - position.y) > DELTA)) // the point is not the starting point
            {
                // is there a wall around the intersection ? -> if yes, we return the intersection
                gf::Vector2i intersection((int)(hitPoint.x + 0.5), (int)(hitPoint.y + 0.5));
                if (m_walls->getTile(intersection.x, intersection.y) != 0 ||
                    m_walls->getTile(intersection.x - 1, intersection.y) != 0 ||
                    m_walls->getTile(intersection.x, intersection.y - 1) != 0 ||
                    m_walls->getTile(intersection.x - 1, intersection.y - 1) != 0)
                {
                    return intersection;
                }
            }

            if (m_walls->getTile(tileX, tileY) != 0)
            {
                return hitPoint;
            }
            distX += unitX;
        }
        else
        {

            tileY += (vY > 0) ? 1 : -1;
            gf::Vector2f hitPoint = position + distY * direction;

            // is the point very close to an intersection on the grid ?
            double decX = hitPoint.x - (long)hitPoint.x;
            double decY = hitPoint.y - (long)hitPoint.y;
            if ((decX < DELTA || 1 - decX < DELTA) &&                                                     // the point X is very close to an intersection on the grid
                (decY < DELTA || 1 - decY < DELTA) &&                                                     // the point Y is very close to an intersection on the grid
                (std::abs(hitPoint.x - position.x) > DELTA || std::abs(hitPoint.y - position.y) > DELTA)) // the point is not the starting point
            {
                // is there a wall around the intersection ? -> if yes, we return the intersection
                gf::Vector2i intersection((int)(hitPoint.x + 0.5), (int)(hitPoint.y + 0.5));
                if (m_walls->getTile(intersection.x, intersection.y) != 0 ||
                    m_walls->getTile(intersection.x - 1, intersection.y) != 0 ||
                    m_walls->getTile(intersection.x, intersection.y - 1) != 0 ||
                    m_walls->getTile(intersection.x - 1, intersection.y - 1) != 0)
                {
                    return intersection;
                }
            }

            if (m_walls->getTile(tileX, tileY) != 0)
            {
                return hitPoint;
            }
            distY += unitY;
        }
    }
    return position + 100 * direction;
}

bool isPartIncluded(std::vector<gf::Vector2f> subSegments)
{
    // std::cout << "Subsegments vector: [";
    // for (const auto &segment : subSegments)
    // {
    //     std::cout << "(" << segment.x << ", " << segment.y << "), ";
    // }
    // std::cout << "]" << std::endl;

    for (int i = 0; i < subSegments.size() - 1; i += 2)
    {
        gf::Vector2f start1 = subSegments[i];
        gf::Vector2f end1 = subSegments[i + 1];

        for (int j = 0; j < subSegments.size() - 1; j += 2)
        {
            if (i == j)
            {
                continue;
            }
            gf::Vector2f start2 = subSegments[j];
            gf::Vector2f end2 = subSegments[j + 1];

            // std::cout << std::fixed << std::setprecision(8);
            // std::cout << "start1 : (" << start1.x << ", " << start1.y << ") end1 : (" << end1.x << ", " << end1.y << ")" << std::endl;
            // std::cout << "start2 : (" << start2.x << ", " << start2.y << ") end2 : (" << end2.x << ", " << end2.y << ")" << std::endl;
            // std::cout << std::endl;

            bool a = (start1.x <= start2.x && start2.x <= end1.x) || std::abs(start1.x - start2.x) < DELTA || std::abs(start2.x - end1.x) < DELTA;
            bool b = (start1.x >= start2.x && start2.x >= end1.x) || std::abs(start1.x - start2.x) < DELTA || std::abs(start2.x - end1.x) < DELTA;
            bool c = (start1.y <= start2.y && start2.y <= end1.y) || std::abs(start1.y - start2.y) < DELTA || std::abs(start2.y - end1.y) < DELTA;
            bool d = (start1.y >= start2.y && start2.y >= end1.y) || std::abs(start1.y - start2.y) < DELTA || std::abs(start2.y - end1.y) < DELTA;

            // std::cout << "a : " << a << std::endl;
            // std::cout << "b : " << b << std::endl;
            // std::cout << "c : " << c << std::endl;
            // std::cout << "d : " << d << std::endl;

            if ((a || b) && (c || d))
            {
                // std::cout << "start2 is included in [start1, end1]" << std::endl;
                return true;
            }
        }
    }
    return false;
}


bool isSegmentBehindPlayer(gf::Vector2f start, gf::Vector2f end, gf::Vector2f playerPosition, double playerAngle)
{
    // is the player looking upward or downward ?
    bool isLookingDown = playerAngle < gf::Pi;

    // we create the line equation perpendicular to the player's direction
    double a = -1 / std::tan(playerAngle);
    double b = playerPosition.y - a * playerPosition.x;

    // we check if the segment is behind the player
    if (isLookingDown)
    {
        // the player is looking downward
        // the segment is behind the player if the y coordinate of the segment is lower than 
        // the image of the x coordinate of the segment by the perpendicular line
        return (start.y < a * start.x + b) && (end.y < a * end.x + b);
    }
    else
    {
        // the player is looking upward
        // the segment is behind the player if the y coordinate of the segment is greater than 
        // the image of the x coordinate of the segment by the perpendicular line
        return (start.y > a * start.x + b) && (end.y > a * end.x + b);
    }
}

void Game2D::render()
{

    std::vector<Wall> walls = m_walls->getWalls();
    std::map<std::pair<gf::Vector2i, gf::Vector2i>, std::vector<gf::Vector2f>, PairComparator> segments;

    // int nbSupplementaryRays = 0;

    // render the rays :
    gf::Vector2f position = m_player->getPosition();
    std::vector<gf::Vector2i> sortedVertices = m_walls->getVertices();
    std::sort(sortedVertices.begin(), sortedVertices.end(), CompareVerticesAngle(position));

    // std::vector<gf::Vector2i> sortedVertices = wall.getSortedVertices(m_player->getPosition());

    for (int i = 0; i < sortedVertices.size(); i++)
    {
        gf::Vector2f direction = gf::normalize(gf::Vector2f(sortedVertices[i].x - position.x, sortedVertices[i].y - position.y));

        gf::Vector2f endPoint = castRay2D(position, direction, m_walls);
        double dist = std::sqrt((sortedVertices[i].x - position.x) * (sortedVertices[i].x - position.x) + (sortedVertices[i].y - position.y) * (sortedVertices[i].y - position.y));

        if (sortedVertices[i].x == endPoint.x && sortedVertices[i].y == endPoint.y)
        {
            // we hit the vertex aimed
            gf::VertexArray line(gf::PrimitiveType::Lines, 2);
            line[0].color = gf::Color::Blue;
            line[1].color = gf::Color::Blue;
            line[0].position = position * m_scaleUnit;
            line[1].position = sortedVertices[i] * m_scaleUnit;
            m_renderer.draw(line);

            std::vector<std::pair<gf::Vector2i, gf::Vector2i>> segmentsHit;
            if (m_walls->getSegments(sortedVertices[i], segmentsHit))
            {
                for (auto segment : segmentsHit)
                {
                    // std::cout << "in segment : (" << segment.first.x << ", " << segment.first.y << ") -> (" << segment.second.x << ", " << segment.second.y << ")";
                    // std::cout << " with point : (" << sortedVertices[i].x << ", " << sortedVertices[i].y << ")" << std::endl;
                    segments[segment].push_back(sortedVertices[i]);
                }
            }

            // send a new ray if needed :
            // we know that the vertex hit is a corner of a wall
            // we check how many walls are connected to this corner
            // if there is 1 wall AND we are not opposite to the wall we hit
            //       we send a new ray to the next wall

            int nbWalls = 0;
            gf::Vector2i cells[] = {
                gf::Vector2i(sortedVertices[i].x - 1, sortedVertices[i].y - 1),
                gf::Vector2i(sortedVertices[i].x - 1, sortedVertices[i].y),
                gf::Vector2i(sortedVertices[i].x, sortedVertices[i].y - 1),
                gf::Vector2i(sortedVertices[i].x, sortedVertices[i].y)};

            for (const auto &cell : cells)
            {
                if (m_walls->getTile(cell.x, cell.y) != 0)
                {
                    nbWalls++;
                }
            }

            if (nbWalls < 2)
            {
                // looking what kind of vertex we hit :
                // NW -> 0 (the vertex has the same coordinates as the cell)
                // NE -> 1 (the vertex has the same x coordinate as the cell but not the same y coordinate)
                // SE -> 2 (the vertex has neither the same x coordinate nor the same y coordinate as the cell)
                // SW -> 3 (the vertex has the same y coordinate as the cell but not the same x coordinate)
                int vertexType;
                if (sortedVertices[i].x != endPoint.x)
                {
                    vertexType = 3;
                    if (sortedVertices[i].y != endPoint.y)
                    {
                        vertexType = 2;
                    }
                }
                else
                {
                    vertexType = 0;
                    if (sortedVertices[i].y != endPoint.y)
                    {
                        vertexType = 1;
                    }
                }

                // we check if we are opposing the vertex :
                gf::Vector2f checkPosition(endPoint.x + 0.0001 * direction.x, endPoint.y + 0.0001 * direction.y);
                gf::Vector2i checkCell((int)checkPosition.x, (int)checkPosition.y);
                if (m_walls->getTile(checkCell.x, checkCell.y) == 0)
                {
                    gf::Vector2f newStartPoint(endPoint.x + 0.0001 * direction.x, endPoint.y + 0.0001 * direction.y);
                    gf::Vector2f newEndPoint = castRay2D(newStartPoint, direction, m_walls);

                    line[0].color = gf::Color::Green;
                    line[1].color = gf::Color::Green;
                    line[0].position = sortedVertices[i] * m_scaleUnit;
                    line[1].position = newEndPoint * m_scaleUnit;
                    m_renderer.draw(line);

                    if (m_walls->getSegments(newEndPoint, segmentsHit))
                    {
                        for (auto segment : segmentsHit)
                        {
                            // std::cout << "in segment : (" << segment.first.x << ", " << segment.first.y << ") -> (" << segment.second.x << ", " << segment.second.y << ")";
                            // std::cout << " with point : (" << newEndPoint.x << ", " << newEndPoint.y << ")" << std::endl;
                            segments[segment].push_back(newEndPoint);
                        }
                    }

                    // nbSupplementaryRays++;
                }
            }
        }
    }

    // std::cout << "nbSupplementaryRays : " << nbSupplementaryRays << std::endl;

    // render the map :
    // m_walls->render(m_renderer, m_scaleUnit);
    for (auto wall : walls)
    {
        wall.render(m_renderer, m_scaleUnit);
    }
    // std::cout << std::endl;

    // render the segments :
    for (auto segment : segments)
    {

        // std::map<std::pair<gf::Vector2i, gf::Vector2i>, std::vector<gf::Vector2f>, PairComparator> segments;
        // bool print = segment.first.first == gf::Vector2i(1, 5) && segment.first.second == gf::Vector2i(1, 1);
        
        // if (print)
        // {
        //     std::cout << "INCLUDED ? " << std::endl;
        // }

        //  does one part of the segment is included in another segment ?
        if (isPartIncluded(segment.second))
        {
            // if (print)
            // {
            //     std::cout << "! YES !" << std::endl;
            // }
            // we put the first point of the segment at the end of the vector
            gf::Vector2f firstPoint = segment.second[0];
            segment.second.erase(segment.second.begin());
            segment.second.push_back(firstPoint);
        }
        // else
        // {
        //     if (print)
        //     {
        //         std::cout << "no" << std::endl;
        //     }
        // }

        // draw the triangles :
        gf::VertexArray triangle(gf::PrimitiveType::Triangles, 3);
        triangle[0].color = gf::Color::fromRgba32(0x77777777);
        triangle[1].color = gf::Color::fromRgba32(0x77777777);
        triangle[2].color = gf::Color::fromRgba32(0x77777777);

        gf::VertexArray line(gf::PrimitiveType::Lines, 2);
        line[0].color = gf::Color::fromRgba32(0xFFFFAAFF);
        line[1].color = gf::Color::fromRgba32(0xFFFFAAFF);

        for (std::size_t i = 0; i < segment.second.size() - 1; i += 2)
        {
            // skip the segment if it is behind the player if needed
            if(!RENDER_BEHIND_PLAYER && isSegmentBehindPlayer(segment.second[i], segment.second[i + 1], m_player->getPosition(), m_player->getAngle()))
            {
                continue;
            }

            triangle[0].position = segment.second[i] * m_scaleUnit;
            triangle[1].position = segment.second[i + 1] * m_scaleUnit;
            triangle[2].position = m_player->getPosition() * m_scaleUnit;
            m_renderer.draw(triangle);

            line[0].position = segment.second[i] * m_scaleUnit;
            line[1].position = segment.second[i + 1] * m_scaleUnit;
            m_renderer.draw(line);

            // line[0].position = segment.second[i + 1] * m_scaleUnit;
            // line[1].position = m_player->getPosition() * m_scaleUnit;
            // m_renderer.draw(line);
        }
    }

    // render the player :
    m_player->render(m_renderer, m_scaleUnit);
}