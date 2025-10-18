/**
 * \file DKColor.cpp
 * \brief 
 * \author Young woo Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "DKColor.hpp"
#include <algorithm>
#include <cstdint>

namespace Dokev
{
    CS200::RGBA ColorLerp(CS200::RGBA color1, CS200::RGBA color2, float t)
    {
        t = std::clamp(t, 0.0f, 1.0f);

        auto c1 = CS200::unpack_color(color1);
        auto c2 = CS200::unpack_color(color2);

        std::array<float, 4> result;
        for (int i = 0; i < 4; ++i)
        {
            result[i] = c1[i] + (c2[i] - c1[i]) * t;
        }

        return CS200::pack_color(result);
    }

    CS200::RGBA ColorAdd(CS200::RGBA color1, CS200::RGBA color2)
    {
        auto c1 = CS200::unpack_color(color1);
        auto c2 = CS200::unpack_color(color2);

        std::array<float, 4> result;
        for (int i = 0; i < 4; ++i)
        {
            result[i] = std::min(c1[i] + c2[i], 1.0f);
        }

        return CS200::pack_color(result);
    }

    CS200::RGBA ColorAlpha(CS200::RGBA color, float alpha)
    {
        auto c = CS200::unpack_color(color);

        c[3] = std::clamp(alpha, 0.0f, 1.0f);

        return CS200::pack_color(c);
    }

    CS200::RGBA ColorFromRGBA(int r, int g, int b, int a)
    {
        auto clamp_byte = [](int value) -> uint8_t {
            return static_cast<uint8_t>(std::clamp(value, 0, 255));
        };

        uint8_t R = clamp_byte(r);
        uint8_t G = clamp_byte(g);
        uint8_t B = clamp_byte(b);
        uint8_t A = clamp_byte(a);

        return (static_cast<CS200::RGBA>(R) << 24) |
               (static_cast<CS200::RGBA>(G) << 16) |
               (static_cast<CS200::RGBA>(B) << 8) |
               (static_cast<CS200::RGBA>(A) << 0);
    }

    CS200::RGBA ColorFromNormalized(float r, float g, float b, float a)
    {
        std::array<float, 4> color = {
            std::clamp(r, 0.0f, 1.0f),
            std::clamp(g, 0.0f, 1.0f),
            std::clamp(b, 0.0f, 1.0f),
            std::clamp(a, 0.0f, 1.0f)
        };

        return CS200::pack_color(color);
    }
}
