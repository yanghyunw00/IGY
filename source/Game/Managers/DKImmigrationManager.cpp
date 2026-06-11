/**
 * \file DKImmigrationManager.cpp
 * \brief
 * \author
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "DKImmigrationManager.hpp"
#include "Dokev/Core/DKUtility.hpp"

namespace Dokev
{
    float ImmigrationManager::CalculateImmigrationChance(int availableHomes, float happiness, int humanCount, int totalResources) const
    {
        float chance = 0.0f;

        // house > 0
        if (availableHomes <= 0)
            return 0.0f;

        if (happiness >= 80.0f)
            chance = 0.90f; // 90%
        else if (happiness >= 60.0f)
            chance = 0.75f; // 75%
        else if (happiness >= 50.0f)
            chance = 0.65f; // 65%
        else if (happiness >= 40.0f)
            chance = 0.40f; // 40%
        else if (happiness >= 20.0f)
            chance = 0.15f; // 15%
        else
            chance = 0.0f; // 0%

        // Resource bonus
        int requiredResources = humanCount * 100;
        if (totalResources >= requiredResources * 2)
        {
            chance += 0.10f; //+ 10%
        }

        // Is it big city?
        if (humanCount > 50)
        {
            chance += 0.05f; // + 5 %
        }

        return std::clamp(chance, 0.0f, 1.0f);
    }

    bool ImmigrationManager::Update(float dt, int availableHomes, float happiness, int humanCount, int totalResources)
    {
        immigrationTimer += dt;

        if (immigrationTimer >= baseImmigrationInterval)
        {
            immigrationTimer = 0.0f;

            float chance = CalculateImmigrationChance(availableHomes, happiness, humanCount, totalResources);

            if (Dokev::ChanceRoll(chance))
            {
                return true;
            }
        }

        return false;
    }
}
