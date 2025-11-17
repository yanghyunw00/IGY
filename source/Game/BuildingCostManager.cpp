/**
 * \file BuildingCostManager.cpp
 * \brief Implementation of building cost management system
 * \author Dongyun Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "BuildingCostManager.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Logger.hpp"
#include "Engine/Path.hpp"
#include <fstream>
  
bool BuildingCostManager::LoadFromFile(const std::string& filepath)
{
    try
    {
        auto fullPath = assets::locate_asset(filepath);
        std::ifstream file(fullPath.string());

        if (!file.is_open())
        {
            Engine::GetLogger().LogError("BuildingCostManager: Failed to open file: " + filepath);
            return false;
        }

        nlohmann::json j;
        file >> j;

        if (!j.contains("BuildingCosts"))
        {
            Engine::GetLogger().LogError("BuildingCostManager: JSON missing 'BuildingCosts' key");
            return false;
        }

        costs.clear();

        for (auto& [key, value] : j["BuildingCosts"].items())
        {
            BuildingType type = StringToBuildingType(key);

            if (type == BuildingType::NONE)
            {
                Engine::GetLogger().LogVerbose("BuildingCostManager: Unknown building type: " + key);
                continue;
            }

            BuildingCost cost;
            cost.wood  = value.value("Wood", 0);
            cost.stone = value.value("Stone", 0);
            cost.steel = value.value("Steel", 0);

            costs[type] = cost;
        }

        Engine::GetLogger().LogEvent("BuildingCostManager: Loaded " + std::to_string(costs.size()) + " building costs from " + filepath);
        return true;
    }
    catch (const std::exception& e)
    {
        Engine::GetLogger().LogError("BuildingCostManager: Exception while loading: " + std::string(e.what()));
        return false;
    }
}

BuildingCost BuildingCostManager::GetCost(BuildingType type) const
{
    auto it = costs.find(type);
    if (it != costs.end())
    {
        return it->second;
    }
    return BuildingCost(); 
}

bool BuildingCostManager::CanAfford(BuildingType type, int playerWood, int playerStone, int playerSteel) const
{
    BuildingCost cost = GetCost(type);
    return playerWood >= cost.wood && playerStone >= cost.stone && playerSteel >= cost.steel;
}

BuildingType BuildingCostManager::StringToBuildingType(const std::string& str) const
{
    if (str == "ROAD")            return BuildingType::ROAD;
    if (str == "HOME")            return BuildingType::HOME;
    if (str == "WOOD_WORKSHOP")   return BuildingType::WOOD_WORKSHOP;
    if (str == "ROCK_WORKSHOP")   return BuildingType::ROCK_WORKSHOP;
    if (str == "STEEL_WORKSHOP")  return BuildingType::STEEL_WORKSHOP;
    return BuildingType::NONE;
}
