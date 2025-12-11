/**
 * \file BuildingCostManager.hpp
 * \brief Manages building construction costs loaded from JSON
 * \author Dongyun Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include "../Common/BuildingType.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

struct BuildingCost
{
    int wood  = 0;
    int stone = 0;
    int steel = 0;

    BuildingCost() = default;
    BuildingCost(int w, int st, int stl) : wood(w), stone(st), steel(stl) {}
};

class BuildingCostManager
{
public:
    BuildingCostManager() = default;

    bool LoadFromFile(const std::string& filepath);

    BuildingCost GetCost(BuildingType type) const;

    bool CanAfford(BuildingType type, int playerWood, int playerStone, int playerSteel) const;

private:
    std::unordered_map<BuildingType, BuildingCost> costs;

    BuildingType StringToBuildingType(const std::string& str) const;
};
