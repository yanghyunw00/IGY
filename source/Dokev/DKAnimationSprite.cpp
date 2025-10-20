/**
 * \file DKAnimationSprite.cpp
 * \brief 
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "DKAnimationSprite.hpp"

void AnimatedSprite::Update(double dt)
{
    currentAnimation.Update(dt);
}

void AnimatedSprite::Draw([[maybe_unused]] Math::TransformationMatrix transform)
{
    [[maybe_unused]] int         currentFrame = currentAnimation.GetCurrentSpriteFrame();
    [[maybe_unused]]  Math::ivec2 texelPos     = { currentFrame * frameSize.x, 0 };
    // Drawtexture with Transform matrix 
    // DrawTextureT(texelPos, transform,  framesize) like this
}
