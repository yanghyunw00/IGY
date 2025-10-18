/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include "Vec2.hpp"
#include <algorithm>

namespace Math
{
    struct [[nodiscard]] rect
    {
        Math::vec2 point1{ 0.0, 0.0 }; // top right
        Math::vec2 point2{ 0.0, 0.0 }; // bottom left

        double Left() const noexcept
        {
            return std::min(point1.x, point2.x);
        }

        double Right() const noexcept
        {
            return std::max(point1.x, point2.x);
        }

        double Top() const noexcept
        {
            return std::max(point1.y, point2.y);
        }

        double Bottom() const noexcept
        {
            return std::min(point1.y, point2.y);
        }

        vec2 Size() const noexcept
        {
            return { Right() - Left(), std::abs(Top() - Bottom()) };
        }
    };

    struct [[nodiscard]] irect
    {
        Math::ivec2 point1{ 0, 0 }; // top right
        Math::ivec2 point2{ 0, 0 }; // bottom left

        int Left() const noexcept
        {
            return std::min(point1.x, point2.x);
        }

        int Right() const noexcept
        {
            return std::max(point1.x, point2.x);
        }

        int Top() const noexcept
        {
            return std::max(point1.y, point2.y);
        }

        int Bottom() const noexcept
        {
            return std::min(point1.y, point2.y);
        }

        ivec2 Size() const noexcept
        {
            return { Right() - Left(), std::abs(Top() - Bottom()) };
        }
    };
}
