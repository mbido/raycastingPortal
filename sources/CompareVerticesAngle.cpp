#include <iostream>
#include <algorithm>
#include <vector>

#include "CompareVerticesAngle.hpp"

bool CompareVerticesAngle::operator()(const gf::Vector2f& a, const gf::Vector2f& b)
{
    gf::Vector2f aRel = a - playerPosition;
    gf::Vector2f bRel = b - playerPosition;

    float angleA;
    float angleB;

    if(aRel.x == 0 && aRel.y == 0)
    {
        angleA = 0;
    }
    else
    {
        angleA = atan2(aRel.y, aRel.x);
    }
    
    if(bRel.x == 0 && bRel.y == 0)
    {
        angleB = 0;
    }
    else
    {
        angleB = atan2(bRel.y, bRel.x);
    }
    
    return angleA < angleB;
}
