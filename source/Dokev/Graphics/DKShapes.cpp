/**
 * \file DKShapes.cpp
 * \brief s
 * \author Younwoo Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "DKShapes.hpp"
#include "CS200/IRenderer2D.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Matrix.hpp"


namespace Dokev
{
    void DrawPixel(int posX, int posY, CS200::RGBA color)
    {
        auto& renderer = Engine::GetRenderer2D();

        Math::vec2 center{ static_cast<double>(posX) + 1.5, static_cast<double>(posY) + 1.5 };
        Math::vec2 scale{ 1.0, 1.0 };

        auto transform = Math::TranslationMatrix(center) * Math::ScaleMatrix(scale);

        renderer.DrawRectangle(transform, color, CS200::CLEAR, 0.0);
    }

    void DrawPixelV(Math::vec2 position, CS200::RGBA color)
    {
        DrawPixel(static_cast<int>(position.x), static_cast<int>(position.y), color);
    }

    void DrawLine(int startPosX, int startPosY, int endPosX, int endPosY, CS200::RGBA color)
    {
        auto& renderer = Engine::GetRenderer2D();

        Math::vec2 start{ static_cast<double>(startPosX), static_cast<double>(startPosY) };
        Math::vec2 end{ static_cast<double>(endPosX), static_cast<double>(endPosY) };

        renderer.DrawLine(start, end, color, 1.0);
    }

    void DrawLineEx(Math::vec2 startPos, Math::vec2 endPos, float thick, CS200::RGBA color)
    {
        auto& renderer = Engine::GetRenderer2D();
        renderer.DrawLine(startPos, endPos, color, static_cast<double>(thick));
    }

    void DrawCircle(int centerX, int centerY, float radius, CS200::RGBA color)
    {
        auto& renderer = Engine::GetRenderer2D();

        Math::vec2 center{ static_cast<double>(centerX), static_cast<double>(centerY) };
        Math::vec2 scale{ static_cast<double>(radius * 2.0f), static_cast<double>(radius * 2.0f) };

        auto transform = Math::TranslationMatrix(center) * Math::ScaleMatrix(scale);

        renderer.DrawCircle(transform, color, CS200::CLEAR, 0.0);
    }

    void DrawCircleV(Math::vec2 center, float radius, CS200::RGBA color)
    {
        auto& renderer = Engine::GetRenderer2D();

        Math::vec2 scale{ static_cast<double>(radius * 2.0f), static_cast<double>(radius * 2.0f) };
        auto       transform = Math::TranslationMatrix(center) * Math::ScaleMatrix(scale);

        renderer.DrawCircle(transform, color, CS200::CLEAR, 0.0);
    }

    void DrawRectangle(int posX, int posY, int width, int height, CS200::RGBA color)
    {
        auto& renderer = Engine::GetRenderer2D();

        Math::vec2 center{ static_cast<double>(posX) + static_cast<double>(width) * 0.5, static_cast<double>(posY) + static_cast<double>(height) * 0.5 };
        Math::vec2 scale{ static_cast<double>(width), static_cast<double>(height) };

        auto transform = Math::TranslationMatrix(center) * Math::ScaleMatrix(scale);

        renderer.DrawRectangle(transform, color, CS200::CLEAR, 0.0);
    }

    void DrawRectangleV(Math::vec2 position, Math::vec2 size, CS200::RGBA color)
    {
        auto& renderer = Engine::GetRenderer2D();

        Math::vec2 center = position + size * 0.5;

        auto transform = Math::TranslationMatrix(center) * Math::ScaleMatrix(size);

        renderer.DrawRectangle(transform, color, CS200::CLEAR, 0.0);
    }

    void DrawRectangleLines(int posX, int posY, int width, int height, CS200::RGBA color)
    {
        DrawRectangleLinesEx(posX, posY, width, height, 1.0f, color);
    }

    void DrawRectangleLinesEx(int posX, int posY, int width, int height, float lineThick, CS200::RGBA color)
    {
        auto& renderer = Engine::GetRenderer2D();

        Math::vec2 center{ static_cast<double>(posX) + static_cast<double>(width) * 0.5, static_cast<double>(posY) + static_cast<double>(height) * 0.5 };
        Math::vec2 scale{ static_cast<double>(width), static_cast<double>(height) };

        auto transform = Math::TranslationMatrix(center) * Math::ScaleMatrix(scale);

        renderer.DrawRectangle(transform, CS200::CLEAR, color, static_cast<double>(lineThick));
    }

    void DrawCircleLines(int centerX, int centerY, float radius, CS200::RGBA color)
    {
        auto& renderer = Engine::GetRenderer2D();

        Math::vec2 center{ static_cast<double>(centerX), static_cast<double>(centerY) };
        Math::vec2 scale{ static_cast<double>(radius * 2.0f), static_cast<double>(radius * 2.0f) };

        auto transform = Math::TranslationMatrix(center) * Math::ScaleMatrix(scale);

        renderer.DrawCircle(transform, CS200::CLEAR, color, 1.0);
    }
}
