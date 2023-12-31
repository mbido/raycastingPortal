#include <iostream>
#include <cmath>

#include <gf/Vector.h>
#include <gf/RenderWindow.h>
#include <gf/Shapes.h>


#include "Player.hpp"
#include "MapWalls.hpp"
#include "Game3D.hpp"

#define DELTA 0.00001

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

bool Game3D::isPartIncluded(std::vector<gf::Vector2f> subSegments)
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

void Game3D::update(gf::Time dt) {
    m_player->update(dt);

    m_windowSize = m_renderer.getSize();
    m_scaleUnit = std::min((int)(m_windowSize[0] / m_walls->getNbRows()), (int)(m_windowSize[1] / m_walls->getNbColumns()));
}

gf::Vector2f castRay(gf::Vector2f position, gf::Vector2f direction, MapWalls *m_walls)
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

/**
 * \brief Determines if a segment is visible to the player.
 *
 * This function checks if a segment defined by two points, start and end, is visible to the player.
 * The visibility is determined based on the player's position, player's angle, and the equation of a line (a * x + b).
 * The segment is considered to have a visible part and an invisible part.
 *
 * \param start The starting point of the segment. -> modifiable
 * \param end The ending point of the segment. -> modifiable
 * \param playerPosition The position of the player.
 * \param playerAngle The angle at which the player is looking.
 * \param a The coefficient 'a' in the equation of the line (a * x + b).
 * \param b The coefficient 'b' in the equation of the line (a * x + b).
 *
 * \return True if the segment is visible to the player, false otherwise.
 */
bool Game3D::getVisibleSegment(gf::Vector2f &start, gf::Vector2f &end, gf::Vector2f playerPosition, double playerAngle, double a, double b)
{
    // is the player looking upward or downward ?
    bool isLookingDown = playerAngle < gf::Pi;

    // we consider that the segment has a part visible and a part invisible
    gf::Vector2f visiblePoint;
    gf::Vector2f invisiblePoint;
    gf::Vector2f intersectionPoint;

    // is the segment vertical ?
    bool isVertical = std::abs(start.x - end.x) < DELTA;

    if (isLookingDown)
    {
        if ((start.y < a * start.x + b) && (end.y < a * end.x + b))
        {
            // the segment is completely invisible (behind the player)
            return false;
        }
        else if ((start.y > a * start.x + b) && (end.y > a * end.x + b))
        {
            // the segment is completely visible
            return true;
        }
        else if (start.y > a * start.x + b)
        {
            // the start point is visible and the end point is invisible
            visiblePoint = start;
            invisiblePoint = end;
        }
        else
        {
            // the start point is invisible and the end point is visible
            visiblePoint = end;
            invisiblePoint = start;
        }
    }
    else
    {
        if ((start.y > a * start.x + b) && (end.y > a * end.x + b))
        {
            // the segment is completely invisible (behind the player)
            return false;
        }
        else if ((start.y < a * start.x + b) && (end.y < a * end.x + b))
        {
            // the segment is completely visible
            return true;
        }
        else if (start.y < a * start.x + b)
        {
            // the start point is visible and the end point is invisible
            visiblePoint = start;
            invisiblePoint = end;
        }
        else
        {
            // the start point is invisible and the end point is visible
            visiblePoint = end;
            invisiblePoint = start;
        }
    }
    if (isVertical)
    {
        intersectionPoint = gf::Vector2f(start.x, a * start.x + b);
        if (isLookingDown)
        {
            intersectionPoint.y += DELTA;
        }
        else
        {
            intersectionPoint.y -= DELTA;
        }
    }
    else
    {
        if (a == 0) // !!! have to check if even possible
            a = DELTA/2;
        intersectionPoint = gf::Vector2f((start.y - b) / a, start.y);
        if (isLookingDown)
        {
            intersectionPoint.x += DELTA;
        }
        else
        {
            intersectionPoint.x -= DELTA;
        }
    }
    start = visiblePoint;
    end = intersectionPoint;
    return true;
}

// void Game3D::render() {
//     int nbRays = int(m_renderer.getSize()[0]);
//     double fov = 60 * gf::Pi / 180;
//     double angle = m_player->getAngle() - fov / 2;
//     for(int i = 0 ; i < nbRays ; i++) {
//         angle = std::fmod(angle, 2 * gf::Pi);
//         if (angle < 0) {
//             angle += 2 * gf::Pi;
//         }
//         gf::Vector2f direction = gf::normalize(gf::Vector2f(std::cos(angle), std::sin(angle)));

//         // render the ray :
//         gf::Vector2f position = m_player->getPosition();
//         //gf::Vector2f endPoint = castRay(position, angle, m_walls);


//         // ---CASTING THE RAY---
//         struct castResult cast = castRay(position, direction, m_walls);
//         gf::Vector2f endPoint = cast.endPoint;

//         // the distance of the wall to the player : 
//         double falseDistance = std::sqrt((endPoint[0] - position[0]) * (endPoint[0] - position[0]) 
//                                 + (endPoint[1] - position[1]) * (endPoint[1] - position[1]));
//         // the real distance is the projection of the ray on the direction vector :
//         double realDistance = falseDistance * std::cos(angle - m_player->getAngle());

//         // the height of the projected wall :
//         // Projected Slice Height = realSlideHeight / Distance to the Slice * Distance to the Projection Plane * scaling
//         double height = (realDistance != 0)? m_windowSize[0] / realDistance : m_windowSize[1] * 2;
        
//         // defining gradients and colors of the walls :
//         int gradient = (realDistance * 15 > 255 - 20) ? 20 : 255 - realDistance * 15;

//         gf::v1::Color4f color = gf::Color::fromRgba32(gradient, int(gradient / 8), int(gradient / 4));
//         if(cast.tileSideHit == 0) {
//             color = gf::Color::fromRgba32(int(gradient / 8), gradient, int(gradient / 4));
//         }else if(cast.tileSideHit == 1) {
//             color = gf::Color::fromRgba32(int(gradient / 4), int(gradient / 8), gradient);
//         }else if(cast.tileSideHit == 2) {
//             color = gf::Color::fromRgba32(gradient, int(gradient / 4), gradient);
//         }
        

//         // putting a pixel wide rectangle at i-th pixel of the screen :
//         gf::RectangleShape wall(gf::Vector2f(1, height));
//         wall.setPosition(gf::Vector2f(i, (m_windowSize[1] - height) / 2));
//         wall.setColor(color);
//         m_renderer.draw(wall);

//         angle += fov / nbRays;   
//     }
// }



void Game3D::render()
{
    std::cout << std::endl;
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

        gf::Vector2f endPoint = castRay(position, direction, m_walls);
        double dist = std::sqrt((sortedVertices[i].x - position.x) * (sortedVertices[i].x - position.x) + (sortedVertices[i].y - position.y) * (sortedVertices[i].y - position.y));

        if (sortedVertices[i].x == endPoint.x && sortedVertices[i].y == endPoint.y)
        {
            // we hit the vertex aimed

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
                    gf::Vector2f newEndPoint = castRay(newStartPoint, direction, m_walls);

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


    // render the segments :
    for (auto segment : segments)
    {
        //  does one part of the segment is included in another segment ?
        if (isPartIncluded(segment.second))
        {
            // we put the first point of the segment at the end of the vector
            gf::Vector2f firstPoint = segment.second[0];
            segment.second.erase(segment.second.begin());
            segment.second.push_back(firstPoint);
        }

        // draw the triangles :
        // gf::VertexArray triangle(gf::PrimitiveType::Triangles, 3);
        // triangle[0].color = gf::Color::fromRgba32(0x77777777);
        // triangle[1].color = gf::Color::fromRgba32(0x77777777);
        // triangle[2].color = gf::Color::fromRgba32(0x77777777);

        // gf::VertexArray line(gf::PrimitiveType::Lines, 2);
        // line[0].color = gf::Color::fromRgba32(0xFFFFAAFF);
        // line[1].color = gf::Color::fromRgba32(0xFFFFAAFF);

        for (std::size_t i = 0; i < segment.second.size() - 1; i += 2)
        {
            gf::Vector2f start = segment.second[i];
            gf::Vector2f end = segment.second[i + 1];
            double angle = m_player->getAngle();
            if (angle == 0 || angle == gf::Pi || angle == 2 * gf::Pi || angle == -gf::Pi)
            {
                angle += DELTA/2;
            }
            gf::Vector2f pos = m_player->getPosition();
            double a = -1 / std::tan(angle);
            double b = pos.y - a * pos.x;
            // skip the segment if it is behind the player if needed
            if (!getVisibleSegment(start, end, pos, angle, a, b))
            {
                continue;
            }

            std::cout << "start : (" << start.x << ", " << start.y << ") end : (" << end.x << ", " << end.y << ")" << std::endl;
            // --- For the first point of the segment ---

            // the angle of the ray to the player:
            double rayAngle1 = std::atan2(start.y - pos.y, start.x - pos.x);

            // the distance of the wall to the player :
            double falseDistance1 = std::sqrt((start.x - pos.x) * (start.x - pos.x) + (start.y - pos.y) * (start.y - pos.y));
            // the real distance is the projection of the ray on the direction vector :
            double realDistance1 = falseDistance1 * std::cos(rayAngle1 - angle);

            // the height of the projected wall :
            // Projected Slice Height = realSlideHeight / Distance to the Slice * Distance to the Projection Plane * scaling
            double height1 = (realDistance1 != 0) ? 100.0f / realDistance1 : 200.0f;

            // the x position of the column from the center of the screen :
            double xPos1 = std::tan(rayAngle1 - angle) * 100.0f / 2;
            // if (xPos1 > 100.0f)
            // {
            //     xPos1 = 100.0f;
            // }
            // else if (xPos1 < -100.0f)
            // {
            //     xPos1 = -100.0f;
            // }

            // drawing the column :
            gf::VertexArray column1(gf::PrimitiveType::Lines, 2);
            column1[0].color = gf::Color::fromRgba32(0x7777FFFF);
            column1[1].color = gf::Color::fromRgba32(0x7777FFFF);

            column1[0].position = gf::Vector2f(50.0f + xPos1, 50.0f - height1 / 2);
            column1[1].position = gf::Vector2f(50.0f + xPos1, 50.0f + height1 / 2);
            
            m_renderer.draw(column1);

            // --- For the second point of the segment ---

            // the angle of the ray to the player:
            double rayAngle2 = std::atan2(end.y - pos.y, end.x - pos.x);

            // the distance of the wall to the player :
            double falseDistance2 = std::sqrt((end.x - pos.x) * (end.x - pos.x) + (end.y - pos.y) * (end.y - pos.y));
            // the real distance is the projection of the ray on the direction vector :
            double realDistance2 = falseDistance2 * std::cos(rayAngle2 - angle);

            // the height of the projected wall :
            // Projected Slice Height = realSlideHeight / Distance to the Slice * Distance to the Projection Plane * scaling
            double height2 = (realDistance2 != 0) ? 100.0f / realDistance2 : 200.0f;

            // the x position of the column from the center of the screen :
            double xPos2 = std::tan(rayAngle2 - angle) * 50.0f;
            // if (xPos2 > 100.0f)
            // {
            //     xPos2 = 100.0f;
            // }
            // else if (xPos2 < -100.0f)
            // {
            //     xPos2 = -100.0f;
            // }

            // drawing the column :
            gf::VertexArray column2(gf::PrimitiveType::Lines, 2);
            column2[0].color = gf::Color::fromRgba32(0x77FF77FF);
            column2[1].color = gf::Color::fromRgba32(0x77FF77FF);

            column2[0].position = gf::Vector2f(50.0f + xPos2, 50.0f - height2 / 2);
            column2[1].position = gf::Vector2f(50.0f + xPos2, 50.0f + height2 / 2);
            m_renderer.draw(column2);

            // linking the two columns :
            gf::VertexArray link(gf::PrimitiveType::Lines, 2);
            link[0].color = gf::Color::fromRgba32(0xFF7777FF);
            link[1].color = gf::Color::fromRgba32(0xFF7777FF);

            link[0].position = column1[1].position;
            link[1].position = column2[1].position;
            m_renderer.draw(link);

            link[0].position = column1[0].position;
            link[1].position = column2[0].position;
            m_renderer.draw(link);


            // // --- For the triangles ---
            // gf::VertexArray triangle(gf::PrimitiveType::Triangles, 3);
            // triangle[0].color = gf::Color::fromRgba32(0x777777FF);
            // triangle[1].color = gf::Color::fromRgba32(0x777777FF);
            // triangle[2].color = gf::Color::fromRgba32(0x777777FF);

            // // we draw the trapezoid with two triangles :

            // triangle[0].position = column1[0].position;
            // triangle[1].position = column1[1].position;
            // triangle[2].position = column2[0].position;
            // m_renderer.draw(triangle);

        }
    }

}