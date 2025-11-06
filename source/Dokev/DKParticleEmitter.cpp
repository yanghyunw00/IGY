/**
 * \file DKParticleEmitter.cpp
 * \brief 
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "DKParticleEmitter.hpp"
#include "DKMath.hpp"
#include <cstdlib>
#include <cmath>

namespace Dokev
{
    AnimatedParticle::AnimatedParticle(const std::filesystem::path& spt_file)
        : sprite(spt_file)
    {
        life = 0.0;
    }

    void AnimatedParticle::Start([[maybe_unused]] Math::vec2 position_,[[maybe_unused]] Math::vec2 velocity_, double max_life)
    {
        this->position = position_;
        this->velocity = velocity_;
        this->life = max_life;

        if (!sprite.GetCurrentAnimationName().empty())
        {
            sprite.PlayAnimation(sprite.GetCurrentAnimationName(), true);
        }
    }

    void AnimatedParticle::Update(double dt)
    {
        if (!IsAlive())
            return;

        life -= dt;

        position.x += velocity.x * dt;
        position.y += velocity.y * dt;

        sprite.Update(dt);
    }

    void AnimatedParticle::Draw(Math::TransformationMatrix camera_matrix) const
    {
        if (!IsAlive())
            return;

        auto transform = Math::TranslationMatrix(position);

        auto final_transform = camera_matrix * transform;

        sprite.Draw(final_transform);
    }


    ParticleEmitter::ParticleEmitter(const std::filesystem::path& spt_file, int max_particles)
        : sprite_file(spt_file)
    {
        particles.reserve(max_particles);
        for (int i = 0; i < max_particles; i++)
        {
            particles.push_back(std::make_unique<AnimatedParticle>(spt_file));
        }
    }

    void ParticleEmitter::Emit(int count,
                               Math::vec2 position,
                               Math::vec2 emitter_velocity,
                               Math::vec2 direction,
                               double spread,
                               double speed_variation)
    {
        for (int i = 0; i < count; i++)
        {
            AnimatedParticle* particle = particles[next_index].get();

            if (particle->IsAlive())
            {
                printf("Warning: Particle overwritten\n");
            }

            double angle_variation = 0.0;
            if (spread != 0.0)
            {
                angle_variation = ((rand() % 1024) / 1024.0) * spread - (spread / 2.0);
            }

            double speed_factor = 1.0;
            if (speed_variation > 0.0)
            {
                speed_factor = ((rand() % 1024) / 1024.0) * (2.0 * speed_variation) + (1.0 - speed_variation);
            }

            Math::vec2 random_direction = Math::RotationMatrix(angle_variation) * direction * speed_factor;

            Math::vec2 particle_velocity = random_direction + emitter_velocity;

            particle->Start(position, particle_velocity, max_life);

            next_index++;
            if (next_index >= particles.size())
            {
                next_index = 0;
            }
        }
    }

    void ParticleEmitter::Update(double dt)
    {
        for (auto& particle : particles)
        {
            particle->Update(dt);
        }
    }

    void ParticleEmitter::Draw(Math::TransformationMatrix camera_matrix) const
    {
        for (const auto& particle : particles)
        {
            particle->Draw(camera_matrix);
        }
    }

    int ParticleEmitter::GetActiveCount() const
    {
        int count = 0;
        for (const auto& particle : particles)
        {
            if (particle->IsAlive())
            {
                count++;
            }
        }
        return count;
    }
}
