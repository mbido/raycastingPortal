#include <iostream>
#include <cmath>

#include <gf/Vector.h>
#include <gf/RenderWindow.h>
#include <gf/Shapes.h>

#include "Player.hpp"
#include "MapWalls.hpp"
#include "Game3D.hpp"

#define DELTA 0.00001
#define RANGE 0.1

gf::Vector2f linesIntersection(gf::Vector2f A, gf::Vector2f B, gf::Vector2f C, gf::Vector2f D)
{
    float a1 = B.y - A.y;
    float b1 = A.x - B.x;
    float c1 = a1 * A.x + b1 * A.y;

    float a2 = D.y - C.y;
    float b2 = C.x - D.x;
    float c2 = a2 * C.x + b2 * C.y;

    float delta = a1 * b2 - a2 * b1;

    if (std::abs(delta) < DELTA)
    {
        // std::cerr << "The lines are parallel" << std::endl;
        return gf::Vector2f(0, 0);
    }
    else
    {
        return gf::Vector2f((b2 * c1 - b1 * c2) / delta, (a1 * c2 - a2 * c1) / delta);
    }
}

// clamp function used for collisions
float clamp3D(float min, float max, float value)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

gf::Vector2f checkingCollisionsForEachWall3D(gf::Vector2f closestPointOfTheWall, gf::Vector2f p_position)
{
    float distanceBetweenWallAndPlayer = std::sqrt((p_position.x - closestPointOfTheWall.x) * (p_position.x - closestPointOfTheWall.x) + (p_position.y - closestPointOfTheWall.y) * (p_position.y - closestPointOfTheWall.y));
    if (RANGE > distanceBetweenWallAndPlayer)
    {
        if (distanceBetweenWallAndPlayer == 0)
            return gf::Vector2f(0.0f, 0.0f);
        gf::Vector2f coordToAddToPos = gf::Vector2f(p_position.x - closestPointOfTheWall.x, p_position.y - closestPointOfTheWall.y);
        coordToAddToPos = (coordToAddToPos / distanceBetweenWallAndPlayer) * (RANGE - distanceBetweenWallAndPlayer);
        return coordToAddToPos;
    }
    return gf::Vector2f(0.0f, 0.0f);
}

std::pair<double, gf::Vector2f> checkingCollisionsForEachPortal3D(struct portal *firstPortal, struct portal *secondPortal, gf::Vector2f closestPointOfThePortal, gf::Vector2f p_position, double angle)
{
    float distanceBetweenPortalAndPlayer = std::sqrt((p_position.x - closestPointOfThePortal.x) * (p_position.x - closestPointOfThePortal.x) + (p_position.y - closestPointOfThePortal.y) * (p_position.y - closestPointOfThePortal.y));
    if (RANGE + DELTA > distanceBetweenPortalAndPlayer)
    {

        std::pair<double, gf::Vector2f> nextCoords = std::pair(angle, p_position);

        nextCoords.first = angle + gf::Pi + (secondPortal->facing - firstPortal->facing) * gf::Pi / 2;

        switch ((firstPortal->facing + secondPortal->facing) % 4)
        {
        case 0:
            nextCoords.second = gf::Vector2f(secondPortal->position.x + (p_position.x - firstPortal->position.x), secondPortal->position.y - (p_position.y - firstPortal->position.y));
            break;
        case 1:
            nextCoords.second = gf::Vector2f(secondPortal->position.x + (p_position.y - firstPortal->position.y), secondPortal->position.y + (p_position.x - firstPortal->position.x));
            break;
        case 2:
            nextCoords.second = gf::Vector2f(secondPortal->position.x - (p_position.x - firstPortal->position.x), secondPortal->position.y + (p_position.y - firstPortal->position.y));
            break;
        case 3:
            nextCoords.second = gf::Vector2f(secondPortal->position.x - (p_position.y - firstPortal->position.y), secondPortal->position.y - (p_position.x - firstPortal->position.x));
            break;
        default:
            nextCoords.second = p_position;
            break;
        }

        return nextCoords;
    }
    return std::pair(angle, p_position);
}

gf::Vector2f getClosestPointOfWall3D(Wall wall, gf::Vector2f p_position)
{
    gf::Vector2f closestPointOfTheWall = gf::Vector2f(0.0f, 0.0f);

    std::vector<gf::Vector2i> blocs = wall.getWallCells();

    for (auto bloc : blocs)
    {
        float x = clamp3D((float)bloc.x, (float)bloc.x + 1, p_position.x);
        float y = clamp3D((float)bloc.y, (float)bloc.y + 1, p_position.y);
        float distPlayerClampedPos = (p_position.x - x) * (p_position.x - x) + (p_position.y - y) * (p_position.y - y);
        float distPlayerCurrClosestPos = (p_position.x - closestPointOfTheWall.x) * (p_position.x - closestPointOfTheWall.x) + (p_position.y - closestPointOfTheWall.y) * (p_position.y - closestPointOfTheWall.y);

        if (distPlayerClampedPos <= distPlayerCurrClosestPos)
        {
            closestPointOfTheWall = gf::Vector2f(x, y);
        }
    }
    return closestPointOfTheWall;
}

std::pair<bool, gf::Vector2f> getClosestPointOfPortal3D(struct portal *firstPortal, struct portal *secondPortal, gf::Vector2f p_position)
{
    gf::Vector2f closestPointOfThePortal = gf::Vector2f(0.0f, 0.0f);
    bool isFirstPortal = true;

    // First Portal

    float x, y;

    switch (firstPortal->facing)
    {
    case 0:
        x = clamp3D(firstPortal->position.x, firstPortal->position.x, p_position.x);
        y = clamp3D(firstPortal->position.y - firstPortal->width, firstPortal->position.y + firstPortal->width, p_position.y);
        break;
    case 1:
        x = clamp3D(firstPortal->position.x - firstPortal->width, firstPortal->position.x + firstPortal->width, p_position.x);
        y = clamp3D(firstPortal->position.y, firstPortal->position.y, p_position.y);
        break;
    case 2:
        x = clamp3D(firstPortal->position.x, firstPortal->position.x, p_position.x);
        y = clamp3D(firstPortal->position.y - firstPortal->width, firstPortal->position.y + firstPortal->width, p_position.y);
        break;
    case 3:
        x = clamp3D(firstPortal->position.x - firstPortal->width, firstPortal->position.x + firstPortal->width, p_position.x);
        y = clamp3D(firstPortal->position.y, firstPortal->position.y, p_position.y);
        break;
    default:
        x = p_position.x;
        y = p_position.y;
        break;
    }

    closestPointOfThePortal = gf::Vector2f(x, y);

    // Second Portal

    switch (secondPortal->facing)
    {
    case 0:
        x = clamp3D(secondPortal->position.x, secondPortal->position.x, p_position.x);
        y = clamp3D(secondPortal->position.y - secondPortal->width, secondPortal->position.y + secondPortal->width, p_position.y);
        break;
    case 1:
        x = clamp3D(secondPortal->position.x - secondPortal->width, secondPortal->position.x + secondPortal->width, p_position.x);
        y = clamp3D(secondPortal->position.y, secondPortal->position.y, p_position.y);
        break;
    case 2:
        x = clamp3D(secondPortal->position.x, secondPortal->position.x, p_position.x);
        y = clamp3D(secondPortal->position.y - secondPortal->width, secondPortal->position.y + secondPortal->width, p_position.y);
        break;
    case 3:
        x = clamp3D(secondPortal->position.x - secondPortal->width, secondPortal->position.x + secondPortal->width, p_position.x);
        y = clamp3D(secondPortal->position.y, secondPortal->position.y, p_position.y);
        break;
    default:
        x = p_position.x;
        y = p_position.y;
        break;
    }

    if ((p_position.x - x) * (p_position.x - x) + (p_position.y - y) * (p_position.y - y) < (p_position.x - closestPointOfThePortal.x) * (p_position.x - closestPointOfThePortal.x) + (p_position.y - closestPointOfThePortal.y) * (p_position.y - closestPointOfThePortal.y))
    {
        closestPointOfThePortal = gf::Vector2f(x, y);
        isFirstPortal = false;
    }

    return std::pair(isFirstPortal, closestPointOfThePortal);
}

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

void Game3D::update(gf::Time dt)
{
    m_player->update(dt);

    m_windowSize = m_renderer.getSize();
    m_scaleUnit = std::min((int)(m_windowSize[0] / m_walls->getNbRows()), (int)(m_windowSize[1] / m_walls->getNbColumns()));

    for (auto wall : m_walls->getWalls())
    {
        gf::Vector2f newPos = checkingCollisionsForEachWall3D(getClosestPointOfWall3D(wall, m_player->getPosition()), m_player->getPosition());
        m_player->setPosition(m_player->getPosition() + newPos);
    }

    if (m_firstPortal != NULL && m_secondPortal != NULL)
    {
        std::pair<bool, gf::Vector2f> tmp = getClosestPointOfPortal3D(m_firstPortal, m_secondPortal, m_player->getPosition());
        std::pair<double, gf::Vector2f> positionPostTP = std::pair(m_player->getAngle(), m_player->getPosition());
        if (tmp.first)
        {
            positionPostTP = checkingCollisionsForEachPortal3D(m_firstPortal, m_secondPortal, tmp.second, m_player->getPosition(), m_player->getAngle());
        }
        else
        {
            positionPostTP = checkingCollisionsForEachPortal3D(m_secondPortal, m_firstPortal, tmp.second, m_player->getPosition(), m_player->getAngle());
        }
        m_player->setPosition(positionPostTP.second);
        m_player->setAngle(positionPostTP.first);
    }
}

gf::Vector2f castRay(gf::Vector2f position, gf::Vector2f direction, MapWalls *m_walls, std::pair<gf::Vector2f, gf::Vector2f> portalSegment = std::make_pair(gf::Vector2f(0, 0), gf::Vector2f(0, 0)))
{
    bool isPortalVertical = std::abs(portalSegment.first.x - portalSegment.second.x) < DELTA;
    bool isPlayerLeft = position.x < portalSegment.first.x;
    bool isPlayerUp = position.y < portalSegment.first.y;
    bool isHitPointBehindThePortal = true;
    bool isPortal = portalSegment != std::make_pair(gf::Vector2f(0, 0), gf::Vector2f(0, 0));

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

    for (int i = 0; i < 1000; i++)
    {
        if (distX <= distY)
        {
            tileX += (vX > 0) ? 1 : -1;
            gf::Vector2f hitPoint = position + distX * direction;

            isHitPointBehindThePortal = true;
            if (isPortalVertical)
            {
                if (isPlayerLeft)
                {
                    if (hitPoint.x >= portalSegment.first.x)
                    {
                        isHitPointBehindThePortal = false;
                    }
                }
                else
                {
                    if (hitPoint.x <= portalSegment.first.x)
                    {
                        isHitPointBehindThePortal = false;
                    }
                }
            }
            else
            {
                if (isPlayerUp)
                {
                    if (hitPoint.y >= portalSegment.first.y)
                    {
                        isHitPointBehindThePortal = false;
                    }
                }
                else
                {
                    if (hitPoint.y <= portalSegment.first.y)
                    {
                        isHitPointBehindThePortal = false;
                    }
                }
            }

            // is the point very close to an intersection on the grid ?
            double decX = hitPoint.x - (long)hitPoint.x;
            double decY = hitPoint.y - (long)hitPoint.y;
            if ((decX < DELTA / 2 || 1 - decX < DELTA / 2) &&                                                     // the point X is very close to an intersection on the grid
                (decY < DELTA / 2 || 1 - decY < DELTA / 2) &&                                                     // the point Y is very close to an intersection on the grid
                (std::abs(hitPoint.x - position.x) > DELTA / 2 || std::abs(hitPoint.y - position.y) > DELTA / 2)) // the point is not the starting point
            {
                // is there a wall around the intersection ? -> if yes, we return the intersection
                gf::Vector2i intersection((int)(hitPoint.x + 0.5), (int)(hitPoint.y + 0.5));
                if (m_walls->getTile(intersection.x, intersection.y) != 0 ||
                    m_walls->getTile(intersection.x - 1, intersection.y) != 0 ||
                    m_walls->getTile(intersection.x, intersection.y - 1) != 0 ||
                    m_walls->getTile(intersection.x - 1, intersection.y - 1) != 0)
                {
                    if (!isHitPointBehindThePortal || !isPortal)
                    {
                        return intersection;
                    }
                }
            }

            if (m_walls->getTile(tileX, tileY) != 0)
            {
                // if we are using a portal, we check if the point is before the segment of the portal
                // if yes, we ignore the collision and continue the ray
                if (!isHitPointBehindThePortal || !isPortal)
                {
                    return hitPoint;
                }
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
                    // if we are using a portal, we check if the point is before the segment of the portal
                    // if yes, we ignore the collision and continue the ray
                    if (!isHitPointBehindThePortal || !isPortal)
                    {
                        return intersection;
                    }
                }
            }

            if (m_walls->getTile(tileX, tileY) != 0)
            {
                // if we are using a portal, we check if the point is before the segment of the portal
                // if yes, we ignore the collision and continue the ray
                if (!isHitPointBehindThePortal || !isPortal)
                {
                    return hitPoint;
                }
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
bool getVisibleSegment(gf::Vector2f &start, gf::Vector2f &end, gf::Vector2f playerPosition, double playerAngle, double a, double b)
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
        if (std::abs(a) < DELTA) // !!! have to check if even possible
            a = DELTA / 2;
        intersectionPoint = gf::Vector2f((start.y - b) / a, start.y);
        bool isLookingRight = playerAngle < gf::Pi / 2 || playerAngle > 3 * gf::Pi / 2;
        if (isLookingRight)
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

gf::Vector2f getIntersectionForPortals3D(std::pair<gf::Vector2i, gf::Vector2i> portalSegment, std::pair<gf::Vector2i, gf::Vector2i> segmentToIntersect, gf::Vector2f playerPosition)
{
    bool isPortalVertical = std::abs(portalSegment.first.x - portalSegment.second.x) < DELTA;
    bool isSegmentVertical = std::abs(segmentToIntersect.first.x - segmentToIntersect.second.x) < DELTA;
    bool isPlayerLeft = playerPosition.x < portalSegment.first.x;
    bool isPlayerUp = playerPosition.y < portalSegment.first.y;
    // std::cout << "segment : (" << segmentToIntersect.first.x << ", " << segmentToIntersect.first.y << ") -> (" << segmentToIntersect.second.x << ", " << segmentToIntersect.second.y << ")" << std::endl;

    if (isPortalVertical && !isSegmentVertical && (segmentToIntersect.first.x <= portalSegment.first.x + (isPlayerLeft ? DELTA : -DELTA) && segmentToIntersect.second.x >= portalSegment.first.x + (isPlayerLeft ? DELTA : -DELTA) || segmentToIntersect.first.x >= portalSegment.first.x + (isPlayerLeft ? DELTA : -DELTA) && segmentToIntersect.second.x <= portalSegment.first.x + (isPlayerLeft ? DELTA : -DELTA)))
    {
        // std::cout << "INTERSECTION" << std::endl;
        return gf::Vector2f(portalSegment.first.x, segmentToIntersect.first.y);
    }
    else if (!isPortalVertical && isSegmentVertical && (portalSegment.first.y + (isPlayerUp ? DELTA : -DELTA) <= segmentToIntersect.first.y && portalSegment.second.y + (isPlayerUp ? DELTA : -DELTA) >= segmentToIntersect.first.y || portalSegment.first.y + (isPlayerUp ? DELTA : -DELTA) >= segmentToIntersect.first.y && portalSegment.second.y + (isPlayerUp ? DELTA : -DELTA) <= segmentToIntersect.first.y))
    {
        return gf::Vector2f(segmentToIntersect.first.x, portalSegment.first.y);
    }
    return gf::Vector2f(0, 0);
}

void removeVerticesBefore3D(std::vector<gf::Vector2f> &vertices, std::pair<gf::Vector2i, gf::Vector2i> portalSegment, gf::Vector2f playerPosition)
{
    bool isPortalVertical = std::abs(portalSegment.first.x - portalSegment.second.x) < DELTA;
    bool isPlayerLeft = playerPosition.x < portalSegment.first.x;
    bool isPlayerUp = playerPosition.y < portalSegment.first.y;
    for (int i = 0; i < vertices.size(); i++)
    {
        if (isPortalVertical)
        {
            if (isPlayerLeft)
            {
                if (vertices[i].x <= portalSegment.first.x)
                {
                    vertices.erase(vertices.begin() + i);
                    i--;
                }
            }
            else
            {
                if (vertices[i].x >= portalSegment.first.x)
                {
                    vertices.erase(vertices.begin() + i);
                    i--;
                }
            }
        }
        else
        {
            if (isPlayerUp)
            {
                if (vertices[i].y <= portalSegment.first.y)
                {
                    vertices.erase(vertices.begin() + i);
                    i--;
                }
            }
            else
            {
                if (vertices[i].y >= portalSegment.first.y)
                {
                    vertices.erase(vertices.begin() + i);
                    i--;
                }
            }
        }
    }
}

void Game3D::castPortal(bool isFirstPortal)
{
    struct portal *portal = isFirstPortal ? m_firstPortal : m_secondPortal;
    struct portal *otherPortal = isFirstPortal ? m_secondPortal : m_firstPortal;

    // if the portal is not set, we set it
    if (portal == NULL)
    {
        portal = new struct portal;
        if (isFirstPortal)
        {
            m_firstPortal = portal;
        }
        else
        {
            m_secondPortal = portal;
        }
    }

    // -- cast a ray to find the closest wall
    gf::Vector2f position = m_player->getPosition();
    gf::Vector2f direction = gf::normalize(gf::Vector2f(std::cos(m_player->getAngle()), std::sin(m_player->getAngle())));
    gf::Vector2f hitPoint = castRay(position, direction, m_walls);

    // -- set the portal position
    portal->position = hitPoint;

    // -- set the portal facing
    // getting the segment of the wall hit by the ray
    std::vector<std::pair<gf::Vector2i, gf::Vector2i>> segments;
    if (m_walls->getSegments(hitPoint, segments))
    {
        auto segment = segments[0];
        bool isSegmentVertical = std::abs(segment.first.x - segment.second.x) < DELTA;
        bool isPlayerLeft = position.x < segment.first.x;
        bool isPlayerUp = position.y < segment.first.y;
        if (isSegmentVertical)
        {
            portal->position.y = (int)(portal->position.y) + 0.5; // setting the position to the center of the tile
            if (isPlayerLeft)
            {
                portal->facing = 2;
            }
            else
            {
                portal->facing = 0;
            }
        }
        else
        {
            portal->position.x = (int)(portal->position.x) + 0.5; // setting the position to the center of the tile
            if (isPlayerUp)
            {
                portal->facing = 3;
            }
            else
            {
                portal->facing = 1;
            }
        }
    }
    // if the other portal is already set, we link the two portals
    if (otherPortal != NULL)
    {
        portal->linkedPortal = otherPortal;
        otherPortal->linkedPortal = portal;
    }
}

// !!! a corriger

std::vector<gf::Vector2f> getPointsBetween(gf::Vector2f start, gf::Vector2f end)
{
    std::vector<gf::Vector2f> points;

    // Ajouter le point de départ
    points.push_back(start);

    if (std::abs(start.x - end.x) < DELTA)
    {
        // Segment vertical
        int startY = std::ceil(std::min(start.y, end.y));
        int endY = std::floor(std::max(start.y, end.y));

        for (int y = startY; y < endY; ++y)
        {
            points.push_back(gf::Vector2f(start.x, y));
        }
    }
    else if (std::abs(start.y - end.y) < DELTA)
    {
        // Segment horizontal
        int startX = std::ceil(std::min(start.x, end.x));
        int endX = std::floor(std::max(start.x, end.x));

        for (int x = startX; x < endX; ++x)
        {
            points.push_back(gf::Vector2f(x, start.y));
        }
    }

    // Ajouter le point de fin si différent du point de départ
    if (start.x != end.x || start.y != end.y)
    {
        points.push_back(end);
    }

    return points;
}

void renderAWallTextured(gf::Vector2f start, gf::Vector2f end, std::vector<gf::Vector2f> trapezeVertices, gf::Texture &texture, gf::RenderWindow &renderer)
{

    bool isSegmentVertical = std::abs(start.x - end.x) < DELTA;
    // trapezeVertices is a vector of the trapeze's vertices from left to right, top to bottom

    // lets determine the center of the texture on the trapeze
    // => the center is the intersection of the diagonals of the trapeze

    gf::Vector2f center = linesIntersection(trapezeVertices[0], trapezeVertices[3], trapezeVertices[1], trapezeVertices[2]);
    gf::Vector2f topCenter = linesIntersection(center, gf::Vector2f(center.x, center.y + 1), trapezeVertices[0], trapezeVertices[1]);
    gf::Vector2f bottomCenter = linesIntersection(center, gf::Vector2f(center.x, center.y + 1), trapezeVertices[2], trapezeVertices[3]);

    float textureCenter = (isSegmentVertical) ? (start.y + end.y) / 2 : (start.x + end.x) / 2;

    gf::VertexArray trapeze(gf::PrimitiveType::Triangles, 24);
    // First triangle (Top-Left, Center, Top-Center)
    trapeze[0].position = trapezeVertices[0]; // Top-Left
    trapeze[1].position = center;             // Center
    trapeze[2].position = topCenter;          // Top-Center

    // Deuxième triangle (Top-Center, Center, Top-Right)
    trapeze[3].position = topCenter;          // Top-Center
    trapeze[4].position = center;             // Center
    trapeze[5].position = trapezeVertices[1]; // Top-Right

    // Troisième triangle (Bottom-gauche, Center, Bottom-Center)
    trapeze[6].position = trapezeVertices[2]; // Bottom-gauche
    trapeze[7].position = center;             // Center
    trapeze[8].position = bottomCenter;       // Bottom-Center

    // Quatrième triangle (Bottom-Center, Center, Bottom-Right)
    trapeze[9].position = bottomCenter;        // Bottom-Center
    trapeze[10].position = center;             // Center
    trapeze[11].position = trapezeVertices[3]; // Bottom-Right

    // Cinquième triangle (Top-Left, Center, Bottom-Gauche)
    trapeze[12].position = trapezeVertices[0]; // Top-Left
    trapeze[13].position = center;             // Center
    trapeze[14].position = trapezeVertices[2]; // Bottom-gauche

    // Sixième triangle (Top-Right, Center, Bottom-Right)
    trapeze[15].position = trapezeVertices[1]; // Top-Right
    trapeze[16].position = center;             // Center
    trapeze[17].position = trapezeVertices[3]; // Bottom-Right

    // Texture coordinates
    int scale = 3;
    trapeze[0].texCoords = {(isSegmentVertical) ? start.y : start.x, 0.0f}; // Top-Left
    trapeze[1].texCoords = {textureCenter, 0.5f};                           // Center
    trapeze[2].texCoords = {textureCenter, 0.0f};                           // Top-Center

    trapeze[3].texCoords = {textureCenter, 0.0f};                       // Top-Center
    trapeze[4].texCoords = {textureCenter, 0.5f};                       // Center
    trapeze[5].texCoords = {(isSegmentVertical) ? end.y : end.x, 0.0f}; // Top-Right

    trapeze[6].texCoords = {(isSegmentVertical) ? start.y : start.x, 1.0f}; // Bottom-left
    trapeze[7].texCoords = {textureCenter, 0.5f};                           // Center
    trapeze[8].texCoords = {textureCenter, 1.0f};                           // Bottom-Center

    trapeze[9].texCoords = {textureCenter, 1.0f};                        // Bottom-Center
    trapeze[10].texCoords = {textureCenter, 0.5f};                       // Center
    trapeze[11].texCoords = {(isSegmentVertical) ? end.y : end.x, 1.0f}; // Bottom-Right

    trapeze[12].texCoords = {(isSegmentVertical) ? start.y : start.x, 0.0f}; // Top-Left
    trapeze[13].texCoords = {textureCenter, 0.5f};                           // Center
    trapeze[14].texCoords = {(isSegmentVertical) ? start.y : start.x, 1.0f}; // Bottom-left

    trapeze[15].texCoords = {(isSegmentVertical) ? end.y : end.x, 0.0f}; // Top-Right
    trapeze[16].texCoords = {textureCenter, 0.5f};                       // Center
    trapeze[17].texCoords = {(isSegmentVertical) ? end.y : end.x, 1.0f}; // Bottom-Right

    texture.setRepeated(true);

    gf::RenderStates states;
    states.texture[0] = &texture;

    renderer.draw(trapeze, states);
}

void renderAPortal(bool isFirstPortal, gf::Vector2f startPortal, gf::Vector2f endPortal, gf::Vector2f position, double angle, gf::RenderWindow &m_renderer)
{
    gf::Vector2i viewSize = m_renderer.getView().getSize();
    double viewWidth = viewSize.x;
    double viewHeight = viewSize.y;

    bool isSegmentVertical = std::abs(startPortal.x - endPortal.x) < DELTA;
    auto color = (isFirstPortal) ? gf::Color::Cyan : gf::Color::Orange;

    gf::VertexArray trapezoid(gf::PrimitiveType::Triangles, 6);
    trapezoid[0].color = color;
    trapezoid[1].color = color;
    trapezoid[2].color = color;
    trapezoid[3].color = color;
    trapezoid[4].color = color;
    trapezoid[5].color = color;

    // if one of the end point or start point is behind the player, we do not draw it
    if (getVisibleSegment(startPortal, endPortal, position, angle, -1 / std::tan(angle), position.y - (-1 / std::tan(angle)) * position.x))
    {

        double startAngle = std::atan2(startPortal.y - position.y, startPortal.x - position.x);
        double endAngle = std::atan2(endPortal.y - position.y, endPortal.x - position.x);

        double falseDistanceStart = std::sqrt((startPortal.x - position.x) * (startPortal.x - position.x) + (startPortal.y - position.y) * (startPortal.y - position.y));
        double falseDistanceEnd = std::sqrt((endPortal.x - position.x) * (endPortal.x - position.x) + (endPortal.y - position.y) * (endPortal.y - position.y));

        double realDistanceStart = falseDistanceStart * std::cos(startAngle - angle);
        double realDistanceEnd = falseDistanceEnd * std::cos(endAngle - angle);

        double heightStart = (realDistanceStart != 0) ? viewHeight / realDistanceStart / 2 : viewHeight * 2;
        double heightEnd = (realDistanceEnd != 0) ? viewHeight / realDistanceEnd / 2 : viewHeight * 2;

        double xPosStart = std::tan(startAngle - angle) * viewWidth / 2;
        double xPosEnd = std::tan(endAngle - angle) * viewWidth / 2;

        trapezoid[0].position = gf::Vector2f(viewWidth / 2 + xPosStart, viewHeight / 2 - heightStart / 2);
        trapezoid[1].position = gf::Vector2f(viewWidth / 2 + xPosStart, viewHeight / 2 + heightStart / 2);
        trapezoid[2].position = gf::Vector2f(viewWidth / 2 + xPosEnd, viewHeight / 2 - heightEnd / 2);
        trapezoid[3].position = gf::Vector2f(viewWidth / 2 + xPosStart, viewHeight / 2 + heightStart / 2);
        trapezoid[4].position = gf::Vector2f(viewWidth / 2 + xPosEnd, viewHeight / 2 - heightEnd / 2);
        trapezoid[5].position = gf::Vector2f(viewWidth / 2 + xPosEnd, viewHeight / 2 + heightEnd / 2);
        m_renderer.draw(trapezoid);
    }
}

void resizePortal(gf::Vector2f &portalStart, gf::Vector2f &portalEnd, gf::Vector2f segmentStart, gf::Vector2f segmentEnd)
{
    bool isSegmentVertical = std::abs(segmentStart.x - segmentEnd.x) < DELTA;
    int index = (isSegmentVertical) ? 1 : 0;

    portalStart[index] = std::max(portalStart[index], std::min(segmentStart[index], segmentEnd[index]));
    portalEnd[index] = std::min(portalEnd[index], std::max(segmentStart[index], segmentEnd[index]));

    // std::cout << "portalStart : (" << portalStart.x << ", " << portalStart.y << ")" << std::endl;
    // std::cout << "portalEnd : (" << portalEnd.x << ", " << portalEnd.y << ")" << std::endl << std::endl;
}

bool isBetween(gf::Vector2f point, gf::Vector2f start, gf::Vector2f end)
{
    bool isSegmentVertical = std::abs(start.x - end.x) < DELTA;
    // std::cout << "isSegmentVertical : " << isSegmentVertical << std::endl;
    int index = (isSegmentVertical) ? 0 : 1;
    int index2 = (index + 1) % 2;
    bool isOnSegment = std::abs(point[index] - start[index]) < DELTA;
    return point[index2] > std::min(start[index2], end[index2]) && point[index2] < std::max(start[index2], end[index2]);
}

void Game3D::render(bool isPortal, std::pair<gf::Vector2i, gf::Vector2i> portalSegment)
{
    // setting the precision of cout 
    std::cout.precision(100);
    // std::cout << std::endl;

    // getting the segments where the portal are on :
    auto firstPortalSegment = std::pair<gf::Vector2i, gf::Vector2i>(gf::Vector2i(0, 0), gf::Vector2i(0, 0));
    std::vector<std::pair<gf::Vector2i, gf::Vector2i>> segmentsP;
    if (m_firstPortal != NULL && m_walls->getSegments(m_firstPortal->position, segmentsP))
    {

        firstPortalSegment = segmentsP[0];
    }
    auto secondPortalSegment = std::pair<gf::Vector2i, gf::Vector2i>(gf::Vector2i(0, 0), gf::Vector2i(0, 0));
    if (m_secondPortal != NULL && m_walls->getSegments(m_secondPortal->position, segmentsP))
    {
        secondPortalSegment = segmentsP[0];
    }

    std::vector<Wall> walls = m_walls->getWalls();
    std::map<std::pair<gf::Vector2i, gf::Vector2i>, std::vector<gf::Vector2f>, PairComparator> segments;

    // int nbSupplementaryRays = 0;

    // render the rays :
    gf::Vector2f position = m_player->getPosition();
    // we offset the position if needed
    if ((int)(position.x * 1000) == position.x * 1000)
    {
        position.x = position.x + DELTA;
    }
    if ((int)(position.y * 1000) == position.y * 1000)
    {
        position.y = position.y - DELTA;
    }

    std::vector<gf::Vector2i> sV = m_walls->getVertices();
    std::vector<gf::Vector2f> sortedVertices;
    // std::cout << "sV : " << std::endl;
    for (auto v : sV)
    {
        // std::cout << "-\t(" << v.x << ", " << v.y << ")" << std::endl;
        sortedVertices.push_back(gf::Vector2f(v.x, v.y));
    }
    // if we are in a portal, we add a vertex at every intersection between a segment and the line defined by the segment of the portal
    // then we remove every vertices that are before that line
    if (isPortal)
    {
        // adding the vertices :
        std::set<std::pair<gf::Vector2i, gf::Vector2i>, PairComparator> seenSegments;
        // a copy of sortedVertices is needed because we are adding vertices to the vector while iterating over it
        std::vector<gf::Vector2f> sortedVerticesCopy = sortedVertices;
        // removing the vertices not needed :
        removeVerticesBefore3D(sortedVertices, portalSegment, m_player->getPosition());
        for (auto v : sortedVerticesCopy)
        {
            // std::cout << "vertex : (" << v.x << ", " << v.y << ")" << std::endl;
            std::vector<std::pair<gf::Vector2i, gf::Vector2i>> vertexSegments;
            if (m_walls->getSegments(v, vertexSegments))
            {
                for (auto segment : vertexSegments)
                {
                    // std::cout << "in segment : (" << segment.first.x << ", " << segment.first.y << ") -> (" << segment.second.x << ", " << segment.second.y << ")" << std::endl;
                    if (seenSegments.insert(segment).second)
                    {
                        gf::Vector2f intersection = getIntersectionForPortals3D(portalSegment, segment, m_player->getPosition());
                        if (intersection != gf::Vector2f(0, 0))
                        {
                            sortedVertices.push_back(gf::Vector2f(intersection.x, intersection.y));
                            // // rendering a small circle
                            // double radius = 3.0f;
                            // gf::CircleShape circle(radius * 2);
                            // circle.setPosition(intersection * m_scaleUnit - gf::Vector2f(radius, radius));
                            // circle.setColor(gf::Color::Red);
                            // m_renderer.draw(circle);
                        }
                    }
                }
            }
        }
        // for (auto v : sortedVertices)
        // {
        //     double radius = 3.0f;
        //     gf::CircleShape circle(radius * 2);
        //     circle.setPosition(v * m_scaleUnit - gf::Vector2f(radius, radius));
        //     circle.setColor(gf::Color::White);
        //     m_renderer.draw(circle);
        // }
    }
    std::sort(sortedVertices.begin(), sortedVertices.end(), CompareVerticesAngle(position));

    // std::vector<gf::Vector2i> sortedVertices = wall.getSortedVertices(m_player->getPosition());

    for (int i = 0; i < sortedVertices.size(); i++)
    {
        gf::Vector2f direction = gf::normalize(gf::Vector2f(sortedVertices[i].x - position.x, sortedVertices[i].y - position.y));

        gf::Vector2f endPoint;

        if (isPortal)
        {
            endPoint = castRay(position, direction, m_walls, portalSegment);
        }
        else
        {
            endPoint = castRay(position, direction, m_walls);
        }
        double dist = std::sqrt((sortedVertices[i].x - position.x) * (sortedVertices[i].x - position.x) + (sortedVertices[i].y - position.y) * (sortedVertices[i].y - position.y));

        if (std::abs(sortedVertices[i].x - endPoint.x) < DELTA && std::abs(sortedVertices[i].y - endPoint.y) < DELTA)
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

        bool isSegmentVertical = std::abs(segment.first.first.x - segment.first.second.x) < DELTA;
        bool isSegmentRightToPlayer = segment.first.first.x > m_player->getPosition().x;
        bool isSegmentUpToPlayer = segment.first.first.y < m_player->getPosition().y;

        auto color = (isSegmentVertical) ? gf::Color::fromRgba32(0x77FF77FF) : gf::Color::fromRgba32(0x7777FFFF);

        for (std::size_t i = 0; i < segment.second.size() - 1; i += 2)
        {
            if (segment.second.size() < 2)
            {
                continue;
            }
            std::vector<gf::Vector2f> points = getPointsBetween(segment.second[i], segment.second[i + 1]);
            // std::cout << std::endl
            //           << "points : " << std::endl;
            // for (auto p : points)
            // {
            //     std::cout << "\t-(" << p.x << ", " << p.y << ")" << std::endl;
            // }

            for (size_t i = 0; i < points.size() - 1; i++)
            {

                gf::Vector2f start = points[i];
                gf::Vector2f end = points[i + 1];

                if (start == end)
                {
                    continue;
                }

                double angle = m_player->getAngle();
                if (angle == 0 || angle == gf::Pi || angle == 2 * gf::Pi || angle == -gf::Pi)
                {
                    angle += DELTA / 2;
                }
                gf::Vector2f pos = m_player->getPosition();
                double a = -1 / std::tan(angle);
                double b = pos.y - a * pos.x;
                // skip the segment if it is behind the player if needed
                if (!getVisibleSegment(start, end, pos, angle, a, b))
                {
                    continue;
                }

                gf::Vector2i viewSize = m_renderer.getView().getSize();
                double viewWidth = viewSize.x;
                double viewHeight = viewSize.y;
                // std::cout << "viewWidth : " << viewWidth << std::endl;
                // std::cout << "viewHeight : " << viewHeight << std::endl;

                // std::cout << "start : (" << start.x << ", " << start.y << ") end : (" << end.x << ", " << end.y << ")" << std::endl;
                // --- For the first point of the segment ---

                // the angle of the ray to the player:
                double rayAngle1 = std::atan2(start.y - pos.y, start.x - pos.x);

                // the distance of the wall to the player :
                double falseDistance1 = std::sqrt((start.x - pos.x) * (start.x - pos.x) + (start.y - pos.y) * (start.y - pos.y));
                // the real distance is the projection of the ray on the direction vector :
                double realDistance1 = falseDistance1 * std::cos(rayAngle1 - angle);

                // the height of the projected wall :
                // Projected Slice Height = realSlideHeight / Distance to the Slice * Distance to the Projection Plane * scaling
                double height1 = (realDistance1 != 0) ? viewHeight / realDistance1 / 2 : viewHeight * 2;

                // the x position of the column from the center of the screen :
                double xPos1 = std::tan(rayAngle1 - angle) * viewWidth / 2;

                // drawing the column :
                gf::VertexArray column1(gf::PrimitiveType::Lines, 2);
                column1[0].color = gf::Color::fromRgba32(0x7777FFFF);
                column1[1].color = gf::Color::fromRgba32(0x7777FFFF);

                column1[0].position = gf::Vector2f(viewWidth / 2 + xPos1, viewHeight / 2 - height1 / 2);
                column1[1].position = gf::Vector2f(viewWidth / 2 + xPos1, viewHeight / 2 + height1 / 2);

                // m_renderer.draw(column1);

                // --- For the second point of the segment ---

                // the angle of the ray to the player:
                double rayAngle2 = std::atan2(end.y - pos.y, end.x - pos.x);

                // the distance of the wall to the player :
                double falseDistance2 = std::sqrt((end.x - pos.x) * (end.x - pos.x) + (end.y - pos.y) * (end.y - pos.y));
                // the real distance is the projection of the ray on the direction vector :
                double realDistance2 = falseDistance2 * std::cos(rayAngle2 - angle);

                // the height of the projected wall :
                // Projected Slice Height = realSlideHeight / Distance to the Slice * Distance to the Projection Plane * scaling
                double height2 = (realDistance2 != 0) ? viewHeight / realDistance2 / 2 : viewHeight;

                // the x position of the column from the center of the screen :
                double xPos2 = std::tan(rayAngle2 - angle) * viewWidth / 2;

                // drawing the column :
                gf::VertexArray column2(gf::PrimitiveType::Lines, 2);
                column2[0].color = gf::Color::fromRgba32(0x77FF77FF);
                column2[1].color = gf::Color::fromRgba32(0x77FF77FF);

                column2[0].position = gf::Vector2f(viewWidth / 2 + xPos2, viewHeight / 2 - height2 / 2);
                column2[1].position = gf::Vector2f(viewWidth / 2 + xPos2, viewHeight / 2 + height2 / 2);

                renderAWallTextured(start, end, {column1[0].position, column2[0].position, column1[1].position, column2[1].position}, m_texture, m_renderer);

                // m_renderer.draw(column2);

                // linking the two columns :
                // gf::VertexArray link(gf::PrimitiveType::Lines, 2);
                // link[0].color = gf::Color::fromRgba32(0xFF7777FF);
                // link[1].color = gf::Color::fromRgba32(0xFF7777FF);

                // link[0].position = column1[1].position;
                // link[1].position = column2[1].position;
                // m_renderer.draw(link);

                // link[0].position = column1[0].position;
                // link[1].position = column2[0].position;
                // m_renderer.draw(link);
                // std::cout << "posX1 : " << column1[0].position.x << " posX2 : " << column2[0].position.x << std::endl;

                // bool isVertical = std::abs(start.x - end.x) < DELTA;

                // --- For the triangles ---
                // gf::VertexArray triangle(gf::PrimitiveType::Triangles, 3);
                // triangle[0].color = color;
                // triangle[1].color = color;
                // triangle[2].color = color;

                // // we draw the trapezoid with two triangles :

                // triangle[0].position = column1[0].position;
                // triangle[1].position = column1[1].position;
                // triangle[2].position = column2[0].position;
                // m_renderer.draw(triangle);

                // triangle[0].position = column1[1].position;
                // triangle[1].position = column2[0].position;
                // triangle[2].position = column2[1].position;
                // m_renderer.draw(triangle);

                // We want to draw a texture on the wall but we have to manually repeat the texture
                // For that we have to segment the wall in segments of size 1 and not begin necessarily at the start of the wall

                // int coordToCheck = (isSegmentVertical) ? 1 : 0; // if the wall is vertical, we check the y coordinate, else we check the x coordinate
                // float startCoordOnTexture = (isSegmentVertical && isSegmentRightToPlayer || !isSegmentVertical && isSegmentUpToPlayer) ? start[coordToCheck] - (int)(start[coordToCheck]) : 1 - start[coordToCheck] - (int)(start[coordToCheck]);
                // float endCoordOnTexture = (isSegmentVertical && isSegmentRightToPlayer || !isSegmentVertical && isSegmentUpToPlayer) ? end[coordToCheck] - (int)(end[coordToCheck]) : 1 - end[coordToCheck] - (int)(end[coordToCheck]);

                // // we manage the case where the segment to draw is shorter than 1
                // double diff = std::abs(start[coordToCheck] - end[coordToCheck]);

                // // !!! TODO : il faut prendre en compte que même si le segment est plus petit que 1 il faut
                // // si le start et end ne sont pas sur la même cellule, il faut dessiner deux fractions de la texture
                // // aussi il faut avoir redetermine les coordonnées des "colonnes" avant d'afficher la fraction de texture (factoriser les coordonnées projetés)
                // // peut être il faut fractionner avant et balancer dans une fonction deux colonnes et elle se charge de tout avec une lambda fonction!!!
                // if (diff <= 1)
                // {
                //     gf::VertexArray vertices2(gf::PrimitiveType::Triangles, 6);
                //     // Premier triangle (haut-gauche, bas-gauche, haut-droite)
                //     vertices2[0].position = column1[1].position; // Haut-gauche
                //     vertices2[1].position = column1[0].position; // Bas-gauche
                //     vertices2[2].position = column2[1].position; // Haut-droite

                //     // Second triangle (bas-gauche, bas-droite, haut-droite)
                //     vertices2[3].position = column1[0].position; // Bas-gauche
                //     vertices2[4].position = column2[0].position; // Bas-droite
                //     vertices2[5].position = column2[1].position; // Haut-droite

                //     // Coordonnées de texture ajustées pour assurer une déformation uniforme entre les deux triangles
                //     vertices2[0].texCoords = {start.x, 0.0f}; // Haut-gauche
                //     vertices2[1].texCoords = {start.x, 1.0f}; // Bas-gauche
                //     vertices2[2].texCoords = {end.x, 0.0f};   // Haut-droite

                //     vertices2[3].texCoords = {start.x, 1.0f}; // Bas-gauche
                //     vertices2[4].texCoords = {end.x, 1.0f};   // Bas-droite
                //     vertices2[5].texCoords = {end.x, 0.0f};   // Haut-droite

                //     m_texture.setRepeated(true);
                //     gf::RenderStates states;
                //     states.texture[0] = &m_texture;
                //     m_renderer.draw(vertices2, states);
                // }
                // // else // !!! TODO : il faut prendre en compte que même si le segment est plus petit que 1 il faut
                // // {
                // //     // in that case we draw the first none complete segment, then the complete segments, then the last none complete segment

                // //     // first segment :
                // //     gf::VertexArray vertices2(gf::PrimitiveType::Triangles, 6);
                // //     // Premier triangle (haut-gauche, bas-gauche, haut-droite)
                // //     vertices2[0].position = column1[1].position; // Haut-gauche
                // //     vertices2[1].position = column1[0].position; // Bas-gauche
                // //     vertices2[2].position = column2[1].position; // Haut-droite

                // //     // Second triangle (bas-gauche, bas-droite, haut-droite)
                // //     vertices2[3].position = column1[0].position; // Bas-gauche
                // //     vertices2[4].position = column2[0].position; // Bas-droite
                // //     vertices2[5].position = column2[1].position; // Haut-droite

                // //     // Coordonnées de texture ajustées pour assurer une déformation uniforme entre les deux triangles
                // //     vertices2[0].texCoords = {startCoordOnTexture, 0.0f}; // Haut-gauche
                // //     vertices2[1].texCoords = {startCoordOnTexture, 1.0f}; // Bas-gauche
                // //     vertices2[2].texCoords = {1.0f, 0.0f}; // Haut-droite

                // //     vertices2[3].texCoords = {1.0f, 1.0f}; // Bas-gauche
                // //     vertices2[4].texCoords = {1.0f, 1.0f}; // Bas-droite
                // //     vertices2[5].texCoords = {1.0f, 0.0f}; // Haut-droite

                // //     while(--diff > 1)
                // //     {
                // //         gf::RenderStates states;
                // //         states.texture[0] = &m_texture;
                // //         m_renderer.draw(vertices2, states);
                // //     }

                // //     gf::RenderStates states;
                // //     states.texture[0] = &m_texture;
                // //     m_renderer.draw(vertices2, states);
                // // }
                // else
                // {
                //     gf::VertexArray vertices2(gf::PrimitiveType::Triangles, 6);
                //     // Premier triangle (haut-gauche, bas-gauche, haut-droite)
                //     vertices2[0].position = column1[1].position; // Haut-gauche
                //     vertices2[1].position = column1[0].position; // Bas-gauche
                //     vertices2[2].position = column2[1].position; // Haut-droite

                //     // Second triangle (bas-gauche, bas-droite, haut-droite)
                //     vertices2[3].position = column1[0].position; // Bas-gauche
                //     vertices2[4].position = column2[0].position; // Bas-droite
                //     vertices2[5].position = column2[1].position; // Haut-droite

                //     // Coordonnées de texture ajustées pour assurer une déformation uniforme entre les deux triangles
                //     vertices2[0].texCoords = {start.x, 0.0f}; // Haut-gauche
                //     vertices2[1].texCoords = {start.x, 1.0f}; // Bas-gauche
                //     vertices2[2].texCoords = {end.x, 0.0f};   // Haut-droite

                //     vertices2[3].texCoords = {start.x, 1.0f}; // Bas-gauche
                //     vertices2[4].texCoords = {end.x, 1.0f};   // Bas-droite
                //     vertices2[5].texCoords = {end.x, 0.0f};   // Haut-droite

                //     m_texture.setRepeated(true);
                //     gf::RenderStates states;
                //     states.texture[0] = &m_texture;
                //     m_renderer.draw(vertices2, states);
                // }
            }
            // render the portals if it is on that segment :
            int index = (isSegmentVertical) ? 1 : 0;
            // if (segment.second.size() > i + 1 && m_firstPortal != NULL && firstPortalSegment == segment.first)
            // {
            //     std::cout << "m_firstPortal->position[index] : " << m_firstPortal->position[index] << std::endl;

            //     std::cout << "segment.second[i][index] : " << segment.second[i][index] << std::endl;
            //     std::cout << "segment.second[i + 1][index] : " << segment.second[i + 1][index] << std::endl;
            // }
            if (m_firstPortal != NULL && firstPortalSegment == segment.first)
            {
                gf::Vector2f startPortal(m_firstPortal->position.x - ((m_firstPortal->facing % 2 == 1) ? m_firstPortal->width / 2 : 0), m_firstPortal->position.y - ((m_firstPortal->facing % 2 == 0) ? m_firstPortal->width / 2 : 0));
                gf::Vector2f endPortal(m_firstPortal->position.x + ((m_firstPortal->facing % 2 == 1) ? m_firstPortal->width / 2 : 0), m_firstPortal->position.y + ((m_firstPortal->facing % 2 == 0) ? m_firstPortal->width / 2 : 0));
                
                // std::cout << "segment.second[i] : (" << segment.second[i].x << ", " << segment.second[i].y << ")" << std::endl;
                // std::cout << "segment.second[i + 1] : (" << segment.second[i + 1].x << ", " << segment.second[i + 1].y << ")" << std::endl;
                // std::cout << "portal : (" << startPortal.x << ", " << startPortal.y << ") -> (" << endPortal.x << ", " << endPortal.y << ")" << std::endl;
                bool isBetweenStart = isBetween(startPortal, segment.second[i], segment.second[i + 1]);
                bool isBetweenEnd = isBetween(endPortal, segment.second[i], segment.second[i + 1]);
                // std::cout << "isBetweenStart : " << isBetweenStart << std::endl;
                // std::cout << "isBetweenEnd : " << isBetweenEnd << std::endl;
                if (segment.second.size() > 1 && (isBetweenStart || isBetweenEnd))
                {
                    // std::cout << "Rendering the first portal" << std::endl;
                    resizePortal(startPortal, endPortal, segment.second[i], segment.second[i + 1]);
                    renderAPortal(true, startPortal, endPortal, position, m_player->getAngle(), m_renderer);
                }
                // else
                // {
                //     std::cout << "Not rendering the first portal" << std::endl;
                // }
            }

            if (m_secondPortal != NULL && secondPortalSegment == segment.first)
            {
                gf::Vector2f startPortal(m_secondPortal->position.x - ((m_secondPortal->facing % 2 == 1) ? m_secondPortal->width / 2 : 0), m_secondPortal->position.y - ((m_secondPortal->facing % 2 == 0) ? m_secondPortal->width / 2 : 0));
                gf::Vector2f endPortal(m_secondPortal->position.x + ((m_secondPortal->facing % 2 == 1) ? m_secondPortal->width / 2 : 0), m_secondPortal->position.y + ((m_secondPortal->facing % 2 == 0) ? m_secondPortal->width / 2 : 0));

                if (segment.second.size() > 1 && m_secondPortal != NULL && secondPortalSegment == segment.first && (isBetween(startPortal, segment.second[i], segment.second[i + 1]) || isBetween(endPortal, segment.second[i], segment.second[i + 1])))
                {
                    // std::cout << "Rendering the second portal" << std::endl;
                    resizePortal(startPortal, endPortal, segment.second[i], segment.second[i + 1]);
                    renderAPortal(false, startPortal, endPortal, position, m_player->getAngle(), m_renderer);
                }
                // else
                // {
                //     std::cout << "Not rendering the second portal" << std::endl;
                // }
            }
        }
    }

    // render the portals :
    // a rectangle for now
    // gf::Vector2i viewSize = m_renderer.getView().getSize();
    // double viewWidth = viewSize.x;
    // double viewHeight = viewSize.y;
    // if (m_firstPortal != NULL)
    // {

    //     gf::VertexArray triangle(gf::PrimitiveType::Triangles, 3);
    //     triangle[0].color = gf::Color::Cyan;
    //     triangle[1].color = gf::Color::Cyan;
    //     triangle[2].color = gf::Color::Cyan;

    //     gf::Vector2f startPortal(m_firstPortal->position.x - ((m_firstPortal->facing % 2 == 1) ? m_firstPortal->width / 2 : 0), m_firstPortal->position.y - ((m_firstPortal->facing % 2 == 0) ? m_firstPortal->width / 2 : 0));
    //     gf::Vector2f endPortal(m_firstPortal->position.x + ((m_firstPortal->facing % 2 == 1) ? m_firstPortal->width / 2 : 0), m_firstPortal->position.y + ((m_firstPortal->facing % 2 == 0) ? m_firstPortal->width / 2 : 0));

    //     // if one of the end point or start point is behind the player, we do not draw it
    //     if (getVisibleSegment(startPortal, endPortal, position, m_player->getAngle(), -1 / std::tan(m_player->getAngle()), position.y - (-1 / std::tan(m_player->getAngle())) * position.x))
    //     {

    //         double startAngle = std::atan2(startPortal.y - position.y, startPortal.x - position.x);
    //         double endAngle = std::atan2(endPortal.y - position.y, endPortal.x - position.x);

    //         double falseDistanceStart = std::sqrt((startPortal.x - position.x) * (startPortal.x - position.x) + (startPortal.y - position.y) * (startPortal.y - position.y));
    //         double falseDistanceEnd = std::sqrt((endPortal.x - position.x) * (endPortal.x - position.x) + (endPortal.y - position.y) * (endPortal.y - position.y));

    //         double realDistanceStart = falseDistanceStart * std::cos(startAngle - m_player->getAngle());
    //         double realDistanceEnd = falseDistanceEnd * std::cos(endAngle - m_player->getAngle());

    //         double heightStart = (realDistanceStart != 0) ? viewHeight / realDistanceStart / 2 : viewHeight * 2;
    //         double heightEnd = (realDistanceEnd != 0) ? viewHeight / realDistanceEnd / 2 : viewHeight * 2;

    //         double xPosStart = std::tan(startAngle - m_player->getAngle()) * viewWidth / 2;
    //         double xPosEnd = std::tan(endAngle - m_player->getAngle()) * viewWidth / 2;

    //         triangle[0].position = gf::Vector2f(viewWidth / 2 + xPosStart, viewHeight / 2 - heightStart / 2);
    //         triangle[1].position = gf::Vector2f(viewWidth / 2 + xPosStart, viewHeight / 2 + heightStart / 2);
    //         triangle[2].position = gf::Vector2f(viewWidth / 2 + xPosEnd, viewHeight / 2 - heightEnd / 2);
    //         m_renderer.draw(triangle);

    //         triangle[0].position = gf::Vector2f(viewWidth / 2 + xPosStart, viewHeight / 2 + heightStart / 2);
    //         triangle[1].position = gf::Vector2f(viewWidth / 2 + xPosEnd, viewHeight / 2 - heightEnd / 2);
    //         triangle[2].position = gf::Vector2f(viewWidth / 2 + xPosEnd, viewHeight / 2 + heightEnd / 2);
    //         m_renderer.draw(triangle);

    //         // gf::VertexArray line(gf::PrimitiveType::Lines, 2);
    //         // line[0].color = gf::Color::Cyan;
    //         // line[1].color = gf::Color::Cyan;
    //         // line[0].position = gf::Vector2f(m_firstPortal->position.x - ((m_firstPortal->facing % 2 == 1) ? m_firstPortal->width / 2 : 0), m_firstPortal->position.y - ((m_firstPortal->facing % 2 == 0) ? m_firstPortal->width / 2 : 0)) * m_scaleUnit;
    //         // line[1].position = gf::Vector2f(m_firstPortal->position.x + ((m_firstPortal->facing % 2 == 1) ? m_firstPortal->width / 2 : 0), m_firstPortal->position.y + ((m_firstPortal->facing % 2 == 0) ? m_firstPortal->width / 2 : 0)) * m_scaleUnit;

    //         // m_renderer.draw(line);
    //     }
    // }

    // if (m_secondPortal != NULL)
    // {

    //     gf::VertexArray triangle(gf::PrimitiveType::Triangles, 3);
    //     triangle[0].color = gf::Color::Orange;
    //     triangle[1].color = gf::Color::Orange;
    //     triangle[2].color = gf::Color::Orange;

    //     gf::Vector2f startPortal(m_secondPortal->position.x - ((m_secondPortal->facing % 2 == 1) ? m_secondPortal->width / 2 : 0), m_secondPortal->position.y - ((m_secondPortal->facing % 2 == 0) ? m_secondPortal->width / 2 : 0));
    //     gf::Vector2f endPortal(m_secondPortal->position.x + ((m_secondPortal->facing % 2 == 1) ? m_secondPortal->width / 2 : 0), m_secondPortal->position.y + ((m_secondPortal->facing % 2 == 0) ? m_secondPortal->width / 2 : 0));

    //     if (getVisibleSegment(startPortal, endPortal, position, m_player->getAngle(), -1 / std::tan(m_player->getAngle()), position.y - (-1 / std::tan(m_player->getAngle())) * position.x))
    //     {

    //         double startAngle = std::atan2(startPortal.y - position.y, startPortal.x - position.x);
    //         double endAngle = std::atan2(endPortal.y - position.y, endPortal.x - position.x);

    //         double falseDistanceStart = std::sqrt((startPortal.x - position.x) * (startPortal.x - position.x) + (startPortal.y - position.y) * (startPortal.y - position.y));
    //         double falseDistanceEnd = std::sqrt((endPortal.x - position.x) * (endPortal.x - position.x) + (endPortal.y - position.y) * (endPortal.y - position.y));

    //         double realDistanceStart = falseDistanceStart * std::cos(startAngle - m_player->getAngle());
    //         double realDistanceEnd = falseDistanceEnd * std::cos(endAngle - m_player->getAngle());

    //         double heightStart = (realDistanceStart != 0) ? viewHeight / realDistanceStart / 2 : viewHeight * 2;
    //         double heightEnd = (realDistanceEnd != 0) ? viewHeight / realDistanceEnd / 2 : viewHeight * 2;

    //         double xPosStart = std::tan(startAngle - m_player->getAngle()) * viewWidth / 2;
    //         double xPosEnd = std::tan(endAngle - m_player->getAngle()) * viewWidth / 2;

    //         triangle[0].position = gf::Vector2f(viewWidth / 2 + xPosStart, viewHeight / 2 - heightStart / 2);
    //         triangle[1].position = gf::Vector2f(viewWidth / 2 + xPosStart, viewHeight / 2 + heightStart / 2);
    //         triangle[2].position = gf::Vector2f(viewWidth / 2 + xPosEnd, viewHeight / 2 - heightEnd / 2);
    //         m_renderer.draw(triangle);

    //         triangle[0].position = gf::Vector2f(viewWidth / 2 + xPosStart, viewHeight / 2 + heightStart / 2);
    //         triangle[1].position = gf::Vector2f(viewWidth / 2 + xPosEnd, viewHeight / 2 - heightEnd / 2);
    //         triangle[2].position = gf::Vector2f(viewWidth / 2 + xPosEnd, viewHeight / 2 + heightEnd / 2);
    //         m_renderer.draw(triangle);

    //         // gf::VertexArray line(gf::PrimitiveType::Lines, 2);
    //         // line[0].color = gf::Color::Cyan;
    //         // line[1].color = gf::Color::Cyan;
    //         // line[0].position = gf::Vector2f(m_secondPortal->position.x - ((m_firstPortal->facing % 2 == 1) ? m_firstPortal->width / 2 : 0), m_firstPortal->position.y - ((m_firstPortal->facing % 2 == 0) ? m_firstPortal->width / 2 : 0)) * m_scaleUnit;
    //         // line[1].position = gf::Vector2f(m_firstPortal->position.x + ((m_firstPortal->facing % 2 == 1) ? m_firstPortal->width / 2 : 0), m_firstPortal->position.y + ((m_firstPortal->facing % 2 == 0) ? m_firstPortal->width / 2 : 0)) * m_scaleUnit;

    //         // m_renderer.draw(line);
    //     }
    // }
}