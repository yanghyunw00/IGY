/**
 * \file DKGrid.cpp
 * \brief Isometric grid utility functions for coordinate conversion and grid management
 * \author Dongyun Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "DKGrid.hpp"
#include <algorithm>
#include <cmath>

namespace Dokev
{
    namespace Grid
    {

        bool IsValidGridPosition(Math::fvec2 gridPos, int gridWidth, int gridHeight)
        {
            return IsInsideBounds(static_cast<int>(gridPos.x), static_cast<int>(gridPos.y), gridWidth, gridHeight);
        }

        bool IsInsideBounds(int x, int y, int gridWidth, int gridHeight)
        {
            return x >= 0 && x < gridWidth && y >= 0 && y < gridHeight;
        }

        std::vector<Math::fvec2> GetNeighbors4Way(Math::fvec2 gridPos)
        {
            std::vector<Math::fvec2> neighbors;
            neighbors.reserve(4);

            neighbors.push_back(Math::fvec2{ gridPos.x + 1, gridPos.y });
            neighbors.push_back(Math::fvec2{ gridPos.x - 1, gridPos.y });
            neighbors.push_back(Math::fvec2{ gridPos.x, gridPos.y + 1 });
            neighbors.push_back(Math::fvec2{ gridPos.x, gridPos.y - 1 });

            return neighbors;
        }

        std::vector<Math::fvec2> GetValidNeighbors4Way(Math::fvec2 gridPos, int gridWidth, int gridHeight)
        {
            std::vector<Math::fvec2> neighbors = GetNeighbors4Way(gridPos);
            std::vector<Math::fvec2> validNeighbors;

            for (const auto& neighbor : neighbors)
            {
                if (IsValidGridPosition(neighbor, gridWidth, gridHeight))
                {
                    validNeighbors.push_back(neighbor);
                }
            }

            return validNeighbors;
        }

        int GridPosToIndex(Math::fvec2 gridPos, int gridWidth)
        {
            return static_cast<int>(gridPos.y) * gridWidth + static_cast<int>(gridPos.x);
        }

        int GridPosToIndex(int x, int y, int gridWidth)
        {
            return y * gridWidth + x;
        }

        Math::fvec2 IndexToGridPos(int index, int gridWidth)
        {
            int x = index % gridWidth;
            int y = index / gridWidth;
            return Math::fvec2{ static_cast<float>(x), static_cast<float>(y) };
        }

        bool IsNeighbor(Math::fvec2 grid1, Math::fvec2 grid2, bool diagonal)
        {
            float dx = std::abs(grid1.x - grid2.x);
            float dy = std::abs(grid1.y - grid2.y);

            if (diagonal)
            {
                return (dx <= 1 && dy <= 1) && !(dx == 0 && dy == 0);
            }
            else
            {
                return (dx == 1 && dy == 0) || (dx == 0 && dy == 1);
            }
        }

        Math::fvec2 ScreenToGridIsometricWithHeight(Math::fvec2 screenPos, int originX, int originY, int tileWidthHalf, int tileHeightHalf, const std::vector<double>& heightMap, int gridWidth, int gridHeight)
        {
            float localX = screenPos.x - static_cast<float>(originX);
            float localY = screenPos.y - static_cast<float>(originY);

            float approxX = (localX / static_cast<float>(tileWidthHalf) + localY / static_cast<float>(tileHeightHalf)) * 0.5f;
            float approxY = (localY / static_cast<float>(tileHeightHalf) - localX / static_cast<float>(tileWidthHalf)) * 0.5f;

            int cx = static_cast<int>(std::floor(approxX));
            int cy = static_cast<int>(std::floor(approxY));

            for (int dy = -1; dy <= 1; ++dy)
            {
                for (int dx = -1; dx <= 1; ++dx)
                {
                    int tx = cx + dx;
                    int ty = cy + dy;

                    if (tx < 0 || tx >= gridWidth || ty < 0 || ty >= gridHeight)
                        continue;

                    double tileHeight  = heightMap[ty * gridWidth + tx];
                    int    tileScreenX = originX + (tx - ty) * tileWidthHalf;
                    int    tileScreenY = originY + (tx + ty) * tileHeightHalf - static_cast<int>(tileHeight);

                    float relX = screenPos.x - static_cast<float>(tileScreenX);
                    float relY = screenPos.y - static_cast<float>(tileScreenY);

                    float nx = std::abs(relX) / static_cast<float>(tileWidthHalf);
                    float ny = std::abs(relY) / static_cast<float>(tileHeightHalf);

                    if (nx + ny <= 1.0f)
                    {
                        return Math::fvec2{ static_cast<float>(tx), static_cast<float>(ty) };
                    }
                }
            }

            if (cx >= 0 && cx < gridWidth && cy >= 0 && cy < gridHeight)
                return Math::fvec2{ static_cast<float>(cx), static_cast<float>(cy) };

            return Math::fvec2{ -1.0f, -1.0f };
        }

        Math::fvec2 WorldToIsometric(float gridX, float gridY, float height, int originX, int originY, int tileWidthHalf, int tileHeightHalf)
        {
            int isoX = originX + static_cast<int>((gridX - gridY) * tileWidthHalf);
            int isoY = originY + static_cast<int>((gridX + gridY) * tileHeightHalf) - static_cast<int>(height);

            float centerX = static_cast<float>(isoX + tileWidthHalf);
            float centerY = static_cast<float>(isoY + tileHeightHalf);

            return Math::fvec2{ centerX, centerY };
        }

        Math::fvec2 GridToScreenIsometric(Math::fvec2 gridPos, float tileWidth, float tileHeight)
        {
            float screenX = (gridPos.x - gridPos.y) * tileWidth * 0.5f;
            float screenY = (gridPos.x + gridPos.y) * tileHeight * 0.5f;
            return Math::fvec2{ screenX, screenY };
        }

        Math::fvec2 ScreenToGridIsometric(Math::fvec2 screenPos, float tileWidth, float tileHeight)
        {
            float gridX = (screenPos.x / (tileWidth * 0.5f) + screenPos.y / (tileHeight * 0.5f)) * 0.5f;
            float gridY = (screenPos.y / (tileHeight * 0.5f) - screenPos.x / (tileWidth * 0.5f)) * 0.5f;
            return Math::fvec2{ gridX, gridY };
        }

    }
}
