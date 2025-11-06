/**
 * \file DKGameObjectManager.hpp
 * \brief
 * \author Hyunseok Choi
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#pragma once
#include "DKGameObject.hpp"
#include "Engine/Matrix.hpp"
#include <algorithm>
#include <memory>
#include <vector>


namespace Dokev
{
    class GameObjectManager
    {
    public:
        GameObject*                               CreateGameObject();
        std::vector<std::unique_ptr<GameObject>>& GetGameObjects();
        void                                      Refresh();

        // void UpdateAll(double dt);  // update
        // void DrawAll(Math::TransformationMatrix camera_matrix); //  draw

    private:
        std::vector<std::unique_ptr<GameObject>> m_gameObjects;
    };
}
