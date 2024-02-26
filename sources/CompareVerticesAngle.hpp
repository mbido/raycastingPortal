#ifndef COMPARE_HPP
#define COMPARE_HPP

#include <gf/Vector.h>
#include <gf/RenderWindow.h>
#include <gf/Shapes.h>

#include <vector>

/**
 * @brief A class made to compare two vertices' positions in order to sort them by angle relative
 * to the player's position.
 */
class CompareVerticesAngle
{
public:
    CompareVerticesAngle(const gf::Vector2f &playerPos)
        : playerPosition(playerPos)
    {
    }

    bool operator()(const gf::Vector2f &a, const gf::Vector2f &b);

private:
    const gf::Vector2f &playerPosition;
};

class CompareVerticesAngleReverse
{
public:
    CompareVerticesAngleReverse(const gf::Vector2f &playerPos)
        : playerPosition(playerPos)
    {
    }

    bool operator()(const gf::Vector2f &a, const gf::Vector2f &b);

private:
    const gf::Vector2f &playerPosition;
};

#endif // COMPARE_HPP