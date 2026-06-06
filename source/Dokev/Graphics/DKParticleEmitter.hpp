/**
 * \file DKParticleEmitter.hpp
 * \brief Simple particle emitter system using AnimatedSprite
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once

#include "DKAnimationSprite.hpp"
#include "Dokev/Core/DKUtility.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Vec2.hpp"
#include <filesystem>
#include <memory>
#include <vector>

namespace Dokev
{

    class AnimatedParticle
    {
    public:
        AnimatedParticle(const std::filesystem::path& spt_file);

        void Start(Math::vec2 position_, Math::vec2 velocity_, double max_life);
        void Update(double dt);
        void Draw(Math::TransformationMatrix camera_matrix) const;

        bool IsAlive() const
        {
            return life > 0;
        }

        virtual ~AnimatedParticle()    = default;
        virtual std::string TypeName() = 0;

    private:
        AnimatedSprite sprite;
        Math::vec2     position{ 0, 0 };
        Math::vec2     velocity{ 0, 0 };
        double         life{ 0.0 };
    };

    template <typename T>
    class ParticleEmitter
    {
    public:

        ParticleEmitter();
        ~ParticleEmitter() = default;
        void Emit(int count, Math::vec2 position, Math::vec2 emitter_velocity, Math::vec2 direction, double spread = 0.0, double speed_variation = 0.5);


        void Update(double dt);
        void Draw(Math::TransformationMatrix camera_matrix) const;
        int  GetActiveCount() const;

    private:
        std::vector<T*> particles;
        int             next_index{ 0 };
    };

    template <typename T>
    ParticleEmitter<T>::ParticleEmitter()
    {
        for (int i = 0; i < T::MaxCount; i++)
        {
            T* particle = new T();
            particles.push_back(particle);
        }
        next_index = 0;
    }

    template <typename T>

    void ParticleEmitter<T>::Emit(int count, Math::vec2 position, Math::vec2 emitter_velocity, Math::vec2 direction, double spread, double speed_variation)
    {
        for (int i = 0; i < count; i++)
        {
            AnimatedParticle* particle = particles[next_index];

            if (particle->IsAlive())
            {
                printf("Particle overwritten");
            }

            double angle_variation = 0.0;
            if (spread != 0.0)
            {
                angle_variation = Dokev::GetRandomFloat(0.0f, 1.0f) * spread - (spread / 2.0);
            }

            double speed_factor = 1.0;
            if (speed_variation > 0.0)
            {
                speed_factor = Dokev::GetRandomFloat(0.0f, 1.0f) * (2.0 * speed_variation) + (1.0 - speed_variation);
            }

            Math::vec2 random_direction = Math::RotationMatrix(angle_variation) * direction * speed_factor;

            Math::vec2 particle_velocity = random_direction + emitter_velocity;

            particle->Start(position, particle_velocity, T::MaxLife);

            next_index++;
            if (next_index >= particles.size())
            {
                next_index = 0;
            }
        }
    }

    template <typename T>
    void ParticleEmitter<T>::Update(double dt)
    {
        for (auto& particle : particles)
        {
            particle->Update(dt);
        }
    }

    template <typename T>
    void ParticleEmitter<T>::Draw(Math::TransformationMatrix camera_matrix) const
    {
        for (const auto& particle : particles)
        {
            particle->Draw(camera_matrix);
        }
    }

    template <typename T>
    int ParticleEmitter<T>::GetActiveCount() const
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