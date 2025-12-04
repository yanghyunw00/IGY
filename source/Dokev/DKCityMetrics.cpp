#include "DKCityMetrics.hpp"
#include "DKGameObjectManager.hpp"
#include "Game/ResourceManager.hpp"
#include "Game/Building.hpp"
#include "Game/BuildingType.hpp"
#include "Game/ResidentBuilding.hpp"
#include "DKMetaProgramming.hpp"

namespace Dokev
{
    bool CityMetrics::UpdateSystems(double dt, GameObjectManager* gameObjectManager, ResourceManager* resourceManager)
    {
        if (!gameObjectManager || !resourceManager)
            return false;

        //Gather game state data
        int humanCount    = static_cast<int>(gameObjectManager->GetHumans().size());
        int homeCapacity  = 0;
        int workplaceCount = 0;
        int totalHouses   = 0;
        int housesWithWater = 0;

        for (const auto& building : gameObjectManager->GetBuildings())
        {
            BuildingType type = building->GetBuildingType();

            if (Meta::is_residential_type(type))
            {
                totalHouses++;
                homeCapacity += building->GetMaxCapacity();

                // Count houses with water (TODO: implement when water system is ready)
                // For now, assume all houses have water
                housesWithWater++;
            }

            if (Meta::is_workshop_type(type))
            {
                workplaceCount++;
            }
        }

        // Update Happiness System 
        happinessSystem.UpdateFactors(humanCount, homeCapacity, workplaceCount, housesWithWater, totalHouses);
        averageHappiness = happinessSystem.CalculateTotalHappiness();

        //  Update Immigration System 
        int availableHomes = homeCapacity - humanCount;
        int totalResources = resourceManager->GetTotalResources();

        bool shouldSpawnImmigrant = immigrationSystem.Update(static_cast<float>(dt), availableHomes, averageHappiness,
                                                             humanCount, totalResources);

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