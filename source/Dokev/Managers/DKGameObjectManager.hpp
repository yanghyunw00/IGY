/**
 * \file DKGameObjectManager.hpp
 * \brief
 * \author Hyunseok Choi
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#pragma once
#include "../GameObject/DKComponent.hpp"
#include "../GameObject/DKGameObject.hpp"
#include <algorithm>
#include <memory>
#include <vector>

class Human;
class Building;
class Resources;

namespace Dokev
{
    class GameObjectManager : public IComponent
    {
    public:
        void AddHuman(Human* human);
        void AddBuilding(Building* building);
        void RemoveBuilding(int gridX, int gridY);
        void CheckHp(Building* building);

        std::vector<std::unique_ptr<Human>>&    GetHumans();
        std::vector<std::unique_ptr<Building>>& GetBuildings();

        void Unload();
        void Refresh();

    private:
        std::vector<std::unique_ptr<Human>>    m_humans;
        std::vector<std::unique_ptr<Building>> m_buildings;
    };
}
