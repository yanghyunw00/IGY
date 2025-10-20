/**
 * \file DKMouseInput.cpp
 * \brief 
 * \author Junseok Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "DKMouseInput.hpp"

namespace Dokev
{
    Mouse::Mouse()
    {
        buttonsDown.fill(false);
        previousButtonsDown.fill(false);
    }

    void Mouse::Update()
    {
        previousButtonsDown = buttonsDown;
        previousPosition = position;

        Uint32 mouseState = SDL_GetMouseState(&position.x, &position.y);

        SetMouseDown(Button::Left, (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0);
        SetMouseDown(Button::Middle, (mouseState & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0);
        SetMouseDown(Button::Right, (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0);
        scrollDelta = 0;
    }

    bool Mouse::MouseDown(Button button) const
    {
        return buttonsDown[static_cast<size_t>(button)];
    }

    bool Mouse::MouseJustPressed(Button button) const
    {
        const size_t index = static_cast<size_t>(button);
        return buttonsDown[index] && !previousButtonsDown[index];
    }

    bool Mouse::MouseJustReleased(Button button) const
    {
        const size_t index = static_cast<size_t>(button);
        return !buttonsDown[index] && previousButtonsDown[index];
    }

    Math::ivec2 Mouse::GetPosition() const
    {
        return position;
    }

    Math::vec2 Mouse::GetPositionFloat() const
    {
        return Math::vec2{ static_cast<double>(position.x), static_cast<double>(position.y) };
    }

    Math::ivec2 Mouse::GetDelta() const
    {
        return position - previousPosition;
    }

    int Mouse::GetScrollDelta() const
    {
        return scrollDelta;
    }

    void Mouse::ProcessEvent(const SDL_Event& event)
    {
        if (event.type == SDL_MOUSEWHEEL)
        {
            scrollDelta = event.wheel.y;
        }
    }

    void Mouse::SetMouseDown(Button button, bool value)
    {
        buttonsDown[static_cast<size_t>(button)] = value;
    }
}
