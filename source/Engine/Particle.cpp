/**
 * \file Particle.cpp
 * \author Rudy Castan
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "Particle.hpp"
#include <algorithm>

namespace Engine
{
    void Particle::Update(double delta_time)
    {
        // Update position based on velocity
        position.x += velocity.x * delta_time;
        position.y += velocity.y * delta_time;

        // Update lifetime
        lifeTimer += delta_time;

        // Update alpha based on life ratio (fade out as particle ages)
        const double life_ratio = GetLifeRatio();
        alpha = std::max(0.0, 1.0 - life_ratio);
    }

    bool Particle::IsAlive() const
    {
        return lifeTimer < maxLife;
    }

    double Particle::GetLifeRatio() const
    {
        if (maxLife <= 0.0)
            return 1.0;
        return std::min(lifeTimer / maxLife, 1.0);
    }
}
