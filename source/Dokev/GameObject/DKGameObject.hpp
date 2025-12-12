/**
 * \file DKGameObject.hpp
 * \brief
 * \author Hyunseok Choi
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#pragma once
#include "DKGameObjectTypes.hpp"
#include <string>

namespace Dokev
{
    class GameObject
    {
    public:
        virtual ~GameObject() = default;

        virtual std::string    TypeName() = 0;
        virtual GameObjectType Type()     = 0;

        virtual bool IsDestroy() const
        {
            return !m_isActive;
        }

        virtual void Destroy()
        {
            m_isActive = false;
        }

        bool IsActive() const
        {
            return m_isActive;
        }

        void SetActive(bool active)
        {
            m_isActive = active;
        }

    protected:
        bool m_isActive = true;
    };
}
