#pragma once

#include "../Dokev/DKColor.hpp"
#include "Engine/Vec2.hpp"
#include "Game/Entities/Human.hpp"

struct IComponent
{
    virtual ~IComponent() = default;
};

struct TransformComponent : public IComponent
{
    Math::fvec2 position;
    Math::fvec2 gridPosition;
};

struct PathComponent : public IComponent
{
    std::vector<Math::fvec2> path;
    size_t                   currentPathIndex = 0;
    bool                     isMoving         = false;
    float                    moveTimer        = 0.0f;
    const float              moveDelay        = 0.5f;
    Math::fvec2              startPosition;
    Math::fvec2              targetGridPos;
};

struct AIComponent : public IComponent
{
    HumanState state = HumanState::Idle;
    HumanAI    ai;
};

struct StatsComponent : public IComponent
{
    HumanStats stats;
};

struct RenderComponent : public IComponent
{
    CS200::RGBA color  = Dokev::ColorPalette::BLUE;
    float       radius = 20.0f;
};