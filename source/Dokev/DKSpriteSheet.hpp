/**
 * \file DKSpriteSheet.hpp
 * \brief Sprite sheet information manager
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#pragma once

#include "DKTexture.hpp"
#include "Engine/Vec2.hpp"
#include <vector>
#include <filesystem>

namespace Dokev
{
    class SpriteSheet
    {
    public:

        SpriteSheet() = default;
        

        explicit SpriteSheet(const std::filesystem::path& spt_file);
        

        SpriteSheet(Texture2D texture, 
                    Math::ivec2 frameSize,
                    std::vector<Math::ivec2> framePositions,
                    Math::vec2 hotSpot = {0, 0});
        
        Texture2D GetTexture() const { return texture; }
        

        Math::vec2 GetHotSpot() const { return hotSpot; }

        Math::ivec2 GetFrameSize() const { return frameSize; }

        Math::ivec2 GetFramePosition(int frameIndex) const;
        
        Math::vec4 GetFrameUV(int frameIndex) const;
        

        int GetFrameCount() const { return static_cast<int>(framePositions.size()); }

        const std::string& GetAnimationFile() const { return animationFile; }
        

        bool IsValid() const { return texture.IsValid() && !framePositions.empty(); }

    private:
        Texture2D                   texture;
        Math::vec2                  hotSpot{ 0, 0 };
        Math::ivec2                 frameSize{ 0, 0 };
        std::vector<Math::ivec2>    framePositions;
        std::string                 animationFile;
    };
}