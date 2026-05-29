/**
 * \file Building.cpp
 * \brief Base class for all building object
 * \author Dongyun Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "Building.hpp"
#include "../Managers/FloatingTextManager.hpp"
#include "Game/Managers/WorkManager.hpp"

#include "Dokev/Graphics/DKShapes.hpp"
#include "Engine/Path.hpp"
#include <cassert>


Building::Building(Math::fvec2 pos, int w, int h, int hp_, float buildDuration)
    : gridPosition(pos), width(w), height(h), hp(hp_), isBuilt(false), max_capacity(0), current_capacity(0), cost(0), progress(0.0f), buildTime(0.0f),
      maxBuildTime(buildDuration <= 0.0f ? 0.001f : buildDuration)
{
    assert(w > 0 && "Building width must be positive");
    assert(h > 0 && "Building height must be positive");

    texture = Dokev::LoadTexture(assets::locate_asset("Assets/images/textures/MissingTexture.png").string());
}

Building::~Building() = default;

void Building::UpdateConstruction(float dt)
{
    assert(dt >= 0.0f && "Delta time cannot be negative");

    if (isBuilt)
    {
        progress = 1.0f;
        return;
    }

    buildTime += dt;
    progress = buildTime / maxBuildTime;

    assert(progress >= 0.0f && progress <= 1.1f && "Progress out of range");

    if (progress >= 1.0f)
    {
        progress = 1.0f;
        isBuilt  = true;
        FloatingTextManager::Instance().Add("Complete!", gridPosition * TILE_WIDTH, 0x00FF00FF);
        OnConstructed(); // add job
    }
}

Math::ivec2 Building::CalculateScreenPosition(int viewIndex, const GridMap* map) const
{
    if (!map)
    {
        return { 0, 0 };
    }

    Math::fvec2 transformedPos = map->TransformPathPointForView(Math::fvec2(gridPosition.x, gridPosition.y), viewIndex);

    int gridX_int = static_cast<int>(transformedPos.x);
    int gridY_int = static_cast<int>(transformedPos.y);

    if (gridX_int < 0 || gridX_int >= GRID_SIZE_X || gridY_int < 0 || gridY_int >= GRID_SIZE_Y)
    {
        return { 0, 0 };
    }

    GridCell** viewArray = map->GetViewArray(viewIndex);
    if (!viewArray)
    {
        return { 0, 0 };
    }

    GridCell* cell = viewArray[gridY_int * GRID_SIZE_X + gridX_int];
    if (!cell)
    {
        return { 0, 0 };
    }

    float m_height = static_cast<float>(cell->getHeight());

    int originX = map->GetIsoOriginX();
    int originY = map->GetIsoOriginY();

    int screenX = originX + static_cast<int>((gridX_int - gridY_int) * TILE_WIDTH_HALF);
    int screenY = originY + static_cast<int>((gridX_int + gridY_int) * TILE_HEIGHT_HALF);
    int topY    = static_cast<int>(m_height) - screenY;

    int drawX = screenX + TILE_WIDTH_HALF - texture.GetWidth() / 2;
    int drawY = topY + TILE_HEIGHT * 2;

    return { drawX, drawY };
}

void Building::UpdateLightTimer(float dt)
{
    if (current_capacity > 0)
        lightTimer = 30.0f;
    else if (lightTimer > 0.0f)
        lightTimer -= dt;
}

Math::ivec2 Building::GetLightPos(int viewIndex, const GridMap* map) const
{
    auto p = CalculateScreenPosition(viewIndex, map);
    float yFrac = (buildingType == BuildingType::STREET_LIGHT) ? 0.06f : 0.5f;
    return { p.x + texture.GetWidth() / 2, p.y - static_cast<int>(texture.GetHeight() * yFrac) };
}

void Building::DrawProgressBar(int viewIndex, const GridMap* map) const
{
    if (isBuilt || !map)
    {
        return;
    }

    Math::ivec2 drawPos   = CalculateScreenPosition(viewIndex, map);
    const int   barWidth  = texture.GetWidth();
    const int   barHeight = 6;

    const int barX = drawPos.x;
    const int barY = drawPos.y + 6;

    Dokev::DrawRectangle(barX, barY, barWidth, barHeight, 0x444444FF);

    int filledWidth = static_cast<int>(static_cast<float>(barWidth) * progress);
    if (filledWidth > 0)
    {
        Dokev::DrawRectangle(barX, barY, filledWidth, barHeight, 0x00FF00FF);
    }
}

int Building::GetHp() const
{
    return hp;
}

void Building::setHP(int value)
{
    hp = value;
}

int Building::GetCost() const
{
    return cost;
}

int Building::GetMaintenanceCost() const
{
    return maintenanceCost;
}

Math::fvec2 Building::GetGrid() const
{
    return gridPosition;
}

int Building::GetWidth() const
{
    return width;
}

int Building::GetHeight() const
{
    return height;
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

void Building::OnConstructed()
{
}

void Building::ForceComplete()
{
    buildTime = maxBuildTime;
    progress  = 1.0f;
    isBuilt   = true;
    OnConstructed();
}
