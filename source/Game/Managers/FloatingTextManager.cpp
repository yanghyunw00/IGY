/**
 * \file FloatingTextManager.cpp
 * \brief
 * \author
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "FloatingTextManager.hpp"
#include <algorithm>

void FloatingTextManager::Add(std::string text, Math::fvec2 pos, CS200::RGBA color, float duration, bool screenSpace)
{
    texts.push_back({ std::move(text), pos, duration, duration, color, screenSpace });
}

void FloatingTextManager::Update(float dt)
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

void FloatingTextManager::Draw(bool screenSpace)
{
    auto& fm = CS230::FontManager::Instance();
    for (const auto& t : texts)
    {
        if (t.screenSpace != screenSpace)
            continue;

        const float       alphaScale = std::clamp(t.life / t.duration, 0.0f, 1.0f);
        const auto        alpha      = static_cast<uint8_t>(alphaScale * 255.0f);
        const CS200::RGBA tint       = (t.color & 0xFFFFFF00u) | alpha;

        fm.DrawText("MainFont", t.text, t.pos, CS230::TextAlign::Center, tint);
    }
}
