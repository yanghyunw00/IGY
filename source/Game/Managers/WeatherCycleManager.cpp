/**
 * \file WeatherCycleManager.cpp
 * \author Dongyun Lee
 * \date 2026 Spring
 */
#include "WeatherCycleManager.hpp"
#include "Dokev/Core/DKColor.hpp"
#include <algorithm>
#include <cstdlib>

static float RandFloat()
{
    return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
}

static CS200::RGBA TargetOverlay(WeatherType w)
{
    using namespace Dokev::ColorPalette;
    switch (w)
    {
        case WeatherType::Clear:   return CS200::CLEAR;
        case WeatherType::Cloudy:  return Dokev::ColorAlpha(GRAY,     0.20f);
        case WeatherType::Rain:    return Dokev::ColorAlpha(DARKGRAY,  0.30f);
        case WeatherType::Thunder: return Dokev::ColorAlpha(NAVY,      0.50f);
        default:                   return CS200::CLEAR;
    }
}

static float TargetRainIntensity(WeatherType w)
{
    switch (w)
    {
        case WeatherType::Rain:    return 0.6f;
        case WeatherType::Thunder: return 1.0f;
        default:                   return 0.0f;
    }
}

void WeatherCycleManager::Update(float dt)
{
    weatherTimer += dt;

    if (weatherTimer >= weatherDuration)
    {
        weatherTimer    = 0.f;
        previous        = current;
        current         = next;
        next            = PickNextWeather();
        weatherDuration = PickDuration(current);
        transitionTimer = 0.f;
    }

    if (transitionTimer < transitionDuration)
        transitionTimer = std::min(transitionTimer + dt, transitionDuration);

    // Thunder flash scheduling
    if (current == WeatherType::Thunder)
    {
        thunderCooldown -= dt;
        if (thunderCooldown <= 0.f)
        {
            thunderCooldown = 3.f + RandFloat() * 5.f;
            flashTimer      = kFlashDuration;
            pendingFlash    = true;
        }
    }
    else
    {
        thunderCooldown = 3.f + RandFloat() * 5.f;
    }

    if (flashTimer > 0.f)
        flashTimer -= dt;
}

WeatherType WeatherCycleManager::PickNextWeather() const
{
    const int   from         = static_cast<int>(current);
    const float r            = RandFloat();
    float       accumulated  = 0.f;
    for (int to = 0; to < 4; ++to)
    {
        accumulated += kTransitionTable[from][to];
        if (r < accumulated)
            return static_cast<WeatherType>(to);
    }
    return WeatherType::Clear;
}

float WeatherCycleManager::PickDuration(WeatherType w) const
{
    switch (w)
    {
        case WeatherType::Clear:   return 30.f + RandFloat() * 60.f;
        case WeatherType::Cloudy:  return 20.f + RandFloat() * 30.f;
        case WeatherType::Rain:    return 25.f + RandFloat() * 35.f;
        case WeatherType::Thunder: return 10.f + RandFloat() * 20.f;
        default:                   return 30.f;
    }
}

CS200::RGBA WeatherCycleManager::GetOverlayColor() const
{
    // Lightning flash overrides everything
    if (flashTimer > 0.f)
    {
        float alpha = (flashTimer / kFlashDuration) * 0.75f;
        return Dokev::ColorAlpha(Dokev::ColorPalette::WHITE, alpha);
    }

    const float t = std::min(transitionTimer / transitionDuration, 1.f);
    return Dokev::ColorLerp(TargetOverlay(previous), TargetOverlay(current), t);
}

float WeatherCycleManager::GetRainIntensity() const
{
    const float t = std::min(transitionTimer / transitionDuration, 1.f);
    float from    = TargetRainIntensity(previous);
    float to      = TargetRainIntensity(current);
    return from + (to - from) * t;
}

void WeatherCycleManager::SetWeather(WeatherType w)
{
    previous        = current;
    current         = w;
    next            = PickNextWeather();
    weatherTimer    = 0.f;
    weatherDuration = PickDuration(current);
    transitionTimer = 0.f;
    flashTimer      = 0.f;
    pendingFlash    = false;
}

bool WeatherCycleManager::TryConsumeThunderFlash()
{
    if (pendingFlash)
    {
        pendingFlash = false;
        return true;
    }
    return false;
}
