/**
 * \file Resident.cpp
 * \brief Dervied class of Building, manage human tiredness
 * \author Dongyun Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "ResidentBuilding.hpp"

ResidentBuilding::ResidentBuilding(Math::fvec2 pos, int w, int h, BuildingType bt) : Building(pos, w, h), satisfaction(1.0f), taxrate(10)
{
    max_capacity     = 50;
    current_capacity = 0;
    cost             = 100;
    buildingType     = bt;

    switch (bt)
    {
        case BuildingType::SmallHouse:
            texture      = Dokev::LoadTexture("Assets/images/textures/home.png");
            max_capacity = 10;
            cost         = 0;
            break;

        default:
            texture      = Dokev::LoadTexture("Assets/images/textures/MissingTexture.png");
            max_capacity = 0;
            cost         = 0;
            break;
    }
}

ResidentBuilding::~ResidentBuilding()
{
    // TODO
}

void ResidentBuilding::Update([[maybe_unused]] float dt)
{
    // TODO
}

void ResidentBuilding::Draw(int viewIndex, const GridMap* map)
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
    
    int drawX = screenX + TILE_WIDTH_HALF;
    int drawY = topY;

    texture.Draw(drawX, drawY);
}

void ResidentBuilding::AddResident(int count)
{
    current_capacity += count;
    if (current_capacity > max_capacity)
    {
        current_capacity = max_capacity;
    }
}

void ResidentBuilding::RemoveResident(int count)
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

int ResidentBuilding::GetTaxIncome() const
{
    return static_cast<int>(current_capacity * taxrate / 100);
}

float ResidentBuilding::GetSatisfaction() const
{
    return satisfaction;
}

bool ResidentBuilding::IsFull() const
{
    return current_capacity >= max_capacity;
}