/**
 * \file Building.cpp
 * \brief Base class for all building objcect
 * \author Dongyun Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "Building.hpp"

Building::Building(Math::fvec2 pos, int w, int h)
    : gridPosition(pos)
    , width(w)
    , height(h)
    , isBuilt(false)
    , max_capacity(0)
    , current_capacity(0)
    , cost(0)
{
    texture = Dokev::LoadTexture("Assets/images/textures/MissingTexture.png");
    progress = 1.0f; // TODO: after finishing time manager fix this to 0
}

Building::~Building() = default;

int Building::GetCost() const
{
    return cost;
}

Math::fvec2 Building::GetGrid() const
{
    return gridPosition;
}

bool Building::GetIsBuilt() const
{
    return isBuilt;
}

unsigned int Building::GetMaxCapacity() const
{
    return max_capacity;
}

unsigned int Building::GetCurrentCapacity() const
{
    return current_capacity;
}

float Building::GetProgress() const
{
    return progress;
}

BuildingType Building::GetBuildingType() const
{
    return buildingType;
}