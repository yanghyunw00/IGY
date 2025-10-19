#pragma once

#include "Engine/Vec2.hpp"

namespace Dokev
{
    struct Particle
    {
        Math::vec2 position{};   
        Math::vec2 velocity{};   
        double     size{ 1.0 };  
        double     lifeTimer{ 0.0 }; 
        double     maxLife{ 1.0 };   
        double     alpha{ 1.0 };    

        void Update(double delta_time);

        [[nodiscard]] bool IsAlive() const;

        [[nodiscard]] double GetLifeRatio() const;
    };
}
