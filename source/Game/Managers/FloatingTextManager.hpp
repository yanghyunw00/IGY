/**
 * \file FloatingTextManager.hpp
 * \brief
 * \author
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once

#include "Dokev/Core/DKMath.hpp"
#include "Engine/FontManager.hpp"
#include "Engine/Matrix.hpp"
#include <algorithm>
#include <string>
#include <vector>


struct FloatingText
{
    std::string text;
    Math::fvec2 pos;
    float       life;
    float       duration;
    CS200::RGBA color;
    bool        screenSpace;
};

class FloatingTextManager
{
public:
    static FloatingTextManager& Instance()
    {
        static FloatingTextManager instance;
        return instance;
    }

    // screenSpace == false : pos is a world coordinate (drawn with the camera transform).
    // screenSpace == true  : pos is a screen-pixel coordinate, fixed on screen like UI
    //                        (origin bottom-left, +y up). Drawn in the UI pass.
    void Add(std::string text, Math::fvec2 pos, CS200::RGBA color = 0xFFFFFFFF, float duration = 1.5f, bool screenSpace = false);

    void Update(float dt);

    // Draw only the texts matching the requested space. Call once inside the
    // world-space scene (screenSpace == false) and once inside the UI scene
    // (screenSpace == true).
    void Draw(bool screenSpace = false);

private:
    std::vector<FloatingText> texts;
    FloatingTextManager() = default;
};
