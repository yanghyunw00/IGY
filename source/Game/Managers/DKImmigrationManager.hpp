/**
 * \file DKImmigrationManager.hpp
 * \brief
 * \author
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include <algorithm>
#include <cstdlib>

namespace Dokev
{
    class ImmigrationManager
    {
    private:
        mutable float immigrationTimer        = 0.0f;
        mutable float baseImmigrationInterval = 10.0f; // check every 10sec

    public:

        float CalculateImmigrationChance(int availableHomes, float happiness, int humanCount, int totalResources) const;

        bool Update(float dt, int availableHomes, float happiness, int humanCount, int totalResources);

        /**
         * \brief Get time until next immigration check (for UI)
         */
        float GetNextImmigrationTime() const
        {
            return baseImmigrationInterval - immigrationTimer;
        }

        /**
         * \brief Get base interval
         */
        float GetBaseInterval() const
        {
            return baseImmigrationInterval;
        }

        /**
         * \brief Set base interval
         */
        void SetBaseInterval(float interval)
        {
            baseImmigrationInterval = interval;
        }

        /**
         * \brief Reset timer
         */
        void ResetTimer()
        {
            immigrationTimer = 0.0f;
        }
    };

}