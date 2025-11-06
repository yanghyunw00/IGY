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

        // void UpdateAll(double dt);  //여기서 한번에 update
        // void DrawAll(Math::TransformationMatrix camera_matrix); // 여기서 한번에 draw

    private:
        std::vector<std::unique_ptr<GameObject>> m_gameObjects;
    };
}
