/**
 * \file DayCycleManager.hpp
 * \brief 5 real-time minutes = 1 in-game day. Tracks time of day and provides sky overlay color.
 */
#pragma once
#include "CS200/RGBA.hpp"

class DayCycleManager
{
public:
    // 5 real minutes at 1x simulation speed = 1 game day
    static constexpr float kDayDuration = 300.0f;

    void Update(float simulationDt);
    void SetTime(int hour, int minute);

    int         GetCurrentDay()    const { return currentDay; }
    float       GetDayProgress()   const { return dayTimer / kDayDuration; }
    int         GetHour()          const { return static_cast<int>(GetDayProgress() * 24.f); }
    int         GetMinute()        const { return static_cast<int>(GetDayProgress() * 24.f * 60.f) % 60; }
    bool        IsWorkingHours()   const { const int h = GetHour(); return h >= 9 && h < 17; }
    const char* GetPhaseName()     const;
    CS200::RGBA GetOverlayColor()  const;

private:
    // Start at 7:00 AM so the player sees morning on day 1
    float dayTimer   = (7.0f / 24.0f) * kDayDuration;
    int   currentDay = 1;
};
