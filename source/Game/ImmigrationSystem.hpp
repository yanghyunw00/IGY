/**
 * \file ImmigrationSystem.hpp
 * \brief 
 * \author 
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include <algorithm>
#include <cstdlib>

class ImmigrationSystem
{
private:
    float immigrationTimer        = 0.0f;
    float baseImmigrationInterval = 10.0f; // check every 10sec

public:

    float CalculateImmigrationChance(int availableHomes, float happiness, int humanCount, int totalResources) const
    {
        float chance = 0.0f;

        // house > 0
        if (availableHomes <= 0)
            return 0.0f;

        if (happiness >= 80.0f)
            chance = 0.80f; // 80%
        else if (happiness >= 60.0f)
            chance = 0.50f; // 50%
        else if (happiness >= 40.0f)
            chance = 0.20f; // 20%
        else if (happiness >= 20.0f)
            chance = 0.05f; // 5%
        else
            chance = 0.0f; // 0% 

        // Resource bonus
        int requiredResources = humanCount * 100;
        if (totalResources >= requiredResources * 2)
        {
            chance += 0.10f; //+ 10%
        }

        //Is it big city?
        if (humanCount > 50)
        {
            chance += 0.05f; // + 5 %
        }

        return std::clamp(chance, 0.0f, 1.0f);
    }

    bool Update(float dt, int availableHomes, float happiness, int humanCount, int totalResources)
    {
        immigrationTimer += dt;

        if (immigrationTimer >= baseImmigrationInterval)
        {
            immigrationTimer = 0.0f;

            float chance = CalculateImmigrationChance(availableHomes, happiness, humanCount, totalResources);

            float random = static_cast<float>(rand()) / RAND_MAX;
            if (random < chance)
            {
                return true; 
            }
        }

        return false;
    }

    /**
     * \brief Get time until next immigration check (for UI)
     */
    float GetNextImmigrationTime() const { return baseImmigrationInterval - immigrationTimer; }

    /**
     * \brief Get base interval
     */
    float GetBaseInterval() const { return baseImmigrationInterval; }

    /**
     * \brief Set base interval 
     */
    void SetBaseInterval(float interval) { baseImmigrationInterval = interval; }

    /**
     * \brief Reset timer
     */
    void ResetTimer() { immigrationTimer = 0.0f; }
};
