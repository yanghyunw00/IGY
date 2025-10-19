/**
 * \file Particle.hpp
 * \author Rudy Castan
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#pragma once

#include "Vec2.hpp"

namespace Engine
{
    /**
     * \brief Simple particle class for particle systems
     *
     * A basic particle that stores position, velocity, size, lifetime, and color properties.
     * Can be used for various particle effects like wind, dust, smoke, fire, etc.
     */
    struct Particle
    {
        Math::vec2 position{};   // Current position in world space
        Math::vec2 velocity{};   // Velocity (pixels per second)
        double     size{ 1.0 };  // Particle size
        double     lifeTimer{ 0.0 }; // Current lifetime elapsed
        double     maxLife{ 1.0 };   // Maximum lifetime before particle dies
        double     alpha{ 1.0 };     // Transparency (0.0 = fully transparent, 1.0 = fully opaque)

        /**
         * \brief Update particle state based on delta time
         * \param delta_time Time elapsed since last frame
         */
        void Update(double delta_time);

        /**
         * \brief Check if particle is still alive
         * \return true if particle lifetime hasn't exceeded maxLife, false otherwise
         */
        [[nodiscard]] bool IsAlive() const;

        /**
         * \brief Get normalized life ratio (0.0 at birth, 1.0 at death)
         * \return Life ratio from 0.0 to 1.0
         */
        [[nodiscard]] double GetLifeRatio() const;
    };
}
