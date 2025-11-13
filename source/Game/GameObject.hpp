#pragma once
#include <vector>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include "Component.hpp" 

class GameObject
{
public:
    template<typename T, typename... TArgs>
    std::shared_ptr<T> AddComponent(TArgs&&... args)
    {
        auto newComponent = std::make_shared<T>(std::forward<TArgs>(args)...);
        m_components[std::type_index(typeid(T))] = newComponent;
        return newComponent;
    }

    template<typename T>
    std::shared_ptr<T> GetComponent()
    {
        auto it = m_components.find(std::type_index(typeid(T)));
        if (it != m_components.end())
        {
            return std::dynamic_pointer_cast<T>(it->second);
        }
        return nullptr;
    }

    template<typename T>
    bool HasComponent()
    {
        return m_components.count(std::type_index(typeid(T)));
    }

    template<typename T>
    void RemoveComponent()
    {
        m_components.erase(std::type_index(typeid(T)));
    }

    bool IsActive() const { return m_isActive; }
    void SetActive(bool active) { m_isActive = active; }
    void Destroy() { m_isActive = false; }

private:
    std::unordered_map<std::type_index, std::shared_ptr<IComponent>> m_components;
    bool m_isActive = true;
};