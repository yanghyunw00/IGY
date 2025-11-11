/**
 * \file DKGameObjectManager.cpp
 * \brief
 * \author Hyunseok Choi
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "DKGameObjectManager.hpp"
#include "Game/Human.hpp"

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

    void GameObjectManager::Refresh()
    {
        m_humans.erase(
            std::remove_if(m_humans.begin(), m_humans.end(),
                [](const auto& obj) { return obj->IsDestroy(); }),
            m_humans.end()
        );
    }

    void GameObjectManager::Unload()
    {
        m_humans.clear();
    }
}