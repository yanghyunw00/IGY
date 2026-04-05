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

// bit0=N(y-1)  bit1=E(x+1)  bit2=S(y+1)  bit3=W(x-1)
int Road::getConnectionMask(int x, int y) const
{
    int mask = 0;
    if (hasRoad(x,     y - 1)) mask |= 1;  // N
    if (hasRoad(x + 1, y    )) mask |= 2;  // E
    if (hasRoad(x,     y + 1)) mask |= 4;  // S
    if (hasRoad(x - 1, y    )) mask |= 8;  // W
    return mask;
}

void Road::updateRoadTileAt(int x, int y, GridMap& map)
{
    if (!hasRoad(x, y))
        return;

    int mask = getConnectionMask(x, y);
    map.SetRoadConnectionMask(x, y, mask);
}

void Road::addRoad(int x, int y, GridMap& map)
{
    auto* tileManager = map.GetTileManager();

    roadPositions.push_back({static_cast<float>(x), static_cast<float>(y)});
    if (tileManager)
        tileManager->AllocateTile(x, y, Dokev::TileType::ROAD);

    map.SetPathCost(x, y, 0.1f);

    updateRoadTileAt(x,     y,     map);
    updateRoadTileAt(x,     y - 1, map);  // N
    updateRoadTileAt(x + 1, y,     map);  // E
    updateRoadTileAt(x,     y + 1, map);  // S
    updateRoadTileAt(x - 1, y,     map);  // W
}

void Road::removeRoad(int x, int y, GridMap& map)
{
    auto* tileManager = map.GetTileManager();

    if (tileManager)
        tileManager->DeallocateTile(x, y);

    GridCell** viewArray = map.GetViewArray(0);
    
    if (viewArray && x >= 0 && x < GRID_SIZE_X && y >= 0 && y < GRID_SIZE_Y)
    {
        GridCell* cell = viewArray[y * GRID_SIZE_X + x];
        if (cell)
            map.SetPathCost(x, y, cell->getMovementCost());
    }

    auto it = std::find_if(roadPositions.begin(), roadPositions.end(),
        [x, y](const Math::fvec2& pos) {
            return static_cast<int>(pos.x) == x && static_cast<int>(pos.y) == y;
        });

    if (it != roadPositions.end())
        roadPositions.erase(it);

    map.ClearRoadConnectionMask(x, y);

    updateRoadTileAt(x,     y - 1, map);  // N
    updateRoadTileAt(x + 1, y,     map);  // E
    updateRoadTileAt(x,     y + 1, map);  // S
    updateRoadTileAt(x - 1, y,     map);  // W
}

bool Road::IsAdjacentToRoad(int x, int y) const
{
    const int dirs[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    for (auto& d : dirs)
    {
        if (hasRoad(x + d[0], y + d[1]))
            return true;
    }
    return false;
}

bool Road::hasRoad(int x, int y) const
{
    for (const auto& pos : roadPositions)
    {
        if (static_cast<int>(pos.x) == x && static_cast<int>(pos.y) == y)
            return true;
    }
    return false;
}

void Road::drawRoads(const GridMap& /*map*/, int /*currentview*/)
{
}

void Road::buildRoadPath(GridMap& map, int startX, int startY, int endX, int endY)
{
    map.FindPath(startX, startY, endX, endY);

    std::vector<Math::fvec2> path = map.GetCurrentPath();

    for (const auto& pos : path)
    {
        int x = static_cast<int>(pos.x);
        int y = static_cast<int>(pos.y);
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
            map.ClearRoadConnectionMask(x, y);
        }
    }

    roadPositions.clear();
}
