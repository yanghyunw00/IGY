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
    };

    HappinessFactors factors;

    float CalculateTotalHappiness() const
    {
        float total = 0.0f;
        // Housing: 50%
        // Employment: 50%
        total += factors.housingScore * 0.50f;
        total += factors.employmentScore * 0.50f;

        return total;
    }

    void UpdateFactors(int humanCount, int homeCapacity, int workplaceCount)
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

        // workshop
        float jobRatio = static_cast<float>(workplaceCount) / std::max(1, humanCount);

        if (jobRatio >= 0.8f && jobRatio <= 1.2f)
            factors.employmentScore = 100.0f;
        else if (jobRatio >= 0.5f && jobRatio <= 1.5f)
            factors.employmentScore = 70.0f;
        else if (jobRatio >= 0.3f)
            factors.employmentScore = 40.0f;
        else
            factors.employmentScore = 20.0f;
    }

    HappinessFactors GetFactors() const
    {
        return factors;
    }
};
