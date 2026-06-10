/**
 * \file AchievementManager.cpp
 * \brief Tracks and displays achievement unlocks with confetti
 * \date 2026 Spring
 * \copyright DigiPen Institute of Technology
 */
#include "AchievementManager.hpp"

#include "Dokev/Core/DKColor.hpp"
#include "Dokev/Core/DKUtility.hpp"
#include "Dokev/Graphics/DKShapes.hpp"
#include "Engine/Engine.hpp"
#include "Engine/FontManager.hpp"
#include "Engine/Path.hpp"
#include "Engine/Window.hpp"

#include <algorithm>
#include <cstdlib>
#include <exception>
#include <cmath>

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
    LoadConfettiAssets();

    achievements = {
        { "pop10",       "[Achievement Unlocked] Population 10!",  false },
        { "build10",     "[Achievement Unlocked] 10 Buildings!",   false },
        { "happy90",     "[Achievement Unlocked] Happiness 90+!",  false },
        { "resource5000", "[Achievement Unlocked] 5000 Resources!",  false },
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
        else if (a.id == "resource5000" && maxSingleResource >= 5000) met = true;

        if (met)
            Unlock(a);
    }

    if (notificationTimer > 0.f)
    {
        notificationTimer = std::max(0.0f, notificationTimer - dt);
    }

    if (!confetti.empty())
    {
        const auto windowSize = Engine::GetWindow().GetSize();

        for (auto& c : confetti)
        {
            c.age += dt;
            const float lifeRatio = std::clamp(1.0f - c.age / c.lifetime, 0.0f, 1.0f);
            c.scale              = c.baseScale * (0.25f + lifeRatio * 0.75f);

            c.pos.x += c.vel.x * dt;
            c.pos.y += c.vel.y * dt;
            c.vel.y -= 780.f * dt;
            c.vel.x *= std::max(0.0f, 1.0f - 0.45f * dt);
            c.rotation += c.angularVelocity * dt;
            c.angularVelocity *= std::max(0.0f, 1.0f - 0.35f * dt);

            float halfW = 10.0f * c.scale;
            float halfH = 12.0f * c.scale;
            if (!confettiAssets.empty() && c.assetIndex < confettiAssets.size())
            {
                halfW = static_cast<float>(confettiAssets[c.assetIndex].GetWidth()) * c.scale * 0.5f;
                halfH = static_cast<float>(confettiAssets[c.assetIndex].GetHeight()) * c.scale * 0.5f;
            }

            if (c.pos.x - halfW < 0.0f && c.vel.x < 0.0f)
            {
                c.pos.x = halfW;
                c.vel.x = -c.vel.x * c.bounce;
                c.angularVelocity = -c.angularVelocity * 0.9f;
            }
            else if (c.pos.x + halfW > static_cast<float>(windowSize.x) && c.vel.x > 0.0f)
            {
                c.pos.x = static_cast<float>(windowSize.x) - halfW;
                c.vel.x = -c.vel.x * c.bounce;
                c.angularVelocity = -c.angularVelocity * 0.9f;
            }

            if (c.pos.y - halfH < 0.0f && c.vel.y < 0.0f)
            {
                c.pos.y = halfH;
                c.vel.y = -c.vel.y * c.bounce;
                c.vel.x *= 0.82f;
                c.angularVelocity *= 0.72f;
                if (std::abs(c.vel.y) < 45.0f)
                {
                    c.vel.y = 0.0f;
                }
            }
            else if (c.pos.y + halfH > static_cast<float>(windowSize.y) && c.vel.y > 0.0f)
            {
                c.pos.y = static_cast<float>(windowSize.y) - halfH;
                c.vel.y = -c.vel.y * c.bounce;
            }
        }

        confetti.erase(
            std::remove_if(confetti.begin(), confetti.end(), [](const ConfettiPiece& c) { return c.age >= c.lifetime; }),
            confetti.end());
    }
}

void AchievementManager::Unlock(Achievement& achievement)
{
    achievement.unlocked = true;
    notificationText     = achievement.message;
    notificationTimer    = notificationDuration;
    SpawnConfetti();
}

void AchievementManager::LoadConfettiAssets()
{
    confettiAssets.clear();

    constexpr const char* assetPaths[] = {
        "Assets/resources/wood_icon.png",
        "Assets/resources/rock_icon.png",
        "Assets/resources/steel_icon.png",
        "Assets/images/textures/IsoPlains.png",
        "Assets/images/textures/IsoWater.png",
        "Assets/images/road/roadUI_6432.png",
        "Assets/images/textures/Human.png",
        "Assets/images/building/house2.png",
        "Assets/images/building/workplace1.png",
        "Assets/images/building/park.png",
        "Assets/resources/Farm.png",
        "Assets/resources/Cafe.png",
        "Assets/resources/restaurant2.png",
    };

    for (const char* path : assetPaths)
    {
        try
        {
            auto texture = Dokev::LoadTexture(assets::locate_asset(path).string());
            if (texture.IsValid())
            {
                confettiAssets.push_back(texture);
            }
        }
        catch (const std::exception&)
        {
        }
    }
}

void AchievementManager::SpawnConfetti()
{
    auto windowSize = Engine::GetWindow().GetSize();

    confetti.clear();
    
    confetti.reserve(confettiCount);

    for (int i = 0; i < confettiCount; ++i)
    {
        ConfettiPiece piece;

        piece.pos.x = static_cast<float>(Dokev::GetRandomInt(0, windowSize.x - 1));
        piece.pos.y = static_cast<float>(windowSize.y) + static_cast<float>(Dokev::GetRandomInt(0, 80));

        piece.vel.x = static_cast<float>(Dokev::GetRandomInt(0, 720) - 360);
        piece.vel.y = -(static_cast<float>(120 + Dokev::GetRandomInt(0, 560)));

        piece.color = confettiColors[Dokev::GetRandomInt(0, colorCount - 1)];
        if (!confettiAssets.empty())
        {
            piece.assetIndex = static_cast<std::size_t>(Dokev::GetRandomInt(0, static_cast<int>(confettiAssets.size()) - 1));
        }
        piece.rotation        = static_cast<float>(Dokev::GetRandomInt(0, 359));
        piece.angularVelocity = static_cast<float>(Dokev::GetRandomInt(0, 1320) - 660);
        piece.baseScale       = static_cast<float>(Dokev::GetRandomInt(22, 48)) / 100.0f;
        piece.scale           = piece.baseScale;
        piece.lifetime        = static_cast<float>(Dokev::GetRandomInt(45, 72)) / 10.0f;
        piece.bounce          = static_cast<float>(Dokev::GetRandomInt(52, 78)) / 100.0f;

        confetti.push_back(piece);
    }
}

void AchievementManager::Draw() const
{
    if ((notificationTimer <= 0.f || notificationText.empty()) && confetti.empty())
        return;

    // Compute alpha: fully visible, fade out over last fadeOutTime seconds
    float alpha = 1.f;
    if (notificationTimer < fadeOutTime)
        alpha = std::clamp(notificationTimer / fadeOutTime, 0.0f, 1.0f);

    const auto        a     = static_cast<uint8_t>(alpha * 255.f);
    const CS200::RGBA textColor = (Dokev::ColorPalette::GOLD & 0xFFFFFF00u) | a;

    // Draw confetti pieces
    for (const auto& c : confetti)
    {
        const float pieceAlpha = std::clamp(1.0f - c.age / c.lifetime, 0.0f, 1.0f);
        const auto  pieceA     = static_cast<uint8_t>(pieceAlpha * 255.0f);
        if (!confettiAssets.empty() && c.assetIndex < confettiAssets.size())
        {
            const CS200::RGBA tint = (Dokev::ColorPalette::WHITE & 0xFFFFFF00u) | pieceA;
            confettiAssets[c.assetIndex].DrawEx(Math::vec2{ static_cast<double>(c.pos.x), static_cast<double>(c.pos.y) }, c.rotation, Math::vec2{ c.scale, c.scale }, tint);
        }
        else
        {
            const CS200::RGBA tint = (c.color & 0xFFFFFF00u) | pieceA;
            Dokev::DrawRectangleV(
                Math::vec2{ static_cast<double>(c.pos.x), static_cast<double>(c.pos.y) },
                Math::vec2{ 28.0 * c.scale, 36.0 * c.scale },
                tint);
        }
    }

    // Draw notification text
    if (notificationTimer > 0.f && !notificationText.empty())
    {
        auto windowSize = Engine::GetWindow().GetSize();
        const Math::fvec2 center = {
            static_cast<float>(windowSize.x) * 0.5f,
            static_cast<float>(windowSize.y) * 0.5f
        };

        CS230::FontManager::Instance().DrawText("MainFont", notificationText, center, CS230::TextAlign::Center, textColor);
    }
}
