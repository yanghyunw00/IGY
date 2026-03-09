/**
 * \file WorkBuilding.cpp
 * \brief Derived class of Building, manages workplace jobs and production
 * \author Dongyun Lee
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "WorkBuilding.hpp"
#include "../Managers/ResourceManager.hpp"
#include "CS200/RGBA.hpp"
#include "Engine/Path.hpp"


Dokev::Texture2D WorkBuilding::loggingCampTexture;
Dokev::Texture2D WorkBuilding::quarryTexture;
Dokev::Texture2D WorkBuilding::ironMineTexture;
Dokev::Texture2D WorkBuilding::missingTexture;
bool             WorkBuilding::texturesLoaded = false;

namespace
{
    CS200::RGBA ApplyAlpha(CS200::RGBA color, uint8_t alpha)
    {
        return (color & 0xFFFFFF00u) | alpha;
    }
}

void WorkBuilding::LoadTextures()
{
    if (!texturesLoaded)
    {
        loggingCampTexture = Dokev::LoadTexture(assets::locate_asset("Assets/resources/LoggingCamp.png").string());
        quarryTexture      = Dokev::LoadTexture(assets::locate_asset("Assets/resources/Quarry.png").string());
        ironMineTexture    = Dokev::LoadTexture(assets::locate_asset("Assets/resources/IronMine.png").string());
        missingTexture     = Dokev::LoadTexture(assets::locate_asset("Assets/images/textures/MissingTexture.png").string());
        texturesLoaded     = true;
    }
}

WorkBuilding::WorkBuilding(Math::fvec2 pos, int w, int h, BuildingType bt) : Building(pos, w, h), productionRate(1.0f), resourceProduce(10), productionTimer(1.0f), resourceManager(nullptr)
{
    LoadTextures();

    max_capacity     = 20;
    current_capacity = 0;
    cost             = 150;

    buildingType = bt;

    switch (bt)
    {
        case BuildingType::WOOD_WORKSHOP:
            texture      = loggingCampTexture;
            max_capacity = 10;
            cost         = 0;
            break;

        case BuildingType::ROCK_WORKSHOP:
            texture      = quarryTexture;
            max_capacity = 10;
            cost         = 0;
            break;

        case BuildingType::STEEL_WORKSHOP:
            texture      = ironMineTexture;
            max_capacity = 10;
            cost         = 0;
            break;

        default:
            texture      = missingTexture;
            max_capacity = 0;
            cost         = 0;
            break;
    }
}

WorkBuilding::~WorkBuilding()
{
    // TODO
}

void WorkBuilding::Update(float dt)
{
    UpdateConstruction(dt);

    if (!isBuilt)
    {
        return;
    }

    if (HasWorker() && resourceManager != nullptr && productionTimer.Update(dt))
    {
        int amountToProduce = static_cast<int>(current_capacity * productionRate);

        switch (buildingType)
        {
            case BuildingType::WOOD_WORKSHOP: resourceManager->AddResource(ResourceType::Wood, amountToProduce); break;

            case BuildingType::ROCK_WORKSHOP: resourceManager->AddResource(ResourceType::Stone, amountToProduce); break;

            case BuildingType::STEEL_WORKSHOP: resourceManager->AddResource(ResourceType::Steel, amountToProduce); break;

            default: break;
        }
    }
}

void WorkBuilding::Draw(int viewIndex, const GridMap* map, [[maybe_unused]] int x, [[maybe_unused]] int y)
{
    if (!map)
    {
        return;
    }

    Math::ivec2 drawPos = CalculateScreenPosition(viewIndex, map);

    CS200::RGBA tint = CS200::WHITE;

    if (!isBuilt)
    {
        const auto alpha = static_cast<uint8_t>(128 + static_cast<int>(progress * 127.0f));
        tint             = ApplyAlpha(tint, alpha);
    }

    texture.Draw(x, y, tint);

    if (!isBuilt)
    {
        DrawProgressBar(viewIndex, map);
    }
}

void WorkBuilding::AddWorker(int count)
{
    current_capacity += count;
    if (current_capacity > max_capacity)
    {
        current_capacity = max_capacity;
    }
}

void WorkBuilding::RemoveWorker(int count)
{
    if (count > static_cast<int>(current_capacity))
    {
        current_capacity = 0;
    }
    else
    {
        current_capacity -= count;
    }
}

int WorkBuilding::GetProduction() const
{
    return static_cast<int>(current_capacity * resourceProduce * productionRate);
}

float WorkBuilding::GetProductionRate() const
{
    return productionRate;
}

bool WorkBuilding::IsFull() const
{
    return current_capacity >= max_capacity;
}

bool WorkBuilding::HasWorker() const
{
    return current_capacity > 0;
}

void WorkBuilding::SetResourceManager(ResourceManager* rm)
{
    resourceManager = rm;
}
