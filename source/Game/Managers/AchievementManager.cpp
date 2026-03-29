/**
 * \file AchievementManager.cpp
 * \brief Tracks and displays achievement unlocks with confetti
 * \date 2026 Spring
 * \copyright DigiPen Institute of Technology
 */
#include "AchievementManager.hpp"

#include "Dokev/Core/DKColor.hpp"
#include "Dokev/Graphics/DKShapes.hpp"
#include "Engine/Engine.hpp"
#include "Engine/FontManager.hpp"
#include "Engine/Window.hpp"

#include <algorithm>
#include <cstdlib>

namespace
{
    constexpr CS200::RGBA confettiColors[] = {
        Dokev::ColorPalette::RED,
        Dokev::ColorPalette::GOLD,
        Dokev::ColorPalette::GREEN,
        Dokev::ColorPalette::ROYALBLUE,
        Dokev::ColorPalette::HOTPINK,
        Dokev::ColorPalette::ORANGE,
        Dokev::ColorPalette::CYAN,
        Dokev::ColorPalette::PURPLE,
        Dokev::ColorPalette::LIME,
    };
    constexpr int colorCount = static_cast<int>(sizeof(confettiColors) / sizeof(confettiColors[0]));
}

void AchievementManager::Init()
{
    achievements = {
        { "pop10",       "[Achievement Unlocked] Population 10!",  false },
        { "build10",     "[Achievement Unlocked] 10 Buildings!",   false },
        { "happy90",     "[Achievement Unlocked] Happiness 90+!",  false },
        { "resource100", "[Achievement Unlocked] 100 Resources!",  false },
    };
}

void AchievementManager::Update(float dt, int population, int buildingCount, float happiness, int maxSingleResource)
{
    for (auto& a : achievements)
    {
        if (a.unlocked)
            continue;

        bool met = false;
        if      (a.id == "pop10"       && population       >= 10)   met = true;
        else if (a.id == "build10"     && buildingCount    >= 10)   met = true;
        else if (a.id == "happy90"     && happiness        >= 90.f) met = true;
        else if (a.id == "resource100" && maxSingleResource >= 100) met = true;

        if (met)
            Unlock(a);
    }

    if (notificationTimer > 0.f)
    {
        notificationTimer -= dt;

        for (auto& c : confetti)
        {
            c.pos.x += c.vel.x * dt;
            c.pos.y += c.vel.y * dt;
            c.vel.y -= 80.f * dt; // gravity (accelerate downward)
        }
    }
}

void AchievementManager::Unlock(Achievement& achievement)
{
    achievement.unlocked = true;
    notificationText     = achievement.message;
    notificationTimer    = notificationDuration;
    SpawnConfetti();
}

void AchievementManager::SpawnConfetti()
{
    auto windowSize = Engine::GetWindow().GetSize();

    confetti.clear();
    confetti.reserve(confettiCount);

    for (int i = 0; i < confettiCount; ++i)
    {
        ConfettiPiece piece;

        // Spread across the entire top of the screen, just above the visible area
        piece.pos.x = static_cast<float>(rand() % windowSize.x);
        piece.pos.y = static_cast<float>(windowSize.y) + static_cast<float>(rand() % 60);

        // Fall downward (y decreases = toward bottom in UI space), with horizontal drift
        piece.vel.x = static_cast<float>((rand() % 160) - 80);
        piece.vel.y = -(static_cast<float>(120 + rand() % 200)); // negative = falling down

        piece.color = confettiColors[rand() % colorCount];

        confetti.push_back(piece);
    }
}

void AchievementManager::Draw() const
{
    if (notificationTimer <= 0.f || notificationText.empty())
        return;

    // Compute alpha: fully visible, fade out over last fadeOutTime seconds
    float alpha = 1.f;
    if (notificationTimer < fadeOutTime)
        alpha = notificationTimer / fadeOutTime;

    const auto        a     = static_cast<uint8_t>(alpha * 255.f);
    const CS200::RGBA textColor = (Dokev::ColorPalette::GOLD & 0xFFFFFF00u) | a;

    // Draw confetti pieces
    for (const auto& c : confetti)
    {
        const CS200::RGBA tint = (c.color & 0xFFFFFF00u) | a;
        Dokev::DrawRectangleV(
            Math::vec2{ static_cast<double>(c.pos.x), static_cast<double>(c.pos.y) },
            Math::vec2{ 12.0, 16.0 },
            tint);
    }

    // Draw notification text
    auto windowSize = Engine::GetWindow().GetSize();
    const Math::fvec2 center = {
        static_cast<float>(windowSize.x) * 0.5f,
        static_cast<float>(windowSize.y) * 0.5f
    };

    CS230::FontManager::Instance().DrawText("MainFont", notificationText, center, CS230::TextAlign::Center, textColor);
}
