/**
 * \file DKComponentManager.hpp
 * \brief
 * \author Hyunseok Choi
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include "../GameObject/DKComponent.hpp"
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>


namespace Dokev
{
    class ComponentManager
    {
    private:
        std::unordered_map<std::type_index, std::shared_ptr<IComponent>> m_components;

    public:
        template <typename T, typename... TArgs>
        std::shared_ptr<T> AddComponent(TArgs&&... args)
        {
            auto newComponent                        = std::make_shared<T>(std::forward<TArgs>(args)...);
            m_components[std::type_index(typeid(T))] = newComponent;
            return newComponent;
        }

        template <typename T>
        std::shared_ptr<T> GetComponent()
        {
            auto it = m_components.find(std::type_index(typeid(T)));
            if (it != m_components.end())
            {
                return std::dynamic_pointer_cast<T>(it->second);
            }
            return nullptr;
        }

        template <typename T>
        void RemoveComponent()
        {
            m_components.erase(std::type_index(typeid(T)));
        }

        template <typename T>
        bool HasComponent()
        {
            return m_components.count(std::type_index(typeid(T)));
        }

        void UpdateAll(double dt);
    };
}