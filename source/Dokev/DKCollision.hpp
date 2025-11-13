/**
 * \file DKCollision.hpp
 * \brief Collision detection utilities
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once

#include "Engine/Vec2.hpp"

namespace Dokev
{

    bool PointInRect(Math::fvec2 point, Math::fvec2 rectPos, Math::fvec2 rectSize);

    bool RectOverlap(Math::fvec2 rect1Pos, Math::fvec2 rect1Size,
                     Math::fvec2 rect2Pos, Math::fvec2 rect2Size);
}
