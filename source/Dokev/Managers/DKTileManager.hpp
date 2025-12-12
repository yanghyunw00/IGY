/**
 * \file DKTileManager.hpp
 * \brief
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#pragma once
#include "Engine/Vec2.hpp"
#include <memory>
#include <vector>


class GridMap;

namespace Dokev
{
    enum class TileType
    {
        WILDERNESS,
        ROAD,
        RESIDENTIAL,
        INDUSTRIAL
    };

    struct TileOccupancy
    {
        TileType type       = TileType::WILDERNESS;
        bool     isOccupied = false;
        int      buildingId = -1;
    };

    class TileManager
    {
    public:
        TileManager() = default;

        void Initialize(int gridSizeX, int gridSizeY);

        bool AllocateTile(int x, int y, TileType type, int buildingId = -1);
        bool DeallocateTile(int x, int y);

        bool AllocateArea(int startX, int startY, int width, int height, TileType type, int buildingId = -1);
        bool DeallocateArea(int startX, int startY, int width, int height);

        TileType GetTileType(int x, int y) const;
        bool     IsTileOccupied(int x, int y) const;
        bool     CanPlaceTile(int x, int y) const;
        bool     CanPlaceArea(int startX, int startY, int width, int height) const;

        int GetTileCount(TileType type) const;
        int GetTotalOccupiedTiles() const;
        int GetBuildingId(int x, int y) const;

        void Clear();

        std::vector<Math::fvec2> GetTilesOfType(TileType type) const;

    private:
        std::vector<std::vector<TileOccupancy>> tiles;
        int                                     gridSizeX = 0;
        int                                     gridSizeY = 0;

        bool IsValidCoordinate(int x, int y) const;
    };
}
