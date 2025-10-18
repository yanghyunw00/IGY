#pragma once

#include "Engine/Vec2.hpp"
#include <vector>

namespace Dokev
{
    namespace Grid
    {
        Math::fvec2 GridToScreenIsometric(Math::fvec2 gridPos, float tileWidth, float tileHeight);
        Math::fvec2 ScreenToGridIsometric(Math::fvec2 screenPos, float tileWidth, float tileHeight);
        Math::fvec2 ScreenToGridIsometricWithHeight(Math::fvec2 screenPos, int originX, int originY, int tileWidthHalf, int tileHeightHalf, const std::vector<double>& heightMap, int gridWidth, int gridHeight);
        Math::fvec2 WorldToIsometric(float gridX, float gridY, float height, int originX, int originY, int tileWidthHalf, int tileHeightHalf);


        bool IsValidGridPosition(Math::fvec2 gridPos, int gridWidth, int gridHeight);
        bool IsWithinBounds(int x, int y, int gridWidth, int gridHeight);

        std::vector<Math::fvec2> GetNeighbors4Way(Math::fvec2 gridPos);
        std::vector<Math::fvec2> GetValidNeighbors4Way(Math::fvec2 gridPos, int gridWidth, int gridHeight);

        int GridPosToIndex(Math::fvec2 gridPos, int gridWidth);
        int GridPosToIndex(int x, int y, int gridWidth);
        Math::fvec2 IndexToGridPos(int index, int gridWidth);

        bool AreNeighbors(Math::fvec2 cell1, Math::fvec2 cell2, bool diagonal = false);
        Math::fvec2 GetDirection(Math::fvec2 from, Math::fvec2 to);
    }
}
