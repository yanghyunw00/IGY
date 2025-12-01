#pragma once
#include "Engine/Vec2.hpp"
#include "Game/GridMap.hpp"
#include "Game/GridCell.hpp"


namespace Dokev
{

    class MouseSelection
    {
    public:

        void Update(float x, float y);
        void Draw() const;

    private:
    Math::vec2 position;
    GridMap* map;

    };
}