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
#include <algorithm>
#include <cassert>
#include <cmath>

namespace
{
    struct FootprintViewTile
    {
        int originalX = 0;
        int originalY = 0;
        int viewX     = 0;
        int viewY     = 0;
        int order     = 0;
        bool valid    = false;
    };

    FootprintViewTile SelectFootprintTileForPosition(const Building& building, const GridMap& map, int viewIndex)
    {
        FootprintViewTile result;

        const int baseX = static_cast<int>(std::round(building.GetGrid().x));
        const int baseY = static_cast<int>(std::round(building.GetGrid().y));

        for (int y = 0; y < building.GetHeight(); ++y)
        {
            for (int x = 0; x < building.GetWidth(); ++x)
            {
                const int originalX = baseX + x;
                const int originalY = baseY + y;

                Math::fvec2 transformed = map.TransformPathPointForView({ static_cast<float>(originalX), static_cast<float>(originalY) }, viewIndex);
                const int   viewX       = std::clamp(static_cast<int>(std::round(transformed.x)), 0, GRID_SIZE_X - 1);
                const int   viewY       = std::clamp(static_cast<int>(std::round(transformed.y)), 0, GRID_SIZE_Y - 1);
                const int   order       = viewX + viewY;

                if (!result.valid || order < result.order || (order == result.order && viewX < result.viewX))
                {
                    result = { originalX, originalY, viewX, viewY, order, true };
                }
            }
        }

        return result;
    }
}

Building::Building(Math::fvec2 pos, int w, int h, int hp_, float buildDuration)
    : gridPosition(pos), width(w), height(h), isBuilt(false), hp(hp_), max_capacity(0), current_capacity(0), cost(0), progress(0.0f), buildTime(0.0f),
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

    if (progress >= 1.0f)
    {
        progress = 1.0f;
        isBuilt  = true;
        FloatingTextManager::Instance().Add("Complete!", gridPosition * TILE_WIDTH, 0x00FF00FF);
        OnConstructed(); // add job
    }

    assert(progress >= 0.0f && progress <= 1.0f && "Progress out of range");
}

Math::ivec2 Building::CalculateScreenPosition(int viewIndex, const GridMap* map) const
{
    if (!map)
    {
        return { 0, 0 };
    }

    FootprintViewTile anchor = SelectFootprintTileForPosition(*this, *map, viewIndex);
    if (!anchor.valid)
    {
        return { 0, 0 };
    }

    if (anchor.viewX < 0 || anchor.viewX >= GRID_SIZE_X || anchor.viewY < 0 || anchor.viewY >= GRID_SIZE_Y)
    {
        return { 0, 0 };
    }

    GridCell** viewArray = map->GetViewArray(viewIndex);
    if (!viewArray)
    {
        return { 0, 0 };
    }

    GridCell* cell = viewArray[anchor.viewY * GRID_SIZE_X + anchor.viewX];
    if (!cell)
    {
        return { 0, 0 };
    }

    float m_height = static_cast<float>(cell->getHeight());

    int originX = map->GetIsoOriginX();
    int originY = map->GetIsoOriginY();

    int screenX = originX + static_cast<int>((anchor.viewX - anchor.viewY) * TILE_WIDTH_HALF);
    int screenY = originY + static_cast<int>((anchor.viewX + anchor.viewY) * TILE_HEIGHT_HALF);
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

    if (buildingType == BuildingType::STREET_LIGHT)
    {
        // Street light draw offset (matches ResidentBuilding::Draw)
        int drawX = p.x + TILE_WIDTH_HALF / 2;
        int drawY = p.y - TILE_HEIGHT_HALF;
        // texture.Draw(x,y) uses y as top of texture in y-up coords, so pixel y=34 from top = drawY - 34
        return { drawX + 23, drawY - 34 };
    }

    return { p.x + texture.GetWidth() / 2, p.y - texture.GetHeight() / 2 };
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
