/**
 * \file DKTimeManager.hpp
 * \brief Time management utility for resource production tracking
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#pragma once

namespace Dokev
{
    class Timer
    {
    private:
        float accumulator;
        float interval;
        bool  isActive;

    public:
        Timer(float intervalSeconds = 1.0f);

        void  Reset();
        void  SetInterval(float intervalSeconds);
        void  Start();
        void  Stop();
        bool  Update(float dt);
        float GetAccumulator() const;
        float GetInterval() const;
        bool  IsActive() const;
    };
}
