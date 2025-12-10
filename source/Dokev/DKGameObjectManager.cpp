/**
 * \file DKGameObjectManager.cpp
 * \brief
 * \author Hyunseok Choi
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "DKGameObjectManager.hpp"
#include "Game/Human.hpp"
#include "Game/Building.hpp"

namespace Dokev
{
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
        m_buildings.erase(
            std::remove_if(m_buildings.begin(), m_buildings.end(),
                [gridX, gridY](const auto& building)
                {
                    auto buildingGrid = building->GetGrid();
                    int buildingX = static_cast<int>(std::round(buildingGrid.x));
                    int buildingY = static_cast<int>(std::round(buildingGrid.y));
                    return buildingX == gridX && buildingY == gridY;
                }),
            m_buildings.end()
        );
    }

    void GameObjectManager::Refresh()
    {
        m_humans.erase(
            std::remove_if(m_humans.begin(), m_humans.end(),
                [](const auto& obj) { return obj->IsDestroy(); }),
            m_humans.end()
        );

      m_buildings.erase(
          std::remove_if(m_buildings.begin(), m_buildings.end(),
              [](const auto& obj) { return obj->IsDestroy(); }),
          m_buildings.end()
      );
    }

    void GameObjectManager::Unload()
    {
        m_humans.clear();
        m_buildings.clear();
    }
}