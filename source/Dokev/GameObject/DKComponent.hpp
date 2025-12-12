/**
 * \file DKComponent.hpp
 * \brief
 * \author Hyunseok Choi
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#pragma once

struct IComponent
{
    virtual ~IComponent() = default;
    virtual void Update([[maybe_unused]] double dt) { };
};