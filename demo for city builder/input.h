/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Input.h
Project:    CS230 Engine
Author:     Junseok Lee
Created:    March 8, 2023
*/

#pragma once
#include <vector>
#include <raylib.h>
namespace CS230 {
    class Input {
    public:
        enum class Keys {
            A, B, C, D, E, F, G, H, I, J, K, L, M,
            N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
            Space, Enter, Left, Up, Right, Down,
            Escape,Tab,Count
        };

        Input();
        void Update();

        bool KeyDown(Keys key);

        bool KeyJustPressed(Keys key);

        bool KeyJustReleased(Keys key);

        void SetKeyDown(Keys key, bool value);


    private:
        std::vector<bool> keys_down;
        std::vector<bool> previous_keys_down;

    };
}
constexpr CS230::Input::Keys& operator++(CS230::Input::Keys& key) noexcept;
constexpr int cs230_to_rl(CS230::Input::Keys cs230_key) noexcept;