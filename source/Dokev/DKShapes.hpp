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

}
