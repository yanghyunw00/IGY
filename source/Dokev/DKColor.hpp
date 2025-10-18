/**
 * \file DKColor.hpp
 * \brief 
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once

#include "CS200/RGBA.hpp"

namespace Dokev
{
    namespace ColorPalette
    {
        constexpr CS200::RGBA CORAL   = 0xFF7F50FF;
        constexpr CS200::RGBA TOMATO  = 0xFF6347FF;
        constexpr CS200::RGBA SALMON  = 0xFA8072FF;
        constexpr CS200::RGBA CRIMSON = 0xDC143CFF;
        constexpr CS200::RGBA RED     = 0xE62937FF;
        constexpr CS200::RGBA MAROON  = 0xBE2148FF;
        constexpr CS200::RGBA PINK    = 0xFF6DC8FF;
        constexpr CS200::RGBA HOTPINK = 0xFF69B4FF;

        constexpr CS200::RGBA ORANGE    = 0xFF9900FF;
        constexpr CS200::RGBA BROWN     = 0x7F503CFF;
        constexpr CS200::RGBA DARKBROWN = 0x4C2C1CFF;

        constexpr CS200::RGBA GOLD   = 0xFFCB00FF;
        constexpr CS200::RGBA YELLOW = 0xFDD400FF;
        constexpr CS200::RGBA BEIGE  = 0xD3B494FF;
        constexpr CS200::RGBA KHAKI  = 0xF0E68CFF;

        constexpr CS200::RGBA PALEGREEN = 0x98FB98FF;
        constexpr CS200::RGBA LIME      = 0x00DC3CFF;
        constexpr CS200::RGBA GREEN     = 0x00E436FF;
        constexpr CS200::RGBA SEAGREEN  = 0x2E8B57FF;
        constexpr CS200::RGBA DARKGREEN = 0x00781EFF;
        constexpr CS200::RGBA OLIVE     = 0x808000FF;

        constexpr CS200::RGBA AQUA      = 0x00FFFFFF;
        constexpr CS200::RGBA CYAN      = 0x00FFFFFF;
        constexpr CS200::RGBA TURQUOISE = 0x40E0D0FF;
        constexpr CS200::RGBA TEAL      = 0x008080FF;

        constexpr CS200::RGBA SKYBLUE   = 0x66D7EEFF;
        constexpr CS200::RGBA ROYALBLUE = 0x4169E1FF;
        constexpr CS200::RGBA BLUE      = 0x0078F0FF;
        constexpr CS200::RGBA DARKBLUE  = 0x003C82FF;
        constexpr CS200::RGBA NAVY      = 0x000080FF;

        constexpr CS200::RGBA LAVENDER   = 0xE6E6FAFF;
        constexpr CS200::RGBA ORCHID     = 0xDA70D6FF;
        constexpr CS200::RGBA PURPLE     = 0xC865C8FF;
        constexpr CS200::RGBA MAGENTA    = 0xFF00FFFF;
        constexpr CS200::RGBA VIOLET     = 0x873CC8FF;
        constexpr CS200::RGBA INDIGO     = 0x4B0082FF;
        constexpr CS200::RGBA DARKPURPLE = 0x702883FF;

        constexpr CS200::RGBA WHITE     = 0xFFFFFFFF;
        constexpr CS200::RGBA LIGHTGRAY = 0xC8C8C8FF;
        constexpr CS200::RGBA SILVER    = 0xC0C0C0FF;
        constexpr CS200::RGBA GRAY      = 0x828282FF;
        constexpr CS200::RGBA SLATEGRAY = 0x708090FF;
        constexpr CS200::RGBA DARKGRAY  = 0x505050FF;
        constexpr CS200::RGBA BLACK     = 0x000000FF;
        constexpr CS200::RGBA BLANK     = 0x00000000;
    }

    CS200::RGBA ColorLerp(CS200::RGBA color1, CS200::RGBA color2, float t);

    CS200::RGBA ColorAdd(CS200::RGBA color1, CS200::RGBA color2);

    CS200::RGBA ColorAlpha(CS200::RGBA color, float alpha);

    CS200::RGBA ColorFromRGBA(int r, int g, int b, int a = 255);

    CS200::RGBA ColorFromNormalized(float r, float g, float b, float a = 1.0f);
}
