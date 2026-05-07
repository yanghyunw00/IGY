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
    if (hour >= 5  && hour < 7)  return "Dawn";
    if (hour >= 7  && hour < 12) return "Morning";
    if (hour >= 12 && hour < 17) return "Afternoon";
    if (hour >= 17 && hour < 20) return "Evening";
    return "Night";
}

// Night  : NAVY,   alpha 0.55
// Dawn   : NAVY -> transparent (orange tint fades out)
// Day    : transparent
// Evening: transparent -> ORANGE alpha 0.35
// Night  : ORANGE -> NAVY
CS200::RGBA DayCycleManager::GetOverlayColor() const
{
    using namespace Dokev::ColorPalette;

    const float hour = GetDayProgress() * 24.f;

    const CS200::RGBA nightColor  = Dokev::ColorAlpha(NAVY,   0.55f);
    const CS200::RGBA duskColor   = Dokev::ColorAlpha(ORANGE, 0.35f);
    const CS200::RGBA dawnFadeOut = Dokev::ColorAlpha(ORANGE, 0.00f);

    if (hour < 5.f)
    {
        return nightColor;
    }
    else if (hour < 7.f)
    {
        // Dawn 5-7: night -> transparent
        const float t = (hour - 5.f) / 2.f;
        return Dokev::ColorLerp(nightColor, dawnFadeOut, t);
    }
    else if (hour < 17.f)
    {
        return CS200::CLEAR;
    }
    else if (hour < 20.f)
    {
        // Evening 17-20: transparent -> orange tint
        const float t = (hour - 17.f) / 3.f;
        return Dokev::ColorLerp(CS200::CLEAR, duskColor, t);
    }
    else
    {
        // Night 20-24: orange tint -> dark navy
        const float t = (hour - 20.f) / 4.f;
        return Dokev::ColorLerp(duskColor, nightColor, t);
    }
}
