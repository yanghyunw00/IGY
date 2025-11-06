/**
 * \file DKGameObjectManager.cpp
 * \brief
 * \author Hyunseok Choi
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "DKGameObjectManager.hpp"

namespace Dokev
{
    GameObject* GameObjectManager::CreateGameObject()
    {
        auto  obj    = std::make_unique<GameObject>();
        auto* objPtr = obj.get();
        m_gameObjects.push_back(std::move(obj));
        return objPtr;
    }

    std::vector<std::unique_ptr<GameObject>>& GameObjectManager::GetGameObjects()
    {
        return m_gameObjects;
    }

    void GameObjectManager::Refresh()
    {
        m_gameObjects.erase(std::remove_if(m_gameObjects.begin(), m_gameObjects.end(), [](const auto& obj) { return !obj->IsActive(); }), m_gameObjects.end());
    }
}