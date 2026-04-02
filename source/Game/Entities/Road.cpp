/**
 * \file Road.cpp
 * \author Dongyun Lee
 * \author
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "Road.hpp"
#include "../World/GridMap.hpp"
#include "Dokev/Core/DKColor.hpp"
#include "Dokev/Managers/DKTileManager.hpp"
#include "Engine/Vec2.hpp"
#include <algorithm>
#include <vector>


void Road::addRoad(int x, int y, GridMap& map)
{
    auto* tileManager = map.GetTileManager();

    roadPositions.push_back({ static_cast<float>(x), static_cast<float>(y) });
    if (tileManager)
    {
        tileManager->AllocateTile(x, y, Dokev::TileType::ROAD);
    }

    // Make road tiles very cheap so A* strongly prefers them
    map.SetPathCost(x, y, 0.1f);
}

void Road::removeRoad(int x, int y, GridMap& map)
{
    auto* tileManager = map.GetTileManager();

    if (tileManager)
    {
        tileManager->DeallocateTile(x, y);
    }

    // Restore original terrain cost from the grid cell
    GridCell** viewArray = map.GetViewArray(0);
    if (viewArray && x >= 0 && x < GRID_SIZE_X && y >= 0 && y < GRID_SIZE_Y)
    {
        GridCell* cell = viewArray[y * GRID_SIZE_X + x];
        if (cell)
            map.SetPathCost(x, y, cell->getMovementCost());
    }

    auto it = std::find_if(roadPositions.begin(), roadPositions.end(), [x, y](const Math::fvec2& pos) { return static_cast<int>(pos.x) == x && static_cast<int>(pos.y) == y; });

    if (it != roadPositions.end())
    {
        roadPositions.erase(it);
    }
}

bool Road::IsAdjacentToRoad(int x, int y) const
{
    const int dirs[4][2] = { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } };
    for (auto& d : dirs)
    {
        if (hasRoad(x + d[0], y + d[1]))
            return true;
    }
    return false;
}

bool Road::hasRoad(int x, int y) const
{
    for (auto it = roadPositions.begin(); it != roadPositions.end(); ++it)
    {
        if (it->x == x && it->y == y)
        {
            return true;
        }
    }

    return false;
}

void Road::drawRoads(const GridMap& map, int currentview)
{
    if (roadPositions.empty())
        return;

    GridCell** viewArray = map.GetViewArray(currentview);

    int originX = 0;
    int originY = 0;

    for (int sum = 0; sum < GRID_SIZE_X + GRID_SIZE_Y - 1; ++sum)
    {
        for (int x = 0; x <= sum; ++x)
        {
            int y = sum - x;

            if (x >= 0 && x < GRID_SIZE_X && y >= 0 && y < GRID_SIZE_Y)
            {
                bool hasRoadHere = false;
                for (const auto& roadPos : roadPositions)
                {
                    if (static_cast<int>(roadPos.x) == x && static_cast<int>(roadPos.y) == y)
                    {
                        hasRoadHere = true;
                        break;
                    }
                }

                if (hasRoadHere)
                {
                    float height  = static_cast<float>(viewArray[y * GRID_SIZE_X + x]->getHeight());
                    int   screenX = originX + (x - y) * TILE_WIDTH_HALF;
                    int   screenY = originY + (x + y) * TILE_HEIGHT_HALF;
                    int   topY    = static_cast<int>(height) - screenY;

                    roadTexture.Draw(screenX, topY);
                }
            }
        }
    }
}

void Road::buildRoadPath(GridMap& map, int startX, int startY, int endX, int endY)
{
    map.FindPath(startX, startY, endX, endY);

    std::vector<Math::fvec2> path = map.GetCurrentPath();

    for (const auto& pos : path)
    {
        int x = static_cast<int>(pos.x);
        int y = static_cast<int>(pos.y);

        // addRoad already handles tile allocation
        addRoad(x, y, map);
    }

    map.ClearPath();
}

void Road::removeAllRoad()
{
    roadPositions.clear();
}

void Road::removeAllRoad(GridMap& map)
{
    auto* tileManager = map.GetTileManager();

    if (tileManager)
    {
        for (const auto& pos : roadPositions)
        {
            int x = static_cast<int>(pos.x);
            int y = static_cast<int>(pos.y);
            tileManager->DeallocateTile(x, y);
        }
    }

    roadPositions.clear();
}
