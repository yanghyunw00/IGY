/**
 * \file DKCityMetrics.hpp
 * \brief City metrics including happiness and immigration systems
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#pragma once
#include "../Managers/HappinessManager.hpp"
#include "DKImmigrationManager.hpp"
#include "Dokev/GameObject/DKComponent.hpp"


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
        HappinessManager   happinessManager;
        ImmigrationManager immigrationManager;

        float averageHappiness      = 100.0f; // Average satisfaction level
        float happinessChangeRate   = 0.0f;   // Rate of change per second (deprecated)
        int   parkCount             = 0;      // Built parks, refreshed each UpdateSystems
        float pentHouseHappyBonus   = 0.0f;   // Permanent happiness bonus from built penthouses
        float debugHappinessOverride = -1.0f; // -1 = disabled; 0-100 = force this value (editor only)

        // Win condition
        int winConditionPopulation = 20;

    public:

        bool UpdateSystems(double dt, GameObjectManager* gameObjectManager, ResourceManager* resourceManager);

        void Update(double dt) override;

        float GetHappiness() const
        {
            return averageHappiness;
        }

        int GetParkCount() const
        {
            return parkCount;
        }

        void AddPentHouseHappyBonus(float amount)
        {
            pentHouseHappyBonus += amount;
        }

        float GetPentHouseHappyBonus() const
        {
            return pentHouseHappyBonus;
        }

        void SetDebugHappiness(float v)
        {
            debugHappinessOverride = v;
        }

        float GetDebugHappiness() const
        {
            return debugHappinessOverride;
        }

        void SetHappinessChangeRate(float rate)
        {
            happinessChangeRate = rate;
        }

        float GetHappinessChangeRate() const
        {
            return happinessChangeRate;
        }

        HappinessManager* GetHappinessManager()
        {
            return &happinessManager;
        }

        ImmigrationManager* GetImmigrationManager()
        {
            return &immigrationManager;
        }

        // Win condition
        int GetWinConditionPopulation() const
        {
            return winConditionPopulation;
        }

        void SetWinConditionPopulation(int population)
        {
            winConditionPopulation = population;
        }

        bool IsWinConditionMet(int currentPopulation) const
        {
            return currentPopulation >= winConditionPopulation;
        }
    };

}