/**
 * \file DKParticleEmitter.cpp
 * \brief
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "DKParticleEmitter.hpp"
#include "DKMath.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Logger.hpp"
#include <cmath>
#include <cstdlib>

namespace Dokev
{
    AnimatedParticle::AnimatedParticle(const std::filesystem::path& spt_file) : sprite(spt_file)
    {
        life = 0.0;
    }

    void AnimatedParticle::Start([[maybe_unused]] Math::vec2 position_, [[maybe_unused]] Math::vec2 velocity_, double max_life)
    {
        this->position = position_;
        this->velocity = velocity_;
        this->life     = max_life;

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
}
