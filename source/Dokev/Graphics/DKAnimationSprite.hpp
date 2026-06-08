/**
 * \file DKAnimationSprite.hpp
 * \brief
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once

#include "DKAnimation.hpp"
#include "DKSpriteSheet.hpp"
#include "DKTexture.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Vec2.hpp"
#include <map>
#include <string>


namespace Dokev
{
    class AnimatedSprite
    {
    public:
        AnimatedSprite() = default;

        explicit AnimatedSprite(const std::filesystem::path& spt_file);


        AnimatedSprite(Texture2D texture, Math::ivec2 frameSize);


        void AddAnimation(const std::string& name, Animation animation);

        void AddAnimationFromFile(const std::string& name, const std::filesystem::path& anm_file);


        void PlayAnimation(const std::string& name, bool reset = true);

        void Update(double dt);
        void Draw(Math::TransformationMatrix transform) const;

        // Same positioning semantics as DrawTexture(posX, posY):
        // places the frame with its top-left at (posX, posY)
        void DrawAtPosition(
            int posX, int posY, CS200::RGBA tint = CS200::WHITE, Math::vec2 scale = { 1.0, 1.0 },
            Dokev::RenderState selected_renderer = Dokev::RenderState::INSTANCED) const;
        void DrawAtTexturePosition(
            int posX, int posY, CS200::RGBA tint = CS200::WHITE, Math::vec2 scale = { 1.0, 1.0 },
            Dokev::RenderState selected_renderer = Dokev::RenderState::INSTANCED) const;

        const std::string& GetCurrentAnimationName() const;
        bool               IsAnimationComplete() const;

        Animation&       GetAnimation(const std::string& name);
        const Animation& GetAnimation(const std::string& name) const;

        const SpriteSheet& GetSpriteSheet() const
        {
            return spriteSheet;
        }

        Math::vec2 GetHotSpot() const
        {
            return spriteSheet.GetHotSpot();
        }

    private:
        Math::vec4 GetCurrentFrameUV() const;

        SpriteSheet                      spriteSheet;
        std::map<std::string, Animation> animations;
        std::string                      currentAnimationName;
        Animation*                       currentAnimation{ nullptr };
    };
}
