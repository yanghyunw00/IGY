/**
 * \file DKParticle.cpp
 * \brief 
 * \author Hyeonseok Choi
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "DKParticle.hpp"
#include <algorithm>

namespace Dokev
{
    void Particle::Update(double delta_time)
    {
        position.x += velocity.x * delta_time;
        position.y += velocity.y * delta_time;

        lifeTimer += delta_time;

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
