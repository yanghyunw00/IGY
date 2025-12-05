#include "DKImmigrationSystem.hpp"

namespace Dokev
{
    float ImmigrationSystem::CalculateImmigrationChance(int availableHomes, float happiness, int humanCount, int totalResources) const
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

        // Is it big city?
        if (humanCount > 50)
        {
            chance += 0.05f; // + 5 %
        }

        return std::clamp(chance, 0.0f, 1.0f);
    }

    bool ImmigrationSystem::Update(float dt, int availableHomes, float happiness, int humanCount, int totalResources)
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
}
