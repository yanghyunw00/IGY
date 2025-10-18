/**
 * \file Input.cpp
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "Input.hpp"
#include "Engine.hpp"
#include "Logger.hpp"
#include <SDL.h>

namespace CS230
{
    Input::Input()
    {
        previous_keys_down.fill(false);
        keys_down.fill(false);
    }

    void Input::Update()
    {
        previous_keys_down          = keys_down;
        const Uint8* keyboard_state = SDL_GetKeyboardState(nullptr);
        SDL_Scancode scancode;

        for (size_t i = 0; i < static_cast<size_t>(Input::Keys::Count); ++i)
        {
            scancode     = convert_cs230_to_sdl(static_cast<Input::Keys>(i));
            keys_down[i] = keyboard_state[scancode];
        }
    }

    void Input::SetKeyDown(Keys key, bool value)
    {
        keys_down[static_cast<std::size_t>(key)] = value;
    }

    bool Input::KeyDown(Keys key) const
    {
        return keys_down[static_cast<std::size_t>(key)];
    }

    bool Input::KeyJustPressed(Keys key) const
    {
        const std::size_t index = static_cast<std::size_t>(key);
        return keys_down[index] && !previous_keys_down[index];
    }

    bool Input::KeyJustReleased(Keys key) const
    {
        const std::size_t index = static_cast<std::size_t>(key);
        return !keys_down[index] && previous_keys_down[index];
    }

    SDL_Scancode Input::convert_cs230_to_sdl(Keys key) noexcept
    {
        switch (key)
        {
            case Keys::A: return SDL_SCANCODE_A;
            case Keys::B: return SDL_SCANCODE_B;
            case Keys::C: return SDL_SCANCODE_C;
            case Keys::D: return SDL_SCANCODE_D;
            case Keys::E: return SDL_SCANCODE_E;
            case Keys::F: return SDL_SCANCODE_F;
            case Keys::G: return SDL_SCANCODE_G;
            case Keys::H: return SDL_SCANCODE_H;
            case Keys::I: return SDL_SCANCODE_I;
            case Keys::J: return SDL_SCANCODE_J;
            case Keys::K: return SDL_SCANCODE_K;
            case Keys::L: return SDL_SCANCODE_L;
            case Keys::M: return SDL_SCANCODE_M;
            case Keys::N: return SDL_SCANCODE_N;
            case Keys::O: return SDL_SCANCODE_O;
            case Keys::P: return SDL_SCANCODE_P;
            case Keys::Q: return SDL_SCANCODE_Q;
            case Keys::R: return SDL_SCANCODE_R;
            case Keys::S: return SDL_SCANCODE_S;
            case Keys::T: return SDL_SCANCODE_T;
            case Keys::U: return SDL_SCANCODE_U;
            case Keys::V: return SDL_SCANCODE_V;
            case Keys::W: return SDL_SCANCODE_W;
            case Keys::X: return SDL_SCANCODE_X;
            case Keys::Y: return SDL_SCANCODE_Y;
            case Keys::Z: return SDL_SCANCODE_Z;
            case Keys::Enter: return SDL_SCANCODE_RETURN;
            case Keys::Left: return SDL_SCANCODE_LEFT;
            case Keys::Right: return SDL_SCANCODE_RIGHT;
            case Keys::Up: return SDL_SCANCODE_UP;
            case Keys::Down: return SDL_SCANCODE_DOWN;
            case Keys::Escape: return SDL_SCANCODE_ESCAPE;
            case Keys::Tab: return SDL_SCANCODE_TAB;
            default: return SDL_SCANCODE_UNKNOWN;
        }
    }

}
