/**
 * \file AchievementManager.hpp
 * \brief Tracks and displays achievement unlocks with confetti
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#pragma once

#include "CS200/RGBA.hpp"
#include "Dokev/Core/DKColor.hpp"
#include "Dokev/Core/DKMath.hpp"
#include "Dokev/Graphics/DKTexture.hpp"
#include <string>
#include <vector>

struct Achievement
{
    std::string id;
    std::string message;
    bool        unlocked = false;
};

struct ConfettiPiece
{
    Math::fvec2 pos;
    Math::fvec2 vel;
    CS200::RGBA color;
    std::size_t assetIndex      = 0;
    float       rotation        = 0.0f;
    float       angularVelocity = 0.0f;
    float       scale           = 0.32f;
    float       baseScale       = 0.32f;
    float       age             = 0.0f;
    float       lifetime        = 6.0f;
    float       bounce          = 0.65f;
};

class AchievementManager
{
public:
    void Init();
    void Update(float dt, int population, int buildingCount, float happiness, int maxSingleResource);
    void Draw() const;

private:
    void Unlock(Achievement& achievement);
    void LoadConfettiAssets();
    void SpawnConfetti();

    std::vector<Achievement>  achievements;
    std::vector<ConfettiPiece> confetti;
    std::vector<Dokev::Texture2D> confettiAssets;

    std::string            notificationText;
    float                  notificationTimer        = 0.f;
    static constexpr float notificationDuration     = 4.f;
    static constexpr float fadeOutTime              = 0.75f;
    static constexpr int   confettiCount            = 130;
};
