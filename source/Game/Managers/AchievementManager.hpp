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
};

class AchievementManager
{
public:
    void Init();
    void Update(float dt, int population, int buildingCount, float happiness, int maxSingleResource);
    void Draw() const;

private:
    void Unlock(Achievement& achievement);
    void SpawnConfetti();

    std::vector<Achievement>  achievements;
    std::vector<ConfettiPiece> confetti;

    std::string            notificationText;
    float                  notificationTimer        = 0.f;
    static constexpr float notificationDuration     = 3.f;
    static constexpr float fadeOutTime              = 0.5f;
    static constexpr int   confettiCount            = 80;
};
