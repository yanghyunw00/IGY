/**
 * \file WorkBuilding.cpp
 * \brief Derived class of Building, manages workplace jobs and production
 * \author Dongyun Lee
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "WorkBuilding.hpp"
#include "CS200/RGBA.hpp"
#include "Engine/Path.hpp"
#include "ResourceManager.hpp"ㅈㅇㅈ

WorkBuilding::WorkBuilding(Math::fvec2 pos, int w, int h, BuildingType bt) : Building(pos, w, h), productionRate(1.0f), resourceProduce(10), productionTimer(1.0f), resourceManager(nullptr)
{
    max_capacity     = 20;
    current_capacity = 0;
    cost             = 150;

    buildingType = bt;

    switch(bt)
    {
        case BuildingType::WOOD_WORKSHOP:
        texture = Dokev::LoadTexture(assets::locate_asset("Assets/images/textures/work.png").string());
        max_capacity = 10;
        cost = 0;
        break;

        case BuildingType::ROCK_WORKSHOP:
        texture = Dokev::LoadTexture(assets::locate_asset("Assets/images/textures/work.png").string());
        max_capacity = 10;
        cost = 0;
        break;

        case BuildingType::STEEL_WORKSHOP:
        texture = Dokev::LoadTexture(assets::locate_asset("Assets/images/textures/work.png").string());
        max_capacity = 10;
        cost = 0;
        break;

        default:
            texture = Dokev::LoadTexture(assets::locate_asset("Assets/images/textures/MissingTexture.png").string());
            max_capacity = 0;
            cost = 0;
            break;
    }
}

WorkBuilding::~WorkBuilding()
{
    // TODO
}

void WorkBuilding::Update([[maybe_unused]] float dt)
{
    if (HasWorker() && resourceManager != nullptr && productionTimer.Update(dt))
    {
        int amountToProduce = static_cast<int>(current_capacity * productionRate);

        switch (buildingType)
        {
            case BuildingType::WOOD_WORKSHOP:
                resourceManager->AddResource(ResourceType::Wood, amountToProduce);
                break;

            case BuildingType::ROCK_WORKSHOP:
                resourceManager->AddResource(ResourceType::Stone, amountToProduce);
                break;

            case BuildingType::STEEL_WORKSHOP:
                resourceManager->AddResource(ResourceType::Steel, amountToProduce);
                break;

            default:
                break;
        }
    }
}

void WorkBuilding::Draw(int viewIndex, const GridMap* map)
{
    Math::fvec2 transformedPos = map->TransformPathPointForView(Math::fvec2(gridPosition.x, gridPosition.y), viewIndex);

    float gridX_float = transformedPos.x;
    float gridY_float = transformedPos.y;

    int gridX_int = static_cast<int>(gridX_float);
    int gridY_int = static_cast<int>(gridY_float);

    GridCell** viewArray = map->GetViewArray(viewIndex);

    float height = static_cast<float>(viewArray[gridY_int * GRID_SIZE_X + gridX_int]->getHeight());

    int originX = 0;
    int originY = 0;

    int screenX = originX + static_cast<int>((gridX_float - gridY_float) * TILE_WIDTH_HALF);
    int screenY = originY + static_cast<int>((gridX_float + gridY_float) * TILE_HEIGHT_HALF);
    int topY = static_cast<int>(height) - screenY;
    
    int drawX = screenX + TILE_WIDTH_HALF - texture.GetWidth() / 2;
    int drawY = topY + TILE_HEIGHT * 2;

    CS200::RGBA tint = CS200::WHITE;
    switch (buildingType)
    {
        case BuildingType::WOOD_WORKSHOP:
            tint = CS200::pack_color({0.6f, 0.4f, 0.2f, 1.0f});
            break;
        case BuildingType::ROCK_WORKSHOP:
            tint = CS200::pack_color({0.5f, 0.5f, 0.5f, 1.0f});
            break;
        case BuildingType::STEEL_WORKSHOP:
            tint = CS200::pack_color({0.7f, 0.7f, 0.8f, 1.0f});
            break;
        default:
            tint = CS200::WHITE;
            break;
    }

    texture.Draw(drawX, drawY, tint);
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
