/**
 * \file WeatherCycleManager.hpp
 * \brief Weather state machine: Clear -> Cloudy -> Rain -> Thunder
 * \author Dongyun Lee
 * \date 2026 Spring
 */
#pragma once
#include "CS200/RGBA.hpp"

enum class WeatherType
{
    Clear,
    Cloudy,
    Rain,
    Thunder
};

class WeatherCycleManager
{
public:
    void Update(float dt);
    void SetWeather(WeatherType w);

    WeatherType GetCurrent()             const { return current; }
    CS200::RGBA GetOverlayColor()        const;
    float       GetRainIntensity()       const;  // 0.0 ~ 1.0
    float       GetCloudShadowOpacity()  const;  // 0.0 ~ 1.0
    bool        TryConsumeThunderFlash();        // true on the frame a lightning strike begins

    // Transition probability table [from][to]: Clear, Cloudy, Rain, Thunder
    static constexpr float kTransitionTable[4][4] = {
        { 0.40f, 0.50f, 0.10f, 0.00f },  // from Clear
        { 0.30f, 0.20f, 0.40f, 0.10f },  // from Cloudy
        { 0.10f, 0.25f, 0.35f, 0.30f },  // from Rain
        { 0.00f, 0.20f, 0.60f, 0.20f },  // from Thunder
    };

private:
    WeatherType PickNextWeather() const;
    float       PickDuration(WeatherType w) const;

    WeatherType previous           = WeatherType::Clear;
    WeatherType current            = WeatherType::Clear;
    WeatherType next               = WeatherType::Cloudy;
    float       weatherTimer       = 0.f;
    float       weatherDuration    = 45.f;
    float       transitionTimer    = 6.f;  // start fully blended in
    float       transitionDuration = 6.f;

    // Thunder flash state
    float thunderCooldown   = 0.f;   // countdown to next lightning
    float flashTimer        = 0.f;   // > 0 while flash is visible
    bool  pendingFlash      = false;

    static constexpr float kFlashDuration = 0.12f;
};
