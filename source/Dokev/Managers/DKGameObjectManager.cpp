/**
 * \file DKGameObjectManager.cpp
 * \brief
 * \author Hyunseok Choi
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "DKGameObjectManager.hpp"
#include "Game/Entities/Building.hpp"
#include "Game/Entities/Human.hpp"
#include <cmath>

namespace Dokev
{
    GameObjectManager::GameObjectManager()  = default;
    GameObjectManager::~GameObjectManager() = default;

    void GameObjectManager::AddHuman(Human* human)
    {
        m_humans.push_back(std::unique_ptr<Human>(human));
    }

    std::vector<std::unique_ptr<Human>>& GameObjectManager::GetHumans()
    {
        return m_humans;
    }

    void GameObjectManager::AddBuilding(Building* building)
    {
        m_buildings.push_back(std::unique_ptr<Building>(building));
    }

    std::vector<std::unique_ptr<Building>>& GameObjectManager::GetBuildings()
    {
        return m_buildings;
    }

    void GameObjectManager::RemoveBuilding(int gridX, int gridY)
    {
        for (auto it = m_buildings.begin(); it != m_buildings.end();)
        {
            auto buildingGrid = (*it)->GetGrid();
            int  buildingX    = static_cast<int>(std::round(buildingGrid.x));
            int  buildingY    = static_cast<int>(std::round(buildingGrid.y));

            if (buildingX == gridX && buildingY == gridY)
            {
                // Sever any raw pointers humans hold to this building before the
                // unique_ptr frees it; otherwise (e.g. earthquake) they dangle.
                Building* dying = it->get();
                for (auto& human : m_humans)
                    human->OnBuildingDestroyed(dying);

                it = m_buildings.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    void GameObjectManager::CheckHp(Building* building)
    {
        if (building->GetHp() <= 0)
        {
            int x = static_cast<int>(building->GetGrid().x);
            int y = static_cast<int>(building->GetGrid().y);

            RemoveBuilding(x,y);
        }
    }

    void GameObjectManager::Refresh()
    {
        m_humans.erase(std::remove_if(m_humans.begin(), m_humans.end(), [](const auto& obj) { return obj->IsDestroy(); }), m_humans.end());

        m_buildings.erase(std::remove_if(m_buildings.begin(), m_buildings.end(), [](const auto& obj) { return obj->IsDestroy(); }), m_buildings.end());
    }

    void GameObjectManager::Unload()
    {
        m_humans.clear();
        m_buildings.clear();
    }
}