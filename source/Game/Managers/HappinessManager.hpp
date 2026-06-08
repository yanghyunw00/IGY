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
        float housingScore     = 100.0f; // house
        float employmentScore  = 100.0f; // workshop
        float nightSafetyScore = 100.0f; // street light
    };

    HappinessFactors factors;

    float CalculateTotalHappiness() const
    {
        float total = 0.0f;
        // Housing: 40%, Employment: 40%, Night Safety: 20%
        total += factors.housingScore     * 0.40f;
        total += factors.employmentScore  * 0.40f;
        total += factors.nightSafetyScore * 0.20f;

        return total;
    }

    void UpdateFactors(int humanCount, int homeCapacity, int workplaceCount, int streetLightCount)
    {
        // housing
        float housingRatio = static_cast<float>(homeCapacity) / std::max(1, humanCount);

        if (housingRatio >= 1.5f)
            factors.housingScore = 100.0f;
        else if (housingRatio >= 1.0f)
            factors.housingScore = 80.0f;
        else if (housingRatio >= 0.7f)
            factors.housingScore = 50.0f;
        else if (housingRatio >= 0.5f)
            factors.housingScore = 30.0f;
        else
            factors.housingScore = 10.0f;

        float jobRatio = static_cast<float>(workplaceCount) / std::max(1, humanCount);

        if (jobRatio >= 0.8f)
            factors.employmentScore = 100.0f;
        else if (jobRatio >= 0.5f)
            factors.employmentScore = 70.0f;
        else if (jobRatio >= 0.3f)
            factors.employmentScore = 40.0f;
        else
            factors.employmentScore = 20.0f;

        // street light — 1 light per 5 people is ideal
        if (humanCount == 0)
        {
            factors.nightSafetyScore = 100.0f;
        }
        else
        {
            float lightRatio = static_cast<float>(streetLightCount) / static_cast<float>(humanCount);
            if (lightRatio >= 0.2f)
                factors.nightSafetyScore = 100.0f;
            else if (lightRatio >= 0.1f)
                factors.nightSafetyScore = 70.0f;
            else if (lightRatio >= 0.05f)
                factors.nightSafetyScore = 40.0f;
            else
                factors.nightSafetyScore = 20.0f;
        }
    }

    HappinessFactors GetFactors() const
    {
        return factors;
    }
};
