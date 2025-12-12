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
};

class FloatingTextManager
{
public:
    static FloatingTextManager& Instance()
    {
        static FloatingTextManager instance;
        return instance;
    }

    void Add(std::string text, Math::fvec2 pos, CS200::RGBA color = 0xFFFFFFFF, float duration = 1.5f);

    void Update(float dt);

    void Draw([[maybe_unused]] const Math::TransformationMatrix& viewMatrix);

private:
    std::vector<FloatingText> texts;
    FloatingTextManager() = default;
};
