/**
 * \file DKTimeManager.cpp
 * \brief Time management utility for resource production tracking
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "DKTimeManager.hpp"

namespace Dokev
{
    Timer::Timer(float intervalSeconds)
        : accumulator(0.0f), interval(intervalSeconds), isActive(true)
    {
    }

    void Timer::Reset()
    {
        accumulator = 0.0f;
    }

    void Timer::SetInterval(float intervalSeconds)
    {
        interval = intervalSeconds;
    }

    void Timer::Start()
    {
        isActive = true;
    }

    void Timer::Stop()
    {
        isActive = false;
    }

    bool Timer::Update(float dt)
    {
        if (!isActive)
        {
            return false;
        }

        accumulator += dt;

        if (accumulator >= interval)
        {
            accumulator -= interval;
            return true;
        }

        return false;
    }

    float Timer::GetAccumulator() const
    {
        return accumulator;
    }

    float Timer::GetInterval() const
    {
        return interval;
    }

    bool Timer::IsActive() const
    {
        return isActive;
    }
}
