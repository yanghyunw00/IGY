/**
 * \file DKMapManager.cpp
 * \brief
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "DKMapManager.hpp"
#include "Dokev/Core/DKUtility.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Logger.hpp"
#include "Game/Resources/Steel.hpp"
#include "Game/Resources/Stone.hpp"
#include "Game/Resources/Wood.hpp"


MapVariant MapManager::s_nextVariant = MapVariant::Balanced;

MapManager::MapManager()
{
    road = std::make_unique<Road>();
}

void MapManager::Initialize()
{
    map.SetMapVariant(s_nextVariant);
    map.ReGenerateMap();
    map.InitializePathFinding();

    // Build initial 3x3 cross road at the center of the map
    {
        const int cx = GRID_SIZE_X / 2;
        const int cy = GRID_SIZE_Y / 2;

        GridCell** viewArray = map.GetViewArray(0);
        const int offsets[9][2] = { {0,0}, {-1,0}, {1,0}, {0,-1}, {0,1}, {-2,0}, {2,0}, {0,-2}, {0,2} };
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
            road->addRoad(x, y, map);
        }
    }

    // spawnResources();
}

void MapManager::SetNextMapVariant(MapVariant variant)
{
    s_nextVariant = variant;
}

MapVariant MapManager::GetNextMapVariant()
{
    return s_nextVariant;
}

void MapManager::SpawnResources()
{
    m_resources.clear();
    GridCell** viewArray = map.GetViewArray(0);
    if (!viewArray)
        return;

    const float spawnChance = 0.2f; // 20% chance atm

    for (int y = 0; y < GRID_SIZE_Y; ++y)
    {
        for (int x = 0; x < GRID_SIZE_X; ++x)
        {
            GridCell* cell = viewArray[y * GRID_SIZE_X + x];

            if (cell && !cell->getWater() && !map.GetTileManager()->IsTileOccupied(x, y))
            {
                if (Dokev::ChanceRoll(spawnChance))
                {
                    int resourceType = Dokev::GetRandomValue(0, 2);
                    int amount       = Dokev::GetRandomValue(100, 500);

                    if (resourceType == 0)
                    {
                        m_resources.push_back(std::make_unique<Wood>(x, y, amount));
                    }
                    else if (resourceType == 1)
                    {
                        m_resources.push_back(std::make_unique<Stone>(x, y, amount));
                    }
                    else
                    {
                        m_resources.push_back(std::make_unique<Steel>(x, y, amount));
                    }
                }
            }
        }
    }

    std::string logMsg = "Spawned " + std::to_string(m_resources.size()) + " resources.";
    Engine::GetLogger().LogEvent(logMsg);
}
