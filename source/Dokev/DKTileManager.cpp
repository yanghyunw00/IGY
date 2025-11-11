/**
 * \file DKTileManager.cpp
 * \brief 
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "DKTileManager.hpp"
#include <algorithm>

namespace Dokev
{
    void TileManager::Initialize(int gridSizeX_, int gridSizeY_)
    {
        this->gridSizeX = gridSizeX_;
        this->gridSizeY = gridSizeY_;

        tiles.assign(gridSizeX_, std::vector<TileOccupancy>(gridSizeY_));
    }

    bool TileManager::AllocateTile(int x, int y, TileType type, int buildingId)
    {
        if (!IsValidCoordinate(x, y))
            return false;

        if (tiles[y][x].isOccupied)
            return false;

        tiles[y][x].type = type;
        tiles[y][x].isOccupied = true;
        tiles[y][x].buildingId = buildingId;

        return true;
    }

    bool TileManager::DeallocateTile(int x, int y)
    {
        if (!IsValidCoordinate(x, y))
            return false;

        tiles[y][x].type = TileType::WILDERNESS;
        tiles[y][x].isOccupied = false;
        tiles[y][x].buildingId = -1;

        return true;
    }

    bool TileManager::AllocateArea(int startX, int startY, int width, int height, TileType type, int buildingId)
    {
        if (!CanPlaceArea(startX, startY, width, height))
            return false;

        for (int y = startY; y < startY + height; ++y)
        {
            for (int x = startX; x < startX + width; ++x)
            {
                tiles[y][x].type = type;
                tiles[y][x].isOccupied = true;
                tiles[y][x].buildingId = buildingId;
            }
        }

        return true;
    }

    bool TileManager::DeallocateArea(int startX, int startY, int width, int height)
    {
        for (int y = startY; y < startY + height; ++y)
        {
            for (int x = startX; x < startX + width; ++x)
            {
                if (IsValidCoordinate(x, y))
                {
                    tiles[y][x].type = TileType::WILDERNESS;
                    tiles[y][x].isOccupied = false;
                    tiles[y][x].buildingId = -1;
                }
            }
        }

        return true;
    }

    TileType TileManager::GetTileType(int x, int y) const
    {
        if (!IsValidCoordinate(x, y))
            return TileType::WILDERNESS;

        return tiles[y][x].type;
    }

    bool TileManager::IsTileOccupied(int x, int y) const
    {
        if (!IsValidCoordinate(x, y))
            return false;

        return tiles[y][x].isOccupied;
    }

    bool TileManager::CanPlaceTile(int x, int y) const
    {
        if (!IsValidCoordinate(x, y))
            return false;

        return !tiles[y][x].isOccupied;
    }

    bool TileManager::CanPlaceArea(int startX, int startY, int width, int height) const
    {
        for (int y = startY; y < startY + height; ++y)
        {
            for (int x = startX; x < startX + width; ++x)
            {
                if (!IsValidCoordinate(x, y) || tiles[y][x].isOccupied)
                    return false;
            }
        }

        return true;
    }

    int TileManager::GetTileCount(TileType type) const
    {
        int count = 0;
        for (const auto& row : tiles)
        {
            for (const auto& tile : row)
            {
                if (tile.type == type && tile.isOccupied)
                    count++;
            }
        }
        return count;
    }

    int TileManager::GetTotalOccupiedTiles() const
    {
        int count = 0;
        for (const auto& row : tiles)
        {
            for (const auto& tile : row)
            {
                if (tile.isOccupied)
                    count++;
            }
        }
        return count;
    }

    int TileManager::GetBuildingId(int x, int y) const
    {
        if (!IsValidCoordinate(x, y))
            return -1;

        return tiles[y][x].buildingId;
    }

    void TileManager::Clear()
    {
        for (auto& row : tiles)
        {
            for (auto& tile : row)
            {
                tile.type = TileType::WILDERNESS;
                tile.isOccupied = false;
                tile.buildingId = -1;
            }
        }
    }

    std::vector<Math::fvec2> TileManager::GetTilesOfType(TileType type) const
    {
        std::vector<Math::fvec2> result;
        for (int y = 0; y < gridSizeY; ++y)
        {
            for (int x = 0; x < gridSizeX; ++x)
            {
                if (tiles[y][x].type == type && tiles[y][x].isOccupied)
                {
                    result.push_back({ static_cast<float>(x), static_cast<float>(y) });
                }
            }
        }
        return result;
    }

    bool TileManager::IsValidCoordinate(int x, int y) const
    {
        return x >= 0 && x < gridSizeX && y >= 0 && y < gridSizeY;
    }
}
