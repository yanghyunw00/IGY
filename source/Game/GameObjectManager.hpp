#pragma once
#include "GameObject.hpp"
#include <vector>
#include <memory>
#include <algorithm>

class GameObjectManager
{
public:
    GameObject* CreateGameObject()
    {
        auto obj = std::make_unique<GameObject>();
        auto* objPtr = obj.get();
        m_gameObjects.push_back(std::move(obj));
        return objPtr;
    }

    std::vector<std::unique_ptr<GameObject>>& GetGameObjects()
    {
        return m_gameObjects;
    }

    void Refresh()
    {
        m_gameObjects.erase(
            std::remove_if(m_gameObjects.begin(), m_gameObjects.end(),
                [](const auto& obj) { return !obj->IsActive(); }
            ),
            m_gameObjects.end()
        );
    }

private:
    std::vector<std::unique_ptr<GameObject>> m_gameObjects;
};