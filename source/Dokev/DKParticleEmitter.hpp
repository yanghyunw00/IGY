/**
 * \file DKParticleEmitter.hpp
 * \brief Simple particle emitter system using AnimatedSprite
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once

#include "DKAnimationSprite.hpp"
#include "Engine/Vec2.hpp"
#include "Engine/Matrix.hpp"
#include <vector>
#include <memory>
#include <filesystem>

namespace Dokev
{
    /**
     * \brief Single animated particle instance
     */
    class AnimatedParticle
    {
    public:
        AnimatedParticle(const std::filesystem::path& spt_file);

        void Start(Math::vec2 position_, Math::vec2 velocity_, double max_life);
        void Update(double dt);
        void Draw(Math::TransformationMatrix camera_matrix) const;

        bool IsAlive() const { return life > 0; }

    private:
        AnimatedSprite sprite;
        Math::vec2 position{ 0, 0 };
        Math::vec2 velocity{ 0, 0 };
        double life{ 0.0 };
    };


    class ParticleEmitter
    {
    public:

        ParticleEmitter(const std::filesystem::path& spt_file, int max_particles);

        void Emit(int count,
                  Math::vec2 position,
                  Math::vec2 emitter_velocity,
                  Math::vec2 direction,
                  double spread = 0.0,
                  double speed_variation = 0.5);


        void Update(double dt);

        void Draw(Math::TransformationMatrix camera_matrix) const;

        int GetActiveCount() const;

        void SetMaxLife(double new_max_life) { this->max_life = new_max_life; }
        
        double GetMaxLife() const { return max_life; }

    private:
        std::vector<std::unique_ptr<AnimatedParticle>> particles;
        std::filesystem::path sprite_file;
        int next_index{ 0 };
        double max_life{ 1.0 };
    };
}
