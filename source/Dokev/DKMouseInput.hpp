#pragma once

#include "Engine/Vec2.hpp"
#include <SDL.h>
#include <array>
#include <gsl/gsl>

namespace Dokev
{
    class Mouse
    {
    public:
        enum class Button
        {
            Left,
            Middle,
            Right,
            Count
        };

        Mouse();

        void Update();

        bool MouseDown(Button button) const;

        bool MouseJustPressed(Button button) const;

        bool MouseJustReleased(Button button) const;

        Math::ivec2 GetPosition() const;

        Math::vec2 GetPositionFloat() const;

        Math::ivec2 GetDelta() const;

        int GetScrollDelta() const;

        void ProcessEvent(const SDL_Event& event);

    private:
        std::array<bool, static_cast<size_t>(Button::Count)> buttonsDown;
        std::array<bool, static_cast<size_t>(Button::Count)> previousButtonsDown;
        Math::ivec2                                          position{ 0, 0 };
        Math::ivec2                                          previousPosition{ 0, 0 };
        int                                                  scrollDelta{ 0 };

        void SetMouseDown(Button button, bool value);
    };
}