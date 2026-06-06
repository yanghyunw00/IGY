/**
 * \file DayCycleManager.cpp
 */
#include "DayCycleManager.hpp"
#include "Dokev/Core/DKColor.hpp"

void DayCycleManager::Update(float simulationDt)
{
    dayTimer += simulationDt;
    while (dayTimer >= kDayDuration)
    {
        dayTimer -= kDayDuration;
        ++currentDay;
    }
}

void DayCycleManager::SetTime(int hour, int minute)
{
    hour   = (hour   % 24 + 24) % 24;
    minute = (minute % 60 + 60) % 60;
    dayTimer = (static_cast<float>(hour) + static_cast<float>(minute) / 60.0f) / 24.0f * kDayDuration;
}

const char* DayCycleManager::GetPhaseName() const
{
    const int hour = GetHour();
    if (hour >= 4  && hour < 6)  return "Dawn";
    if (hour >= 6  && hour < 12) return "Morning";
    if (hour >= 12 && hour < 18) return "Afternoon";
    if (hour >= 18 && hour < 22) return "Evening";
    return "Night";
}

// Night (0-4)  : NAVY full dark
// Dawn  (4-6)  : NAVY -> transparent
// Day   (6-20) : transparent  (낮이 밤보다 훨씬 길다)
// Dusk  (20-22): transparent -> ORANGE tint
// Night (22-24): ORANGE -> NAVY
CS200::RGBA DayCycleManager::GetOverlayColor() const
{
    using namespace Dokev::ColorPalette;

    const float hour = GetDayProgress() * 24.f;

    const CS200::RGBA nightColor  = Dokev::ColorAlpha(NAVY,   0.55f);
    const CS200::RGBA duskColor   = Dokev::ColorAlpha(ORANGE, 0.35f);
    const CS200::RGBA dawnFadeOut = Dokev::ColorAlpha(ORANGE, 0.00f);

    if (hour < 4.f)
    {
        // Night 0-4: full dark
        return nightColor;
    }
    else if (hour < 6.f)
    {
        // Dawn 4-6: night -> transparent
        const float t = (hour - 4.f) / 2.f;
        return Dokev::ColorLerp(nightColor, dawnFadeOut, t);
    }
    else if (hour < 20.f)
    {
        // Day 6-20: clear (14 hours of daytime)
        return CS200::CLEAR;
    }
    else if (hour < 22.f)
    {
        // Dusk 20-22: transparent -> orange tint
        const float t = (hour - 20.f) / 2.f;
        return Dokev::ColorLerp(CS200::CLEAR, duskColor, t);
    }
    else
    {
        // Night 22-24: orange tint -> dark navy
        const float t = (hour - 22.f) / 2.f;
        return Dokev::ColorLerp(duskColor, nightColor, t);
    }
}
