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
#include "../Managers/WorkManager.hpp"
#include "CS200/RGBA.hpp"
#include "Engine/Path.hpp"
#include <algorithm>


Dokev::Texture2D WorkBuilding::loggingCampTexture;
Dokev::Texture2D WorkBuilding::quarryTexture;
Dokev::Texture2D WorkBuilding::ironMineTexture;
Dokev::Texture2D WorkBuilding::granaryTexture;

Dokev::Texture2D WorkBuilding::loggingCampInterationTexture;
Dokev::Texture2D WorkBuilding::quarryInterationTexture;
Dokev::Texture2D WorkBuilding::ironMineInterationTexture;
Dokev::Texture2D WorkBuilding::granaryInterationTexture;

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
        granaryTexture     = Dokev::LoadTexture(assets::locate_asset("Assets/resources/Granary_2x2.png").string());

        loggingCampInterationTexture = Dokev::LoadTexture(assets::locate_asset("Assets/resources/LoggingCamp_interaction.png").string());
        quarryInterationTexture = Dokev::LoadTexture(assets::locate_asset("Assets/resources/Quarry_interaction.png").string());
        ironMineInterationTexture = Dokev::LoadTexture(assets::locate_asset("Assets/resources/IronMine_interaction.png").string());
        granaryInterationTexture = Dokev::LoadTexture(assets::locate_asset("Assets/resources/Granary_2x2_interaction.png").string());


        missingTexture = Dokev::LoadTexture(assets::locate_asset("Assets/images/textures/MissingTexture.png").string());
        texturesLoaded = true;
    }
}

WorkBuilding::WorkBuilding(Math::fvec2 pos, int w, int h, int hp, BuildingType bt) : Building(pos, w, h,hp), productionRate(1.0f), resourceProduce(10), productionTimer(1.0f), resourceManager(nullptr)
{
    LoadTextures();

    max_capacity     = 2;
    current_capacity = 0;
    cost             = 150;

    buildingType = bt;

    switch (bt)
    {
        case BuildingType::WOOD_WORKSHOP:
            texture          = loggingCampTexture;
            max_capacity     = 2;
            cost             = 0;
            maintenanceCost  = 10;
            break;

        case BuildingType::ROCK_WORKSHOP:
            texture          = quarryTexture;
            max_capacity     = 2;
            cost             = 0;
            maintenanceCost  = 10;
            break;

        case BuildingType::STEEL_WORKSHOP:
            texture          = ironMineTexture;
            max_capacity     = 2;
            cost             = 0;
            maintenanceCost  = 15;
            break;

        case BuildingType::GRANARY:
            texture      = granaryTexture;
            max_capacity = 2;
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
    WorkManager::GetInstance().RemoveAllJobsForBuilding(this);
}

void WorkBuilding::OnConstructed()
{
    // add job when constructed
    if (max_capacity > 0)
    {
        WorkManager::GetInstance().AddJob(WorkType::Gathering, gridPosition, this);
    }
}

void WorkBuilding::OnWorkerArrived(Human* human)
{
    (void)human;
    AddWorker(1);

    // Add next worker if not full
    if (!IsFull())
    {
        WorkManager::GetInstance().AddJob(WorkType::Gathering, gridPosition, this);
    }
}

void WorkBuilding::OnWorkerLeft(Human* human)
{
    (void)human;
    RemoveWorker(1);
}

void WorkBuilding::Update(float dt)
{
    UpdateConstruction(dt);

    if (!isBuilt)
    {
        return;
    }

    if (resourceManager != nullptr && HasWorker() && productionTimer.Update(dt))
    {
        int amountToProduce = static_cast<int>(current_capacity * productionRate);

        switch (buildingType)
        {
            case BuildingType::WOOD_WORKSHOP: resourceManager->AddResource(ResourceType::Wood, amountToProduce); break;
            case BuildingType::ROCK_WORKSHOP: resourceManager->AddResource(ResourceType::Stone, amountToProduce); break;
            case BuildingType::STEEL_WORKSHOP: resourceManager->AddResource(ResourceType::Steel, amountToProduce); break;
            case BuildingType::GRANARY: break;
            default: break;
        }
    }

    //rely on the hp
    productionRate -= static_cast<float>(hp) * 0.1f;


    if (HasWorker())
    {
        switch (buildingType)
        {
        case BuildingType::WOOD_WORKSHOP:
            texture = loggingCampInterationTexture;
            break;
        case BuildingType::ROCK_WORKSHOP:
            texture = quarryInterationTexture;
            break;
        case BuildingType::STEEL_WORKSHOP:
            texture = ironMineInterationTexture;
            break;
        case BuildingType::GRANARY:
            texture = granaryInterationTexture;
            break;
        default:
            break;
        }
    }
    else
    {
        switch (buildingType)
        {
        case BuildingType::WOOD_WORKSHOP:
            texture = loggingCampTexture;
            break;
        case BuildingType::ROCK_WORKSHOP:
            texture = quarryTexture;
            break;
        case BuildingType::STEEL_WORKSHOP:
            texture = ironMineTexture;
            break;
        case BuildingType::GRANARY:
            texture = granaryTexture;
            break;
        default:
            break;
        }
    }
}

void WorkBuilding::Draw(int viewIndex, const GridMap* map, int x, int y)
{
    if (!map)
    {
        return;
    }

    // Math::ivec2 drawPos = CalculateScreenPosition(viewIndex, map);

    CS200::RGBA tint = CS200::WHITE;

    if (!isBuilt)
    {
        const auto alpha = static_cast<uint8_t>(128 + static_cast<int>(progress * 127.0f));
        tint             = ApplyAlpha(tint, alpha);
    }

    int drawX = x;
    if (buildingType == BuildingType::GRANARY)
    {
        drawX -= TILE_WIDTH_HALF;
    }

    texture.Draw(drawX, y, tint);

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

void WorkBuilding::SetProductionRate(float rate)
{
    productionRate = rate;
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
