/**
 * \file Vec2.hpp
 * \author Jonathan Holmes
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */


#pragma once
#include <iostream>
#include <limits>

namespace Math
{
    struct vec2;

    struct [[nodiscard]] ivec2
    {
        int x{ 0 };
        int y{ 0 };

        // Constructors
        constexpr ivec2() noexcept = default;

        constexpr ivec2(int x_, int y_) noexcept : x(x_), y(y_)
        {
        }

        explicit constexpr ivec2(int xy) noexcept : x(xy), y(xy)
        {
        }

        // Conversion
        explicit ivec2(const vec2& v) noexcept;

        // Arithmetic operators
        constexpr ivec2& operator+=(const ivec2& rhs) noexcept
        {
            x += rhs.x;
            y += rhs.y;
            return *this;
        }

        constexpr ivec2& operator-=(const ivec2& rhs) noexcept
        {
            x -= rhs.x;
            y -= rhs.y;
            return *this;
        }

        constexpr ivec2& operator*=(int scalar) noexcept
        {
            x *= scalar;
            y *= scalar;
            return *this;
        }

        constexpr ivec2& operator/=(int scalar) noexcept
        {
            x /= scalar;
            y /= scalar;
            return *this;
        }

        // Unary operators
        constexpr ivec2 operator+() const noexcept
        {
            return ivec2{ x, y };
        }

        constexpr ivec2 operator-() const noexcept
        {
            return ivec2{ -x, -y };
        }
    };

    // Non-member operators
    constexpr ivec2 operator+(const ivec2& lhs, const ivec2& rhs) noexcept
    {
        return ivec2{ lhs.x + rhs.x, lhs.y + rhs.y };
    }

    constexpr ivec2 operator-(const ivec2& lhs, const ivec2& rhs) noexcept
    {
        return ivec2{ lhs.x - rhs.x, lhs.y - rhs.y };
    }

    constexpr ivec2 operator*(const ivec2& v, int scalar) noexcept
    {
        return ivec2{ v.x * scalar, v.y * scalar };
    }

    constexpr ivec2 operator*(int scalar, const ivec2& v) noexcept
    {
        return ivec2{ v.x * scalar, v.y * scalar };
    }

    constexpr ivec2 operator/(const ivec2& v, int scalar) noexcept
    {
        return ivec2{ v.x / scalar, v.y / scalar };
    }

    // Comparison operators
    constexpr bool operator==(const ivec2& lhs, const ivec2& rhs) noexcept
    {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    constexpr bool operator!=(const ivec2& lhs, const ivec2& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    // Stream operators
    std::ostream& operator<<(std::ostream& os, const ivec2& v);
    std::istream& operator>>(std::istream& is, ivec2& v);

    /*-------------------------This is vec2--------------------------------*/

    struct [[nodiscard]] vec2
    {
        double x{ 0 };
        double y{ 0 };

        // Constructors
        constexpr vec2() noexcept = default;

        constexpr vec2(double x_, double y_) noexcept : x(x_), y(y_)
        {
        }

        explicit constexpr vec2(double xy) noexcept : x(xy), y(xy)
        {
        }

        // Conversion
        explicit vec2(const ivec2& v) noexcept;

        // Arithmetic operators
        constexpr vec2& operator+=(const vec2& rhs) noexcept
        {
            x += rhs.x;
            y += rhs.y;
            return *this;
        }

        constexpr vec2& operator-=(const vec2& rhs) noexcept
        {
            x -= rhs.x;
            y -= rhs.y;
            return *this;
        }

        constexpr vec2& operator*=(double scalar) noexcept
        {
            x *= scalar;
            y *= scalar;
            return *this;
        }

        constexpr vec2& operator/=(double scalar) noexcept
        {
            x /= scalar;
            y /= scalar;
            return *this;
        }

        // Unary operators
        constexpr vec2 operator+() const noexcept
        {
            return *this;
        }

        constexpr vec2 operator-() const noexcept
        {
            return vec2{ -x, -y };
        }

        double Length() const;

        vec2 Normalize() const;
    };

    // Non-member operators
    constexpr vec2 operator+(const vec2& lhs, const vec2& rhs) noexcept
    {
        return vec2{ lhs.x + rhs.x, lhs.y + rhs.y };
    }

    constexpr vec2 operator-(const vec2& lhs, const vec2& rhs) noexcept
    {
        return vec2{ lhs.x - rhs.x, lhs.y - rhs.y };
    }

    constexpr vec2 operator*(const vec2& v, double scalar) noexcept
    {
        return vec2{ v.x * scalar, v.y * scalar };
    }

    constexpr vec2 operator*(double scalar, const vec2& v) noexcept
    {
        return vec2{ v.x * scalar, v.y * scalar };
    }

    constexpr vec2 operator/(const vec2& v, double scalar) noexcept
    {
        return vec2{ v.x / scalar, v.y / scalar };
    }

    // Comparison operators
    constexpr bool operator==(const vec2& lhs, const vec2& rhs) noexcept
    {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    constexpr bool operator!=(const vec2& lhs, const vec2& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    // Stream operators
    std::ostream& operator<<(std::ostream& os, const vec2& v);
    std::istream& operator>>(std::istream& is, vec2& v);

    //===================fvec2========================================
    struct [[nodiscard]] fvec2
    {
        float x{ 0 };
        float y{ 0 };

        // Constructors
        constexpr fvec2() noexcept = default;

        constexpr fvec2(float x_, float y_) noexcept : x(x_), y(y_)
        {
        }

        explicit constexpr fvec2(float xy) noexcept : x(xy), y(xy)
        {
        }

        // Conversion
        fvec2(const fvec2& v) noexcept;

        float Length() const;

        fvec2 Normalize() const;

        fvec2 vec2Add(fvec2 v1, fvec2 v2);

        fvec2 vec2AddValue(fvec2 v, float add);

        fvec2 vec2Subtract(fvec2 v1, fvec2 v2);

        fvec2 vec2SubtractValue(fvec2 v, float sub);




        // Arithmetic operators
        constexpr fvec2& operator+=(const fvec2& rhs) noexcept
        {
            x += rhs.x;
            y += rhs.y;
            return *this;
        }

        constexpr fvec2& operator-=(const fvec2& rhs) noexcept
        {
            x -= rhs.x;
            y -= rhs.y;
            return *this;
        }

        constexpr fvec2& operator*=(int scalar) noexcept
        {
            x *= scalar;
            y *= scalar;
            return *this;
        }

        constexpr fvec2& operator/=(int scalar) noexcept
        {
            x /= scalar;
            y /= scalar;
            return *this;
        }

        // Unary operators
        constexpr fvec2 operator+() const noexcept
        {
            return fvec2{ x, y };
        }

        constexpr fvec2 operator-() const noexcept
        {
            return fvec2{ -x, -y };
        }
    };

    // Non-member operators
    constexpr fvec2 operator+(const fvec2& lhs, const fvec2& rhs) noexcept
    {
        return fvec2{ lhs.x + rhs.x, lhs.y + rhs.y };
    }

    constexpr fvec2 operator-(const fvec2& lhs, const fvec2& rhs) noexcept
    {
        return fvec2{ lhs.x - rhs.x, lhs.y - rhs.y };
    }

    constexpr fvec2 operator*(const fvec2& v, int scalar) noexcept
    {
        return fvec2{ v.x * scalar, v.y * scalar };
    }

    constexpr fvec2 operator*(int scalar, const fvec2& v) noexcept
    {
        return fvec2{ v.x * scalar, v.y * scalar };
    }

    constexpr fvec2 operator/(const fvec2& v, int scalar) noexcept
    {
        return fvec2{ v.x / scalar, v.y / scalar };
    }

    // Comparison operators
    constexpr bool operator==(const fvec2& lhs, const fvec2& rhs) noexcept
    {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    constexpr bool operator!=(const fvec2& lhs, const fvec2& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    // Stream operators
    std::ostream& operator<<(std::ostream& os, const fvec2& v);
    std::istream& operator>>(std::istream& is, fvec2& v);
}
