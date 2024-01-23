#include <iostream>

#include <gf/Vector.h>
#include <gf/RenderWindow.h>
#include <gf/Shapes.h>

#include "Player.hpp"
#include "MapWalls.hpp"
#include "Game2D.hpp"
#include "Game3D.hpp"

#include <iomanip>

#define DELTA 0.00001
#define RENDER_BEHIND_PLAYER false
#define RANGE 0.09

// clamp function used for collisions
float clamp(float min, float max, float value)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

gf::Vector2f checkingCollisionsForEachWall(gf::Vector2f closestPointOfTheWall, gf::Vector2f p_position)
{
    float distanceBetweenWallAndPlayer = std::sqrt((p_position.x - closestPointOfTheWall.x) * (p_position.x - closestPointOfTheWall.x) + (p_position.y - closestPointOfTheWall.y) * (p_position.y - closestPointOfTheWall.y));
    if (RANGE > distanceBetweenWallAndPlayer)
    {
        gf::Vector2f coordToAddToPos = gf::Vector2f(p_position.x - closestPointOfTheWall.x, p_position.y - closestPointOfTheWall.y);
        coordToAddToPos = (coordToAddToPos / distanceBetweenWallAndPlayer) * (RANGE - distanceBetweenWallAndPlayer);
        return coordToAddToPos;
    }
    return gf::Vector2f(0.0f, 0.0f);
}

gf::Vector2f getClosestPointOfWall(Wall wall, gf::Vector2f p_position)
{
    gf::Vector2f closestPointOfTheWall = gf::Vector2f(0.0f, 0.0f);

    std::vector<gf::Vector2i> blocs = wall.getOccupiedCells();
    for (auto bloc : blocs)
    {
        float x = clamp((float)bloc.x, (float)bloc.x + 1, p_position.x);
        float y = clamp((float)bloc.y, (float)bloc.y + 1, p_position.y);
        if ((p_position.x - x) * (p_position.x - x) + (p_position.y - y) * (p_position.y - y) < (p_position.x - closestPointOfTheWall.x) * (p_position.x - closestPointOfTheWall.x) + (p_position.y - closestPointOfTheWall.y) * (p_position.y - closestPointOfTheWall.y))
        {
            closestPointOfTheWall = gf::Vector2f(x, y);
        }
    }

    return closestPointOfTheWall;
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

std::map<std::pair<gf::Vector2i, gf::Vector2i>, std::vector<gf::Vector2i>, PairComparator> segments;

void Game2D::update(gf::Time dt)
{
    m_player->update(dt);

    m_windowSize = m_renderer.getSize();
    m_scaleUnit = std::min((int)(m_windowSize[0] / m_walls->getNbRows()), (int)(m_windowSize[1] / m_walls->getNbColumns()));

    for (auto wall : m_walls->getWalls())
    {
        gf::Vector2f newPos = checkingCollisionsForEachWall(getClosestPointOfWall(wall, m_player->getPosition()), m_player->getPosition());
        m_player->setPosition(m_player->getPosition() + newPos);
    }

    
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
                if (m_walls->getTile(intersection.x, intersection.y) == 1 ||
                    m_walls->getTile(intersection.x - 1, intersection.y) == 1 ||
                    m_walls->getTile(intersection.x, intersection.y - 1) == 1 ||
                    m_walls->getTile(intersection.x - 1, intersection.y - 1) == 1)
                {
                    return intersection;
                }
            }

            if (m_walls->getTile(tileX, tileY) == 1)
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
                if (m_walls->getTile(intersection.x, intersection.y) == 1 ||
                    m_walls->getTile(intersection.x - 1, intersection.y) == 1 ||
                    m_walls->getTile(intersection.x, intersection.y - 1) == 1 ||
                    m_walls->getTile(intersection.x - 1, intersection.y - 1) == 1)
                {
                    return intersection;
                }
            }

            if (m_walls->getTile(tileX, tileY) == 1)
            {
                return hitPoint;
            }
            distY += unitY;
        }
    }
    return position + 100 * direction;
}

bool Game2D::isPartIncluded(std::vector<gf::Vector2f> subSegments)
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
bool Game2D::getVisibleSegment(gf::Vector2f &start, gf::Vector2f &end, gf::Vector2f playerPosition, double playerAngle, double a, double b)
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
    }
    else
    {
        if (a == 0) // !!! have to check if even possible
            a = DELTA / 2;
        intersectionPoint = gf::Vector2f((start.y - b) / a, start.y);
    }
    start = visiblePoint;
    end = intersectionPoint;
    return true;
}

gf::Vector2f getIntersectionForPortals(std::pair<gf::Vector2i, gf::Vector2i> portalSegment, std::pair<gf::Vector2i, gf::Vector2i> segmentToIntersect, gf::Vector2f playerPosition)
{
    bool isPortalVertical = std::abs(portalSegment.first.x - portalSegment.second.x) < DELTA;
    bool isSegmentVertical = std::abs(segmentToIntersect.first.x - segmentToIntersect.second.x) < DELTA;
    bool isPlayerLeftOrUp = playerPosition.x < portalSegment.first.x || playerPosition.y < portalSegment.first.y;
    if (isPortalVertical && !isSegmentVertical && (segmentToIntersect.first.x < portalSegment.first.x && segmentToIntersect.second.x > portalSegment.first.x || segmentToIntersect.first.x > portalSegment.first.x && segmentToIntersect.second.x < portalSegment.first.x))
    {
        return gf::Vector2f(portalSegment.first.x + (isPlayerLeftOrUp ? DELTA : -DELTA), segmentToIntersect.first.y);
    }
    else if (!isPortalVertical && isSegmentVertical && (portalSegment.first.y < segmentToIntersect.first.y && portalSegment.second.y > segmentToIntersect.first.y || portalSegment.first.y > segmentToIntersect.first.y && portalSegment.second.y < segmentToIntersect.first.y))
    {
        return gf::Vector2f(segmentToIntersect.first.x, portalSegment.first.y + (isPlayerLeftOrUp ? DELTA : -DELTA));
    }
    return gf::Vector2f(0, 0);
}

void removeVerticesBefore(std::vector<gf::Vector2f> &vertices, std::pair<gf::Vector2i, gf::Vector2i> portalSegment, gf::Vector2f playerPosition)
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

void Game2D::castPortal(bool isFirstPortal)
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
    gf::Vector2f hitPoint = castRay2D(position, direction, m_walls);

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

    std::cout << "portal position : (" << portal->position.x << ", " << portal->position.y << ")" << std::endl;
}

void Game2D::render(bool isPortal, std::pair<gf::Vector2i, gf::Vector2i> portalSegment)
{

    std::vector<Wall> walls = m_walls->getWalls();
    std::map<std::pair<gf::Vector2i, gf::Vector2i>, std::vector<gf::Vector2f>, PairComparator> segments;

    // int nbSupplementaryRays = 0;

    gf::Vector2f position = m_player->getPosition();
    position.x = position.x + DELTA;
    position.y = position.y + 2 * DELTA;

    // getting the vertices of the walls sorted by angle :
    std::vector<gf::Vector2i> sV = m_walls->getVertices();
    std::vector<gf::Vector2f> sortedVertices;
    for (auto v : sV)
    {
        sortedVertices.push_back(gf::Vector2f(v.x, v.y));
    }

    // adding vertices at begins and ends of portals
    // if (m_firstPortal != NULL)
    // {
    //     gf::Vector2f startPortal1(m_firstPortal->position.x - ((m_firstPortal->facing % 2 == 1) ? m_firstPortal->width / 2 : 0), m_firstPortal->position.y - ((m_firstPortal->facing % 2 == 0) ? m_firstPortal->width / 2 : 0));
    //     gf::Vector2f endPortal1(m_firstPortal->position.x + ((m_firstPortal->facing % 2 == 1) ? m_firstPortal->width / 2 : 0), m_firstPortal->position.y + ((m_firstPortal->facing % 2 == 0) ? m_firstPortal->width / 2 : 0));
    //     sortedVertices.push_back(startPortal1);
    //     sortedVertices.push_back(endPortal1);
    // }
    // if (m_secondPortal != NULL)
    // {
    //     gf::Vector2f startPortal2(m_secondPortal->position.x - ((m_secondPortal->facing % 2 == 1) ? m_secondPortal->width / 2 : 0), m_secondPortal->position.y - ((m_secondPortal->facing % 2 == 0) ? m_secondPortal->width / 2 : 0));
    //     gf::Vector2f endPortal2(m_secondPortal->position.x + ((m_secondPortal->facing % 2 == 1) ? m_secondPortal->width / 2 : 0), m_secondPortal->position.y + ((m_secondPortal->facing % 2 == 0) ? m_secondPortal->width / 2 : 0));
    //     sortedVertices.push_back(startPortal2);
    //     sortedVertices.push_back(endPortal2);
    // }

    // if we are in a portal, we add a vertex at every intersection between a segment and the line defined by the segment of the portal
    // then we remove every vertices that are before that line
    if (isPortal)
    {
        // adding the vertices :
        std::set<std::pair<gf::Vector2i, gf::Vector2i>, PairComparator> seenSegments;
        for (auto v : sortedVertices)
        {
            std::vector<std::pair<gf::Vector2i, gf::Vector2i>> vertexSegments;
            if (m_walls->getSegments(v, vertexSegments))
            {
                for (auto segment : vertexSegments)
                {
                    if (seenSegments.insert(segment).second)
                    {
                        gf::Vector2f intersection = getIntersectionForPortals(portalSegment, segment, m_player->getPosition());
                        if (intersection != gf::Vector2f(0, 0))
                        {
                            sortedVertices.push_back(gf::Vector2f(intersection.x, intersection.y));
                            // rendering a small circle
                            double radius = 3.0f;
                            gf::CircleShape circle(radius * 2);
                            circle.setPosition(intersection * m_scaleUnit - gf::Vector2f(radius, radius));
                            circle.setColor(gf::Color::Red);
                            m_renderer.draw(circle);
                        }
                    }
                }
            }
        }
        // removing the vertices :
        removeVerticesBefore(sortedVertices, portalSegment, m_player->getPosition());
        for (auto v : sortedVertices)
        {
            double radius = 3.0f;
            gf::CircleShape circle(radius * 2);
            circle.setPosition(v * m_scaleUnit - gf::Vector2f(radius, radius));
            circle.setColor(gf::Color::White);
            m_renderer.draw(circle);
        }
    }
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
                if (m_walls->getTile(cell.x, cell.y) == 1)
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
            double angle = m_player->getAngle();
            if (angle == 0 || angle == gf::Pi || angle == 2 * gf::Pi || angle == -gf::Pi)
            {
                angle += DELTA / 2;
            }
            gf::Vector2f pos = m_player->getPosition();
            double a = -1 / std::tan(angle);
            double b = pos.y - a * pos.x;
            // skip the segment if it is behind the player if needed
            if (!RENDER_BEHIND_PLAYER && !getVisibleSegment(segment.second[i], segment.second[i + 1], pos, angle, a, b))
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

    // render the portals :
    if (m_firstPortal != NULL)
    {
        gf::VertexArray line(gf::PrimitiveType::Lines, 2);
        line[0].color = gf::Color::Cyan;
        line[1].color = gf::Color::Cyan;
        line[0].position = gf::Vector2f(m_firstPortal->position.x - ((m_firstPortal->facing % 2 == 1) ? m_firstPortal->width / 2 : 0), m_firstPortal->position.y - ((m_firstPortal->facing % 2 == 0) ? m_firstPortal->width / 2 : 0)) * m_scaleUnit;
        line[1].position = gf::Vector2f(m_firstPortal->position.x + ((m_firstPortal->facing % 2 == 1) ? m_firstPortal->width / 2 : 0), m_firstPortal->position.y + ((m_firstPortal->facing % 2 == 0) ? m_firstPortal->width / 2 : 0)) * m_scaleUnit;

        m_renderer.draw(line);
        // std::cout << "first portal : (" << line[0].position.x << ", " << line[0].position.y << ") -> (" << line[1].position.x << ", " << line[1].position.y << ")" << std::endl;
    }
    // else
    // {
    //     std::cout << "first portal : NULL" << std::endl;
    // }

    if (m_secondPortal != NULL)
    {
        gf::VertexArray line(gf::PrimitiveType::Lines, 2);
        line[0].color = gf::Color::Orange;
        line[1].color = gf::Color::Orange;
        line[0].position = gf::Vector2f(m_secondPortal->position.x - ((m_secondPortal->facing % 2 == 1) ? m_secondPortal->width / 2 : 0), m_secondPortal->position.y - ((m_secondPortal->facing % 2 == 0) ? m_secondPortal->width / 2 : 0)) * m_scaleUnit;
        line[1].position = gf::Vector2f(m_secondPortal->position.x + ((m_secondPortal->facing % 2 == 1) ? m_secondPortal->width / 2 : 0), m_secondPortal->position.y + ((m_secondPortal->facing % 2 == 0) ? m_secondPortal->width / 2 : 0)) * m_scaleUnit;

        m_renderer.draw(line);
        // std::cout << "second portal : (" << line[0].position.x << ", " << line[0].position.y << ") -> (" << line[1].position.x << ", " << line[1].position.y << ")" << std::endl;
    }
    // else
    // {
    //     std::cout << "second portal : NULL" << std::endl;
    // }

    // for (auto wall : m_walls->getWalls()) {
    //     gf::VertexArray line(gf::PrimitiveType::Lines, 2);
    //     line[0].position = getClosestPointOfWall(wall, m_player->getPosition()) * m_scaleUnit;
    //     line[1].position = m_player->getPosition() * m_scaleUnit;
    //     line[0].color = gf::Color::Cyan;
    //     line[1].color = gf::Color::Cyan;
    //     m_renderer.draw(line);

    //     gf::CircleShape circle(m_scaleUnit / 16);
    //     circle.setPosition(line[0].position);
    //     circle.setColor(gf::Color::White);
    //     circle.setAnchor(gf::Anchor::Center);
    //     m_renderer.draw(circle);
    // }
}