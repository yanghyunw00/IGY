/**
 * \file HappinessManager.hpp
 * \brief
 * \author
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include <algorithm>

class HappinessManager
{
public:
    struct HappinessFactors
    {
        float housingScore    = 100.0f; // house
        float employmentScore = 100.0f; // workshop
        float waterScore      = 100.0f; // water
    };

    HappinessFactors factors;

    float CalculateTotalHappiness() const
    {
        float total = 0.0f;
        // Housing: 35%
        // Employment: 35%
        // Water: 30%
        total += factors.housingScore * 0.35f;
        total += factors.employmentScore * 0.35f;
        total += factors.waterScore * 0.30f;

        return total;
    }

    void UpdateFactors(int humanCount, int homeCapacity, int workplaceCount, int housesWithWater, int totalHouses)
    {
        // housing
        float housingRatio = (float)homeCapacity / std::max(1, humanCount);

        if (housingRatio >= 1.5f)
            factors.housingScore = 100.0f; // 100%
        else if (housingRatio >= 1.0f)
            factors.housingScore = 80.0f; // 80%
        else if (housingRatio >= 0.7f)
            factors.housingScore = 50.0f; // 50%
        else if (housingRatio >= 0.5f)
            factors.housingScore = 30.0f; // 30%
        else
            factors.housingScore = 10.0f;

        // workshop
        float jobRatio = (float)workplaceCount / std::max(1, humanCount);

        if (jobRatio >= 0.8f && jobRatio <= 1.2f)
            factors.employmentScore = 100.0f; // ideal 80~120%
        else if (jobRatio >= 0.5f && jobRatio <= 1.5f)
            factors.employmentScore = 70.0f; // 70%
        else if (jobRatio >= 0.3f)
            factors.employmentScore = 40.0f; // 40%
        else
            factors.employmentScore = 20.0f;

        // water
        if (totalHouses == 0)
        {
            factors.waterScore = 100.0f; // no house, no water
        }
        else
        {
            float waterCoverage = (float)housesWithWater / totalHouses;
            factors.waterScore  = waterCoverage * 100.0f;
        }
    }

    HappinessFactors GetFactors() const
    {
        return factors;
    }
};
