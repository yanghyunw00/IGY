#include "DKCityMetrics.hpp"
#include "../Common/BuildingType.hpp"
#include "../Entities/Building.hpp"
#include "../Entities/ResidentBuilding.hpp"
#include "../Managers/ResourceManager.hpp"
#include "Dokev/Core/DKMetaProgramming.hpp"
#include "Dokev/Managers/DKGameObjectManager.hpp"


namespace Dokev
{
    bool CityMetrics::UpdateSystems(double dt, GameObjectManager* gameObjectManager, ResourceManager* resourceManager)
    {
        if (!gameObjectManager || !resourceManager)
            return false;

        // Gather game state data
        int humanCount      = static_cast<int>(gameObjectManager->GetHumans().size());
        int homeCapacity    = 0;
        int workplaceCount  = 0;
        int streetLightCount = 0;
        parkCount            = 0;

        for (const auto& building : gameObjectManager->GetBuildings())
        {
            BuildingType type = building->GetBuildingType();

            if (Meta::is_residential_type(type))
            {
                homeCapacity += building->GetMaxCapacity();
            }

            if (Meta::is_workshop_type(type))
            {
                workplaceCount++;
            }

            if (type == BuildingType::STREET_LIGHT && building->GetIsBuilt())
            {
                streetLightCount++;
            }

            if (type == BuildingType::PARK && building->GetIsBuilt())
            {
                parkCount++;
            }
        }

        // Update Happiness Manager
        happinessManager.UpdateFactors(humanCount, homeCapacity, workplaceCount, streetLightCount);
        averageHappiness = happinessManager.CalculateTotalHappiness();

        constexpr float parkHappinessPerPark = 0.5f;
        constexpr float parkHappinessCap     = 15.0f;
        averageHappiness = std::min(100.0f, averageHappiness + std::min(parkHappinessCap, parkCount * parkHappinessPerPark));

        //  Update Immigration Manager
        int availableHomes = homeCapacity - humanCount;
        int totalResources = resourceManager->GetTotalResources();

        bool shouldSpawnImmigrant = immigrationManager.Update(static_cast<float>(dt), availableHomes, averageHappiness, humanCount, totalResources);

        return shouldSpawnImmigrant;
    }

    void CityMetrics::Update(double dt)
    {
        averageHappiness += happinessChangeRate * 0.3f * static_cast<float>(dt);

        if (averageHappiness < 0.0f)
            averageHappiness = 0.0f;
        if (averageHappiness > 100.0f)
            averageHappiness = 100.0f;
    }
}