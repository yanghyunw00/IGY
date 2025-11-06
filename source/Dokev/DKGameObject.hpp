#pragma once
#include "DKComponent.hpp"
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace Dokev
{
    class GameObject
    {
    public:
        template <typename T, typename... TArgs>
        std::shared_ptr<T> AddComponent(TArgs&&... args);

        template <typename T>
        std::shared_ptr<T> GetComponent();

        template <typename T>
        bool HasComponent();

        //이걸로 한번에 Draw하고 Update하는게 어떨까용 -현우-
        //virtual void Draw() = 0;
        //virtual void Update([[maybe_unused] double dt]) = 0;
        //virtual ~GameObject() {}
        //virtual std::string TypeName() = 0;
        //virtual GameObjectType Type() = 0;

        
        template <typename T>
        void RemoveComponent();

        bool IsActive() const
        {
            return m_isActive;
        }

        void SetActive(bool active)
        {
            m_isActive = active;
        }

        void Destroy()
        {
            m_isActive = false;
        }

    private:
        std::unordered_map<std::type_index, std::shared_ptr<IComponent>> m_components;
        bool                                                             m_isActive = true;
    };
}
