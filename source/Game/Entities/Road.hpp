/**
 * \file Road.hpp
 * \author Dongyun Lee
 * \author
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#pragma once

#include <vector>

#include "../World/GridMap.hpp"
#include "Dokev/Graphics/DKTexture.hpp"
#include "Engine/Path.hpp"
#include "Engine/Vec2.hpp"

using namespace Dokev;

class GridMap;

class Road
{
private:
    std::vector<Math::fvec2> roadPositions;

    // Returns 4-bit neighbor connection mask.
    // bit0=N(y-1)  bit1=E(x+1)  bit2=S(y+1)  bit3=W(x-1)
    int  getConnectionMask(int x, int y, const GridMap& map) const;
    void updateRoadTileAt(int x, int y, GridMap& map);

public:
    Road() {}
    ~Road() {}

    void addRoad(int x, int y, GridMap& map);
    void removeRoad(int x, int y, GridMap& map);
    void removeAllRoad();
    void removeAllRoad(GridMap& map);
    bool hasRoad(int x, int y) const;
    bool IsAdjacentToRoad(int x, int y) const;
    void drawRoads(const GridMap& map, int currentview);
    void buildRoadPath(GridMap& map, int startX, int startY, int endX, int endY);
    void refreshConnectionsAround(int x, int y, GridMap& map);
    void refreshAllConnections(GridMap& map);

    size_t getRoadCount() const
    {
        return roadPositions.size();
    }

    const std::vector<Math::fvec2>& GetRoadPositions() const
    {
        return roadPositions;
    }
};
