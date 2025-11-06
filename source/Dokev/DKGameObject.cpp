#include "Dokev/DKGameObject.hpp"

namespace Dokev
{
    template <typename T, typename... TArgs>
    std::shared_ptr<T> GameObject::AddComponent(TArgs&&... args)
    {
        auto newComponent                        = std::make_shared<T>(std::forward<TArgs>(args)...);
        m_components[std::type_index(typeid(T))] = newComponent;
        return newComponent;
    }

    template <typename T>
    std::shared_ptr<T> GameObject::GetComponent()
    {
        auto it = m_components.find(std::type_index(typeid(T)));
        if (it != m_components.end())
        {
            return std::dynamic_pointer_cast<T>(it->second);
        }
        return nullptr;
    }

    template <typename T>
    bool GameObject::HasComponent()
    {
        return m_components.count(std::type_index(typeid(T)));
    }

    template <typename T>
    void GameObject::RemoveComponent()
    {
        m_components.erase(std::type_index(typeid(T)));
    }
}
