/**
 * \file DKMapManager.cpp
 * \brief 
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "DKMapManager.hpp"
#include "Dokev/DKUtility.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Logger.hpp"
#include "Game/Wood.hpp"
#include "Game/Stone.hpp"
#include "Game/Steel.hpp"

MapManager::MapManager()
{
    road = std::make_unique<Road>();
}

void MapManager::Initialize()
{
    map.ResetPerlinTexture();
    map.ResetIsoGridMapArray();
    map.InitializePathFinding();
   //spawnResources();
}

void MapManager::SpawnResources()
{
    m_resources.clear();
    GridCell** viewArray = map.GetViewArray(0);
    if (!viewArray) return;

    const float spawnChance = 0.2f;  //20% chance atm

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
                    int amount = Dokev::GetRandomValue(100, 500);

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