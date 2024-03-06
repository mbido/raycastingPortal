#include <iostream>
#include <cmath>

#include <gf/Vector.h>
#include <gf/RenderWindow.h>
#include <gf/Shapes.h>
#include <gf/RenderTexture.h>

#include "Player.hpp"
#include "MapWalls.hpp"
#include "Game3D.hpp"

#define DELTA 0.00001
#define RANGE 0.1

static int nbFirstPortalRendered = 0;
static int nbSecondPortalRendered = 0;

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

    std::vector<gf::Vector2f> blocs = wall.getWallCells();

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
    bool operator()(const std::pair<gf::Vector2f, gf::Vector2f> &a, const std::pair<gf::Vector2f, gf::Vector2f> &b) const
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

bool isPartIncluded(std::vector<gf::Vector2f> subSegments)
{

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

            bool a = (start1.x <= start2.x && start2.x <= end1.x) || std::abs(start1.x - start2.x) < DELTA || std::abs(start2.x - end1.x) < DELTA;
            bool b = (start1.x >= start2.x && start2.x >= end1.x) || std::abs(start1.x - start2.x) < DELTA || std::abs(start2.x - end1.x) < DELTA;
            bool c = (start1.y <= start2.y && start2.y <= end1.y) || std::abs(start1.y - start2.y) < DELTA || std::abs(start2.y - end1.y) < DELTA;
            bool d = (start1.y >= start2.y && start2.y >= end1.y) || std::abs(start1.y - start2.y) < DELTA || std::abs(start2.y - end1.y) < DELTA;

            if ((a || b) && (c || d))
            {
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
                gf::Vector2f intersection((int)(hitPoint.x + 0.5), (int)(hitPoint.y + 0.5));
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
                gf::Vector2f intersection((int)(hitPoint.x + 0.5), (int)(hitPoint.y + 0.5));
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

gf::Vector2f getIntersectionForPortals3D(std::pair<gf::Vector2f, gf::Vector2f> portalSegment, std::pair<gf::Vector2f, gf::Vector2f> segmentToIntersect, gf::Vector2f playerPosition)
{
    bool isPortalVertical = std::abs(portalSegment.first.x - portalSegment.second.x) < DELTA;
    bool isSegmentVertical = std::abs(segmentToIntersect.first.x - segmentToIntersect.second.x) < DELTA;
    bool isPlayerLeft = playerPosition.x < portalSegment.first.x;
    bool isPlayerUp = playerPosition.y < portalSegment.first.y;

    if (isPortalVertical && !isSegmentVertical && (segmentToIntersect.first.x <= portalSegment.first.x + (isPlayerLeft ? DELTA : -DELTA) && segmentToIntersect.second.x >= portalSegment.first.x + (isPlayerLeft ? DELTA : -DELTA) || segmentToIntersect.first.x >= portalSegment.first.x + (isPlayerLeft ? DELTA : -DELTA) && segmentToIntersect.second.x <= portalSegment.first.x + (isPlayerLeft ? DELTA : -DELTA)))
    {
        return gf::Vector2f(portalSegment.first.x, segmentToIntersect.first.y);
    }
    else if (!isPortalVertical && isSegmentVertical && (portalSegment.first.y + (isPlayerUp ? DELTA : -DELTA) <= segmentToIntersect.first.y && portalSegment.second.y + (isPlayerUp ? DELTA : -DELTA) >= segmentToIntersect.first.y || portalSegment.first.y + (isPlayerUp ? DELTA : -DELTA) >= segmentToIntersect.first.y && portalSegment.second.y + (isPlayerUp ? DELTA : -DELTA) <= segmentToIntersect.first.y))
    {
        return gf::Vector2f(segmentToIntersect.first.x, portalSegment.first.y);
    }
    return gf::Vector2f(0, 0);
}

void removeVerticesBefore3D(std::vector<gf::Vector2f> &vertices, std::pair<gf::Vector2f, gf::Vector2f> portalSegment, gf::Vector2f playerPosition)
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

    gf::Vector2f position = m_player->getPosition();
    gf::Vector2f direction = gf::normalize(gf::Vector2f(std::cos(m_player->getAngle()), std::sin(m_player->getAngle())));
    gf::Vector2f hitPoint = castRay(position, direction, m_walls);

    std::vector<std::pair<gf::Vector2f, gf::Vector2f>> segments;
    if (m_walls->getSegments(hitPoint, segments) && !segments.empty())
    {
        auto segment = segments[0];
        bool isVertical = std::abs(segment.first.x - segment.second.x) < DELTA;
        float halfPortalWidth = portal->width / 2;

        portal->position = hitPoint;

        if (isVertical)
        {
            float minY = std::min(segment.first.y, segment.second.y);
            float maxY = std::max(segment.first.y, segment.second.y);

            // Clamping the portal position within the segment boundaries
            portal->position.y = std::max(portal->position.y, minY + halfPortalWidth + 0.01f);
            portal->position.y = std::min(portal->position.y, maxY - halfPortalWidth - 0.01f);

            portal->facing = (position.x < segment.first.x) ? 2 : 0;
        }
        else
        {
            float minX = std::min(segment.first.x, segment.second.x);
            float maxX = std::max(segment.first.x, segment.second.x);

            // Clamping the portal position within the segment boundaries
            portal->position.x = std::max(portal->position.x, minX + halfPortalWidth + 0.01f);
            portal->position.x = std::min(portal->position.x, maxX - halfPortalWidth - 0.01f);

            portal->facing = (position.y < segment.first.y) ? 3 : 1;
        }
    }

    if (otherPortal != NULL)
    {
        portal->linkedPortal = otherPortal;
        otherPortal->linkedPortal = portal;
    }
}

std::vector<gf::Vector2f> getPointsBetween(gf::Vector2f start, gf::Vector2f end)
{
    std::vector<gf::Vector2f> points;

    if (std::abs(start.x - end.x) < DELTA * 10)
    {
        // vertical segment
        float a = std::min(start.y, end.y);
        float b = std::max(start.y, end.y);
        int a1 = std::floor(a) + 1;
        int b1 = std::floor(b);

        points.push_back(gf::Vector2f(start.x, a));

        for (int i = a1; i <= b1; i++)
        {
            points.push_back(gf::Vector2f(start.x, i));
        }

        if (b - b1 > DELTA) // is b a whole number ?
        {
            points.push_back(gf::Vector2f(start.x, b));
        }
    }
    else
    {
        // horizontal segment
        float a = std::min(start.x, end.x);
        float b = std::max(start.x, end.x);
        int a1 = std::floor(a) + 1;
        int b1 = std::floor(b);

        points.push_back(gf::Vector2f(a, start.y));

        for (int i = a1; i <= b1; i++)
        {
            points.push_back(gf::Vector2f(i, start.y));
        }

        if (b - b1 > DELTA) // is b a whole number ?
        {
            points.push_back(gf::Vector2f(b, start.y));
        }
    }

    return points;
}

void renderAWallTextured(gf::Vector2f start, gf::Vector2f end, std::vector<gf::Vector2f> trapezeVertices, const gf::Texture &texture, gf::RenderTarget &renderer)
{

    bool isSegmentVertical = std::abs(start.x - end.x) < DELTA;
    // trapezeVertices is a vector of the trapeze's vertices from left to right, top to bottom

    // lets determine the center of the texture on the trapeze
    // => the center is the intersection of the diagonals of the trapeze
    gf::Vector2f center = linesIntersection(trapezeVertices[0], trapezeVertices[3], trapezeVertices[1], trapezeVertices[2]);
    gf::Vector2f topCenter = linesIntersection(center, gf::Vector2f(center.x, center.y + 1), trapezeVertices[0], trapezeVertices[1]);
    gf::Vector2f bottomCenter = linesIntersection(center, gf::Vector2f(center.x, center.y + 1), trapezeVertices[2], trapezeVertices[3]);

    if (center == gf::Vector2f(0.0f, 0.0f) || topCenter == gf::Vector2f(0.0f, 0.0f) || bottomCenter == gf::Vector2f(0.0f, 0.0f))
    {
        return;
    }

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

    gf::RenderStates states;
    states.texture[0] = &texture;

    renderer.draw(trapeze, states);
}

void renderBlankPortal(bool isFirstPortal, gf::Vector2f startPortal, gf::Vector2f endPortal, gf::Vector2f position, double angle, gf::RenderTarget &renderer)
{
    gf::Vector2f viewSize = renderer.getView().getSize();
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
        renderer.draw(trapezoid);
    }
}
void renderHelper(const Game3D &game, gf::RenderTarget &renderer, bool isPortal = false, struct portal *portal = NULL);
void renderRenderedPortal(bool isFirstPortal, gf::Vector2f startPortal, gf::Vector2f endPortal, gf::Vector2f position, gf::Vector2f casterPosition, double angle, gf::RenderTarget &renderer, const Game3D &game)
{
    gf::Vector2f viewSize = renderer.getView().getSize();
    double viewWidth = viewSize.x;
    double viewHeight = viewSize.y;

    bool isSegmentVertical = std::abs(startPortal.x - endPortal.x) < DELTA;
    auto color = (isFirstPortal) ? gf::Color::Cyan : gf::Color::Orange;

    // if one of the end point or start point is behind the player, we do not draw it
    if (getVisibleSegment(startPortal, endPortal, casterPosition, angle, -1 / std::tan(angle), position.y - (-1 / std::tan(angle)) * position.x))
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

        gf::VertexArray trapezoid(gf::PrimitiveType::Triangles, 6);
        trapezoid[0].position = gf::Vector2f(viewWidth / 2 + xPosStart, viewHeight / 2 - heightStart / 2);
        trapezoid[0].texCoords = trapezoid[0].position / viewSize;
        trapezoid[1].position = gf::Vector2f(viewWidth / 2 + xPosStart, viewHeight / 2 + heightStart / 2);
        trapezoid[1].texCoords = trapezoid[1].position / viewSize;
        trapezoid[2].position = gf::Vector2f(viewWidth / 2 + xPosEnd, viewHeight / 2 - heightEnd / 2);
        trapezoid[2].texCoords = trapezoid[2].position / viewSize;
        trapezoid[3].position = gf::Vector2f(viewWidth / 2 + xPosStart, viewHeight / 2 + heightStart / 2);
        trapezoid[3].texCoords = trapezoid[3].position / viewSize;
        trapezoid[4].position = gf::Vector2f(viewWidth / 2 + xPosEnd, viewHeight / 2 - heightEnd / 2);
        trapezoid[4].texCoords = trapezoid[4].position / viewSize;
        trapezoid[5].position = gf::Vector2f(viewWidth / 2 + xPosEnd, viewHeight / 2 + heightEnd / 2);
        trapezoid[5].texCoords = trapezoid[5].position / viewSize;

        // render the scene in a texture
        gf::RenderTexture renderTexture(renderer.getSize());
        renderTexture.setActive();

        gf::RectangleShape rectangle(renderer.getSize());
        rectangle.setColor(gf::Color::Black);
        renderTexture.draw(rectangle);

        renderHelper(game, renderTexture, true, isFirstPortal ? game.m_secondPortal : game.m_firstPortal);
        renderTexture.display();

        renderer.setActive();

        gf::RenderStates states;
        states.texture[0] = &(renderTexture.getTexture());
        renderer.draw(trapezoid, states);
    }
}

void resizePortal(gf::Vector2f &portalStart, gf::Vector2f &portalEnd, gf::Vector2f segmentStart, gf::Vector2f segmentEnd)
{
    bool isSegmentVertical = std::abs(segmentStart.x - segmentEnd.x) < DELTA;
    int index = (isSegmentVertical) ? 1 : 0;

    portalStart[index] = std::max(portalStart[index], std::min(segmentStart[index], segmentEnd[index]));
    portalEnd[index] = std::min(portalEnd[index], std::max(segmentStart[index], segmentEnd[index]));
}

bool isBetween(gf::Vector2f point, gf::Vector2f start, gf::Vector2f end)
{
    bool isSegmentVertical = std::abs(start.x - end.x) < DELTA;
    int index = (isSegmentVertical) ? 0 : 1;
    int index2 = (index + 1) % 2;
    bool isOnSegment = std::abs(point[index] - start[index]) < DELTA;
    return point[index2] > std::min(start[index2], end[index2]) && point[index2] < std::max(start[index2], end[index2]);
}

void renderCrossHair(gf::RenderTarget &renderer)
{
    gf::Vector2f viewSize = renderer.getView().getSize();
    gf::VertexArray crossHair(gf::PrimitiveType::Lines, 4);
    crossHair[0].position = gf::Vector2f(viewSize.x / 2 - 10, viewSize.y / 2);
    crossHair[1].position = gf::Vector2f(viewSize.x / 2 + 10, viewSize.y / 2);
    crossHair[2].position = gf::Vector2f(viewSize.x / 2, viewSize.y / 2 - 10);
    crossHair[3].position = gf::Vector2f(viewSize.x / 2, viewSize.y / 2 + 10);
    crossHair[0].color = gf::Color::White;
    crossHair[1].color = gf::Color::White;
    crossHair[2].color = gf::Color::White;
    crossHair[3].color = gf::Color::White;
    renderer.draw(crossHair);
}

void castSegments(std::map<std::pair<gf::Vector2f, gf::Vector2f>, std::vector<gf::Vector2f>, PairComparator> &segments, gf::Vector2f position, std::vector<gf::Vector2f> sortedVertices, const Game3D &game)
{
    for (int i = 0; i < sortedVertices.size(); i++)
    {
        gf::Vector2f direction = gf::normalize(gf::Vector2f(sortedVertices[i].x - position.x, sortedVertices[i].y - position.y));

        gf::Vector2f endPoint;

        endPoint = castRay(position, direction, game.m_walls);
        double dist = std::sqrt((sortedVertices[i].x - position.x) * (sortedVertices[i].x - position.x) + (sortedVertices[i].y - position.y) * (sortedVertices[i].y - position.y));

        if (std::abs(sortedVertices[i].x - endPoint.x) < DELTA && std::abs(sortedVertices[i].y - endPoint.y) < DELTA)
        {
            // we hit the vertex aimed

            std::vector<std::pair<gf::Vector2f, gf::Vector2f>> segmentsHit;
            if (game.m_walls->getSegments(sortedVertices[i], segmentsHit))
            {
                for (auto segment : segmentsHit)
                {
                    segments[segment].push_back(sortedVertices[i]);
                }
            }

            // send a new ray if needed :
            // we know that the vertex hit is a corner of a wall
            // we check how many walls are connected to this corner
            // if there is 1 wall AND we are not opposite to the wall we hit
            //       we send a new ray to the next wall

            int nbWalls = 0;
            gf::Vector2f cells[] = {
                gf::Vector2f(sortedVertices[i].x - 1, sortedVertices[i].y - 1),
                gf::Vector2f(sortedVertices[i].x - 1, sortedVertices[i].y),
                gf::Vector2f(sortedVertices[i].x, sortedVertices[i].y - 1),
                gf::Vector2f(sortedVertices[i].x, sortedVertices[i].y)};

            for (const auto &cell : cells)
            {
                if (game.m_walls->getTile(cell.x, cell.y) != 0)
                {
                    nbWalls++;
                }
            }

            if (nbWalls == 1)
            {
                // we check if we are opposing the vertex :
                gf::Vector2f checkPosition(endPoint.x + 0.0001 * direction.x, endPoint.y + 0.0001 * direction.y);
                gf::Vector2f checkCell((int)checkPosition.x, (int)checkPosition.y);
                if (game.m_walls->getTile(checkCell.x, checkCell.y) == 0)
                {
                    gf::Vector2f newStartPoint(endPoint.x + 0.0001 * direction.x, endPoint.y + 0.0001 * direction.y);
                    gf::Vector2f newEndPoint = castRay(newStartPoint, direction, game.m_walls);

                    if (game.m_walls->getSegments(newEndPoint, segmentsHit))
                    {
                        for (auto segment : segmentsHit)
                        {
                            segments[segment].push_back(newEndPoint);
                        }
                    }
                }
            }
        }
    }
}

void renderSegments(const std::map<std::pair<gf::Vector2f, gf::Vector2f>, std::vector<gf::Vector2f>, PairComparator> &segments, std::pair<gf::Vector2f, gf::Vector2f> firstPortalSegment, std::pair<gf::Vector2f, gf::Vector2f> secondPortalSegment, const Game3D &game, gf::RenderTarget &renderer, gf::Vector2f position, gf::Vector2f casterPosition, double angle)
{
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
        bool isSegmentRightToPlayer = segment.first.first.x > position.x;
        bool isSegmentUpToPlayer = segment.first.first.y < position.y;

        auto color = (isSegmentVertical) ? gf::Color::fromRgba32(0x77FF77FF) : gf::Color::fromRgba32(0x7777FFFF);

        for (std::size_t i = 0; i < segment.second.size() - 1; i += 2)
        {

            if (segment.second.size() < 2)
            {
                continue;
            }
            std::vector<gf::Vector2f> points = getPointsBetween(segment.second[i], segment.second[i + 1]);

            for (size_t i = 0; i < points.size() - 1; i++)
            {

                gf::Vector2f start = points[i];
                gf::Vector2f end = points[i + 1];

                if (std::abs((start - end).x) < DELTA && std::abs((start - end).y) < DELTA)
                {
                    continue;
                }

                if (angle == 0 || angle == gf::Pi || angle == 2 * gf::Pi || angle == -gf::Pi)
                {
                    angle += DELTA / 2;
                }
                double a = -1 / std::tan(angle);
                double b = position.y - a * position.x;
                // skip the segment if it is behind the player if needed
                if (!getVisibleSegment(start, end, casterPosition, angle, a, b))
                {
                    continue;
                }

                gf::Vector2f viewSize = renderer.getView().getSize();
                double viewWidth = viewSize.x;
                double viewHeight = viewSize.y;

                // --- For the first point of the segment ---
                double rayAngle1 = std::atan2(start.y - position.y, start.x - position.x);
                double falseDistance1 = std::sqrt((start.x - position.x) * (start.x - position.x) + (start.y - position.y) * (start.y - position.y));
                double realDistance1 = falseDistance1 * std::cos(rayAngle1 - angle);
                double height1 = (realDistance1 != 0) ? viewHeight / realDistance1 / 2 : viewHeight * 2;
                double xPos1 = std::tan(rayAngle1 - angle) * viewWidth / 2;

                gf::Vector2f A(viewWidth / 2 + xPos1, viewHeight / 2 - height1 / 2);
                gf::Vector2f C(viewWidth / 2 + xPos1, viewHeight / 2 + height1 / 2);

                // --- For the second point of the segment ---
                double rayAngle2 = std::atan2(end.y - position.y, end.x - position.x);
                double falseDistance2 = std::sqrt((end.x - position.x) * (end.x - position.x) + (end.y - position.y) * (end.y - position.y));
                double realDistance2 = falseDistance2 * std::cos(rayAngle2 - angle);
                double height2 = (realDistance2 != 0) ? viewHeight / realDistance2 / 2 : viewHeight;
                double xPos2 = std::tan(rayAngle2 - angle) * viewWidth / 2;

                gf::Vector2f B(viewWidth / 2 + xPos2, viewHeight / 2 - height2 / 2);
                gf::Vector2f D(viewWidth / 2 + xPos2, viewHeight / 2 + height2 / 2);

                renderAWallTextured(start, end, {A, B, C, D}, game.m_texture, renderer);
            }
            // render the portals if it is on that segment :
            int index = (isSegmentVertical) ? 1 : 0;
            if (game.m_firstPortal != NULL && firstPortalSegment == segment.first)
            {
                gf::Vector2f startPortal(game.m_firstPortal->position.x - ((game.m_firstPortal->facing % 2 == 1) ? game.m_firstPortal->width / 2 : 0), game.m_firstPortal->position.y - ((game.m_firstPortal->facing % 2 == 0) ? game.m_firstPortal->width / 2 : 0));
                gf::Vector2f endPortal(game.m_firstPortal->position.x + ((game.m_firstPortal->facing % 2 == 1) ? game.m_firstPortal->width / 2 : 0), game.m_firstPortal->position.y + ((game.m_firstPortal->facing % 2 == 0) ? game.m_firstPortal->width / 2 : 0));

                if (segment.second.size() > 1 && game.m_firstPortal != NULL && firstPortalSegment == segment.first && (isBetween(startPortal, segment.second[i], segment.second[i + 1]) || isBetween(endPortal, segment.second[i], segment.second[i + 1])))
                {
                    resizePortal(startPortal, endPortal, segment.second[i], segment.second[i + 1]);
                    if (true || !game.m_secondPortal)
                    { // the portal are not linked so we render the portal as a blank portal
                        renderBlankPortal(true, startPortal, endPortal, position, angle, renderer);
                    }
                    else
                    { // TODO : render the portal as a linked portal
                        nbSecondPortalRendered++;
                        renderRenderedPortal(true, startPortal, endPortal, position, casterPosition, angle, renderer, game);
                    }
                }
            }

            if (game.m_secondPortal != NULL && secondPortalSegment == segment.first)
            {
                gf::Vector2f startPortal(game.m_secondPortal->position.x - ((game.m_secondPortal->facing % 2 == 1) ? game.m_secondPortal->width / 2 : 0), game.m_secondPortal->position.y - ((game.m_secondPortal->facing % 2 == 0) ? game.m_secondPortal->width / 2 : 0));
                gf::Vector2f endPortal(game.m_secondPortal->position.x + ((game.m_secondPortal->facing % 2 == 1) ? game.m_secondPortal->width / 2 : 0), game.m_secondPortal->position.y + ((game.m_secondPortal->facing % 2 == 0) ? game.m_secondPortal->width / 2 : 0));

                if (segment.second.size() > 1 && game.m_secondPortal != NULL && secondPortalSegment == segment.first && (isBetween(startPortal, segment.second[i], segment.second[i + 1]) || isBetween(endPortal, segment.second[i], segment.second[i + 1])))
                {
                    resizePortal(startPortal, endPortal, segment.second[i], segment.second[i + 1]);
                    if (true || !game.m_firstPortal)
                    { // the portal are not linked so we render the portal as a blank portal
                        renderBlankPortal(false, startPortal, endPortal, position, angle, renderer);
                    }
                    else
                    { // TODO : render the portal as a linked portal
                        nbFirstPortalRendered++;
                        renderRenderedPortal(false, startPortal, endPortal, position, casterPosition, angle, renderer, game);
                    }
                }
            }
        }
    }
}

gf::Vector2f getRendererPosition(struct portal *fromPortal, struct portal *toPortal, gf::Vector2f position)
{
    // determine the rotation of the portals
    float delta = -std::fmod(gf::Pi / 2 * (toPortal->facing - fromPortal->facing), 2 * gf::Pi) + gf::Pi;
    float cosDelta = (float)std::cos(delta);
    float sinDelta = (float)std::sin(delta);

    // the matrix of rotation
    gf::Matrix2f rotation(cosDelta, -sinDelta, sinDelta, cosDelta);
    gf::Vector2f rendererPosition = rotation * (position - fromPortal->position) + toPortal->position;

    return rendererPosition;
}

double getRendererAngle(struct portal *fromPortal, struct portal *toPortal, double angle)
{
    // determine the rotation of the portals
    float delta = -std::fmod(gf::Pi / 2 * (toPortal->facing - fromPortal->facing), 2 * gf::Pi) + gf::Pi;
    return angle + delta;
}

void renderHelper(const Game3D &game, gf::RenderTarget &renderer, bool isPortal, struct portal *portal)
{
    // getting the segments where the portal are on :
    std::pair<gf::Vector2f, gf::Vector2f> firstPortalSegment(gf::Vector2f(0, 0), gf::Vector2f(0, 0));
    std::vector<std::pair<gf::Vector2f, gf::Vector2f>> segmentsP;
    if (game.m_firstPortal != NULL && game.m_walls->getSegments(game.m_firstPortal->position, segmentsP))
    {

        firstPortalSegment = segmentsP[0];
    }
    std::pair<gf::Vector2f, gf::Vector2f> secondPortalSegment(gf::Vector2f(0, 0), gf::Vector2f(0, 0));
    if (game.m_secondPortal != NULL && game.m_walls->getSegments(game.m_secondPortal->position, segmentsP))
    {
        secondPortalSegment = segmentsP[0];
    }

    // getting the two positions, one for the caster, the other for the renderer
    gf::Vector2f casterPosition;
    gf::Vector2f rendererPosition;
    double angle;
    if (isPortal)
    {
        if (game.m_firstPortal == portal)
        {
            if (nbFirstPortalRendered > 1)
            {
                return;
            }
            angle = getRendererAngle(game.m_secondPortal, game.m_firstPortal, game.m_player->getAngle());
            rendererPosition = getRendererPosition(game.m_secondPortal, game.m_firstPortal, game.m_player->getPosition());
            casterPosition = game.m_secondPortal->position;
            // offset the caster position to avoid being stuck in the wall
            double d = game.m_secondPortal->facing * gf::Pi / 2;
            gf::Vector2f offset = gf::Vector2f(std::cos(d), std::sin(d)) * 0.01f;
            casterPosition = casterPosition + offset;
        }
        else
        {
            if (nbSecondPortalRendered > 1)
            {
                return;
            }
            angle = getRendererAngle(game.m_firstPortal, game.m_secondPortal, game.m_player->getAngle());
            rendererPosition = getRendererPosition(game.m_firstPortal, game.m_secondPortal, game.m_player->getPosition());
            casterPosition = game.m_firstPortal->position;
            // offset the caster position to avoid being stuck in the wall
            double d = game.m_firstPortal->facing * gf::Pi / 2;
            gf::Vector2f offset = gf::Vector2f(std::cos(d), std::sin(d)) * 0.01f;
            casterPosition = casterPosition + offset;
        }
    }
    else
    {
        casterPosition = game.m_player->getPosition();
        rendererPosition = game.m_player->getPosition();
        angle = game.m_player->getAngle();
    }

    std::vector<Wall> walls = game.m_walls->getWalls();
    std::map<std::pair<gf::Vector2f, gf::Vector2f>, std::vector<gf::Vector2f>, PairComparator> segments;
    std::vector<gf::Vector2f> sortedVertices = game.m_walls->getVertices();

    // to see if needed but for now, i think it is not

    // !!! IL FAUT CHANGER L'ORDRE !!!
    if (isPortal && game.m_player->getAngle() == std::fmod(portal->facing * gf::Pi / 2 + gf::Pi, 2 * gf::Pi))
    {
        std::sort(sortedVertices.begin(), sortedVertices.end(), CompareVerticesAngleReverse(casterPosition));
    }
    else
    {
        std::sort(sortedVertices.begin(), sortedVertices.end(), CompareVerticesAngle(rendererPosition));
    }

    castSegments(segments, casterPosition, sortedVertices, game);

    // render the segments :
    renderSegments(segments, firstPortalSegment, secondPortalSegment, game, renderer, rendererPosition, casterPosition, angle);

    if (!isPortal)
    {
        renderCrossHair(renderer);
    }
}

void Game3D::render()
{
    std::cout << std::endl;
    nbFirstPortalRendered = 0;
    nbSecondPortalRendered = 0;
    renderHelper(*this, m_renderer);
}