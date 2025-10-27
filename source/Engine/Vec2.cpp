/**
 * \file Vec2.cpp
 * \author Jonathan Holmes
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "Vec2.hpp"
#include <cmath>
#include <iostream>
#include <istream>
#include <ostream>
#include <sstream>

namespace Math
{

    /*--------------------------This is ivec2---------------------------------*/

    // Constructors
    ivec2::ivec2(const vec2& v) noexcept : x(static_cast<int>(v.x)), y(static_cast<int>(v.y))
    {
    }

    // Stream operators
    std::ostream& operator<<(std::ostream& os, const ivec2& v)
    {
        os << v.x << ", " << v.y;
        return os;
    }

    std::istream& operator>>(std::istream& is, ivec2& v)
    {
        is >> v.x >> v.y;
        return is;
    }

    /*-------------------------------This is vec2---------------------------------------------------*/
    // Constructors

    vec2::vec2(const ivec2& v) noexcept : x(v.x), y(v.y)
    {
    }

    double vec2::Length() const
    {
        return std::sqrt(x * x + y * y);
    }

    vec2 vec2::Normalize() const
    {
        double length = std::sqrt(x * x + y * y);

        if (length > 0)
        {
            double inv_length = 1 / length;
            return { x * inv_length, y * inv_length };
        }
        return *this;
    }

    // Stream operators
    std::ostream& operator<<(std::ostream& os, const vec2& v)
    {
        os << v.x << ", " << v.y;
        return os;
    }

    std::istream& operator>>(std::istream& is, vec2& v)
    {
        is >> v.x >> v.y;
        return is;
    }

    /*-------------------------------This is fvec2---------------------------------------------------*/

    fvec2::fvec2(const fvec2& v) noexcept : x(v.x), y(v.y)
    {
    }

    float fvec2::Length() const
    {
        return std::sqrtf(x * x + y * y);
    }

    fvec2 fvec2::Normalize() const
    {
        float length = std::sqrtf(x * x + y * y);

        if (length > 0)
        {
            float inv_length = 1 / length;
            return { x * inv_length, y * inv_length };
        }
        return *this;
    }

    fvec2 fvec2::vec2Add(fvec2 v1, fvec2 v2)
    {
        return { v1.x + v2.x, v1.y + v2.y };
    }

    fvec2 fvec2::vec2AddValue(fvec2 v, float add)
    {
        return { v.x + add, v.y + add };
    }

    fvec2 fvec2::vec2Subtract(fvec2 v1, fvec2 v2)
    {
        return { v1.x - v2.x, v1.y - v2.y };
    }

    fvec2 fvec2::vec2SubtractValue(fvec2 v, float sub)
    {
        return { v.x - sub, v.y - sub };
    }

    std::ostream& operator<<(std::ostream& os, const fvec2& v)
    {
        os << v.x << ", " << v.y;
        return os;
    }

    std::istream& operator>>(std::istream& is, fvec2& v)
    {
        is >> v.x >> v.y;
        return is;
    }

    /*-------------------------------This is vec4---------------------------------------------------*/
    std::ostream& operator<<(std::ostream& os, const vec4& v)
    {
        os << v.x << ", " << v.y << ", " << v.z << ", " << v.w;
        return os;
    }

    std::istream& operator>>(std::istream& is, vec4& v)
    {
        is >> v.x >> v.y >> v.z >> v.w;
        return is;
    }
}
