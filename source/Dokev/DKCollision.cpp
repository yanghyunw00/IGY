/**
 * \file DKCollision.cpp
 * \brief Collision detection utilities implementation
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "DKCollision.hpp"

namespace Dokev
{
    bool PointInRect(Math::fvec2 point, Math::fvec2 rectPos, Math::fvec2 rectSize)
    {
        return point.x >= rectPos.x && point.x <= rectPos.x + rectSize.x &&
               point.y >= rectPos.y && point.y <= rectPos.y + rectSize.y;
    }

    bool RectOverlap(Math::fvec2 rect1Pos, Math::fvec2 rect1Size,
                     Math::fvec2 rect2Pos, Math::fvec2 rect2Size)
    {
        return rect1Pos.x < rect2Pos.x + rect2Size.x &&
               rect1Pos.x + rect1Size.x > rect2Pos.x &&
               rect1Pos.y < rect2Pos.y + rect2Size.y &&
               rect1Pos.y + rect1Size.y > rect2Pos.y;
    }
}
