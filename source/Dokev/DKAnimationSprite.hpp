#pragma once
#include "Dokev/DKTexture.hpp"
#include "Engine/Vec2.hpp"
#include "Engine/Matrix.hpp"
#include "Dokev/DKAnimation.hpp"

class AnimatedSprite
  {
  public:
      Dokev::Texture2D texture;
      Dokev::Animation currentAnimation;
      Math::ivec2 frameSize;  

      void Update(double dt);

      void Draw(Math::TransformationMatrix transform);
  };