/**
 * \file DKCityMetrics.hpp
 * \brief
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#pragma once

namespace Dokev
{
    class CityMetrics
    {
        float averageSatisfaction;    // 평균 만족도
        float environmentalPollution; // 환경 오염도 (0-100)
        float financialBalance;       // 재정 상태
        float populationDensity;      // 밀집도 (인구/거주지)

        // 거주지 관련
        int totalResidentialBuildings;
        int totalPopulation;
    };

}