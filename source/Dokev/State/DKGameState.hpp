/**
 * \file DKGameState.hpp
 * \brief
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include "Dokev/Managers/DKComponentManager.hpp"
#include <gsl/gsl>


namespace Dokev
{
    class GameState
    {
    public:
        virtual void          Load()            = 0;
        virtual void          Update(double dt) = 0;
        virtual void          Unload()          = 0;
        virtual gsl::czstring GetName() const   = 0;
        virtual void          Draw() const      = 0;

        virtual void DrawImGui()
        {
        }

        virtual ~GameState() = default;

        template <typename T>
        std::shared_ptr<T> GetGSComponent() const
        {
            return const_cast<ComponentManager&>(componentmanager).GetComponent<T>();
        }

    protected:
        template <typename T, typename... TArgs>
        std::shared_ptr<T> AddGSComponent(TArgs&&... args)
        {
            return componentmanager.AddComponent<T>(std::forward<TArgs>(args)...);
        }

        void UpdateGSComponents(double dt)
        {
            componentmanager.UpdateAll(dt);
        }

        template <typename T>
        void RemoveGSComponent()
        {
            componentmanager.RemoveComponent<T>();
        }

        template <typename T>
        bool HasGSComponent()
        {
            return componentmanager.HasComponent<T>();
        }

    private:
        ComponentManager componentmanager;
    };
}
