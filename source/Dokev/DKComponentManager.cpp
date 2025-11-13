/**
 * \file DKComponentManager.cpp
 * \brief 
 * \author Hyunseok Choi
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "DKComponentManager.hpp"

namespace Dokev
{
    void ComponentManager::UpdateAll(double dt)
    {
        for (auto& [type, component] : m_components)
        {
            component->Update(dt);
        }
    }
}
