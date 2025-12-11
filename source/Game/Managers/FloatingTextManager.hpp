#pragma once

#include "Engine/FontManager.hpp"
#include "Engine/Matrix.hpp"
#include "Dokev/Core/DKMath.hpp"
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

    void Add(std::string text, Math::fvec2 pos, CS200::RGBA color = 0xFFFFFFFF, float duration = 1.5f)
    {
        texts.push_back({ std::move(text), pos, duration, duration, color });
    }

    void Update(float dt)
    {
        for (auto it = texts.begin(); it != texts.end();)
        {
            it->life -= dt;
            it->pos.y += 30.0f * dt;
            if (it->life <= 0.0f)
            {
                it = texts.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    void Draw([[maybe_unused]] const Math::TransformationMatrix& viewMatrix)
    {
        auto& fm = CS230::FontManager::Instance();
        for (const auto& t : texts)
        {
            const float alphaScale = std::clamp(t.life / t.duration, 0.0f, 1.0f);
            const auto  alpha      = static_cast<uint8_t>(alphaScale * 255.0f);
            const CS200::RGBA tint = (t.color & 0xFFFFFF00u) | alpha;

            fm.DrawText("MainFont", t.text, t.pos, CS230::TextAlign::Center, tint);
        }
    }

private:
    std::vector<FloatingText> texts;
    FloatingTextManager() = default;
};
