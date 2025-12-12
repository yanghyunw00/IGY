/**
 * \file DKGrid.hpp
 * \brief Isometric grid utility functions for coordinate conversion and grid management
 * \author Dongyun Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once

#include "Engine/Vec2.hpp"
#include <vector>

namespace Dokev
{
    namespace Grid
    {
        Math::fvec2 GridToScreenIsometric(Math::fvec2 gridPos, float tileWidth, float tileHeight);
        Math::fvec2 ScreenToGridIsometric(Math::fvec2 screenPos, float tileWidth, float tileHeight);
        Math::fvec2 ScreenToGridIsometricWithHeight(
            Math::fvec2 screenPos, int originX, int originY, int tileWidthHalf, int tileHeightHalf, const std::vector<double>& heightMap, int gridWidth, int gridHeight);
        Math::fvec2 WorldToIsometric(float gridX, float gridY, float height, int originX, int originY, int tileWidthHalf, int tileHeightHalf);

        bool IsValidGridPosition(Math::fvec2 gridPos, int gridWidth, int gridHeight);
        bool IsInsideBounds(int x, int y, int gridWidth, int gridHeight);

        std::vector<Math::fvec2> GetNeighbors4Way(Math::fvec2 gridPos);
        std::vector<Math::fvec2> GetValidNeighbors4Way(Math::fvec2 gridPos, int gridWidth, int gridHeight);

        int         GridPosToIndex(Math::fvec2 gridPos, int gridWidth);
        int         GridPosToIndex(int x, int y, int gridWidth);
        Math::fvec2 IndexToGridPos(int index, int gridWidth);

        bool IsNeighbor(Math::fvec2 grid1, Math::fvec2 grid2, bool diagonal = false);
    }
}
