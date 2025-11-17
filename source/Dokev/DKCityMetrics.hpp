/**
 * \file DKCityMetrics.hpp
 * \brief
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#pragma once
#include "Dokev/DKComponent.hpp"

namespace Dokev
{
    class CityMetrics : public IComponent
    {
        float averageHappiness = 100.0f;       // 평균 만족도
        float happinessChangeRate = 0.0f;      // 초당 증가/감소율 (양수=증가, 음수=감소)

        float environmentalPollution; // 환경 오염도 (0-100)
        float financialBalance;       // 재정 상태
        float populationDensity;      // 밀집도 (인구/거주지)

        // 거주지 관련
        int totalResidentialBuildings;
        int totalPopulation;

    public:
        void Update(double dt);

        float GetHappiness() const
        {
            return averageHappiness;
        }

        // 만족도 변화율 설정 (양수=증가, 음수=감소)
        void SetHappinessChangeRate(float rate)
        {
            happinessChangeRate = rate;
        }

        float GetHappinessChangeRate() const
        {
            return happinessChangeRate;
        }
    };

}