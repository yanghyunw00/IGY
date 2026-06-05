/**
 * \file DKMapManager.cpp
 * \brief
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "DKMapManager.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Logger.hpp"


MapVariant                MapManager::s_nextVariant  = MapVariant::Balanced;
std::unique_ptr<GridMap>  MapManager::s_prebuiltMap  = nullptr;
std::unique_ptr<Road>     MapManager::s_prebuiltRoad = nullptr;

MapManager::MapManager()
{
    map  = std::make_unique<GridMap>();
    road = std::make_unique<Road>();
}

void MapManager::SetPrebuiltMap(std::unique_ptr<GridMap> prebuilt)
{
    s_prebuiltMap = std::move(prebuilt);
}

void MapManager::SetPrebuiltRoad(std::unique_ptr<Road> prebuilt)
{
    s_prebuiltRoad = std::move(prebuilt);
}

void MapManager::Initialize()
{
    if (s_prebuiltMap)
    {
        map = std::move(s_prebuiltMap);
    }
    else
    {
        map->SetMapVariant(s_nextVariant);
        map->ReGenerateMap();
    }
    map->InitializePathFinding();

    if (s_prebuiltRoad)
    {
        road = std::move(s_prebuiltRoad);
        return; // roads already placed on the prebuilt map
    }

    // Build initial 3x3 cross road at the center of the map
    {
        const int cx = GRID_SIZE_X / 2;
        const int cy = GRID_SIZE_Y / 2;

        GridCell** viewArray = map->GetViewArray(0);
        const int offsets[9][2] = { {0,0}, {-1,0}, {1,0}, {0,-1}, {0,1}, {-2,0}, {2,0}, {0,-2}, {0,2} };

        // Level all valid road tiles to their average height first
        double totalHeight = 0.0;
        int    validCount  = 0;
        for (auto& off : offsets)
        {
            int x = cx + off[0];
            int y = cy + off[1];
            if (x < 0 || x >= GRID_SIZE_X || y < 0 || y >= GRID_SIZE_Y)
                continue;
            GridCell* cell = viewArray ? viewArray[y * GRID_SIZE_X + x] : nullptr;
            if (!cell || cell->getWater())
                continue;
            totalHeight += cell->getHeight();
            ++validCount;
        }
        if (validCount > 0)
        {
            double avgHeight = totalHeight / validCount;
            const TerrainThresholds& thresh = map->GetGenerationProfile().thresholds;
            for (auto& off : offsets)
            {
                int x = cx + off[0];
                int y = cy + off[1];
                if (x < 0 || x >= GRID_SIZE_X || y < 0 || y >= GRID_SIZE_Y)
                    continue;
                GridCell* cell = viewArray ? viewArray[y * GRID_SIZE_X + x] : nullptr;
                if (!cell || cell->getWater())
                    continue;
                cell->ApplyTerrain(avgHeight, thresh);
            }
        }

        for (auto& off : offsets)
        {
            int x = cx + off[0];
            int y = cy + off[1];
            if (x < 0 || x >= GRID_SIZE_X || y < 0 || y >= GRID_SIZE_Y)
                continue;
            if (viewArray)
            {
                GridCell* cell = viewArray[y * GRID_SIZE_X + x];
                if (cell && cell->getWater())
                    continue;
            }
            road->addRoad(x, y, *map);
        }
        road->refreshAllConnections(*map);
    }

}

void MapManager::SetNextMapVariant(MapVariant variant)
{
    s_nextVariant = variant;
}

MapVariant MapManager::GetNextMapVariant()
{
    return s_nextVariant;
}

