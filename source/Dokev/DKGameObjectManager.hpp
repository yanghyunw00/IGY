/**
 * \file DKGameObjectManager.hpp
 * \brief
 * \author Hyunseok Choi
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#pragma once
#include "DKComponent.hpp"
#include "DKGameObject.hpp"
#include <algorithm>
#include <memory>
#include <vector>

class Human;

namespace Dokev
{
    class GameObjectManager : public IComponent
    {
    public:
        void AddHuman(Human* human);

        std::vector<std::unique_ptr<Human>>& GetHumans();

        void Unload();
        void Refresh();

    private:
        std::vector<std::unique_ptr<Human>> m_humans;
        // std::vector<std::unique_ptr<Building>> m_buildings;
    };
}
