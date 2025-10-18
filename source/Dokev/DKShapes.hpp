/**
 * \file DKMath.hpp
 * \brief shape functions
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

/*
-
DrawPixel: Draw a single 1x1 pixel at integer coordinates

DrawPixelV: Draw a single 1x1 pixel at a 2D vector position

DrawLine: Draw a 1-pixel thick line between two integer coordinates

DrawLineEx: Draw a line between two vector points with custom thickness

DrawCircle: Draw a filled circle at given integer center and radius

DrawCircleV: Draw a filled circle at a vector center with given radius

DrawRectangle: Draw a filled rectangle at integer position and size

DrawRectangleV: Draw a filled rectangle at a vector position and size

DrawRectangleLines: Draw a 1-pixel thick rectangle outline at integer position and size

DrawRectangleLinesEx: Draw a rectangle outline at integer position and size with custom thickness

DrawCircleLines: Draw a circle outline at integer center and radius

*/

#pragma once

#include "CS200/RGBA.hpp"
#include "Engine/Vec2.hpp"

namespace Dokev
{

    void DrawPixel(int posX, int posY, CS200::RGBA color);

    void DrawPixelV(Math::vec2 position, CS200::RGBA color);

    void DrawLine(int startPosX, int startPosY, int endPosX, int endPosY, CS200::RGBA color);

    void DrawLineEx(Math::vec2 startPos, Math::vec2 endPos, float thick, CS200::RGBA color);

    void DrawCircle(int centerX, int centerY, float radius, CS200::RGBA color);

    void DrawCircleV(Math::vec2 center, float radius, CS200::RGBA color);

    void DrawRectangle(int posX, int posY, int width, int height, CS200::RGBA color);

    void DrawRectangleV(Math::vec2 position, Math::vec2 size, CS200::RGBA color);

    void DrawRectangleLines(int posX, int posY, int width, int height, CS200::RGBA color);

    void DrawRectangleLinesEx(int posX, int posY, int width, int height, float lineThick, CS200::RGBA color);


    void DrawCircleLines(int centerX, int centerY, float radius, CS200::RGBA color);

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
}
