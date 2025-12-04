/**
 * \file DKCityMetrics.hpp
 * \brief City metrics including happiness and immigration systems
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#pragma once
#include "Dokev/DKComponent.hpp"
#include "Game/HappinessSystem.hpp"
#include "Game/ImmigrationSystem.hpp"

namespace Dokev
{
    // Forward declarations
    class GameObjectManager;
}

class ResourceManager;

namespace Dokev
{
    class CityMetrics : public IComponent
    {
    private:
        HappinessSystem   happinessSystem;
        ImmigrationSystem immigrationSystem;

        float averageHappiness    = 100.0f; // 평균 만족도
        float happinessChangeRate = 0.0f;   // 초당 증가/감소율 (deprecated)

        float environmentalPollution; // 환경 오염도 (0-100)
        float financialBalance;       // 재정 상태
        float populationDensity;      // 밀집도 (인구/거주지)

        int totalResidentialBuildings;
        int totalPopulation;

    public:

        bool UpdateSystems(double dt, GameObjectManager* gameObjectManager, ResourceManager* resourceManager);

        void Update(double dt) override;

        float GetHappiness() const { return averageHappiness; }

        void SetHappinessChangeRate(float rate) { happinessChangeRate = rate; }

        float GetHappinessChangeRate() const { return happinessChangeRate; }

        HappinessSystem*   GetHappinessSystem() { return &happinessSystem; }
        ImmigrationSystem* GetImmigrationSystem() { return &immigrationSystem; }
    };

}