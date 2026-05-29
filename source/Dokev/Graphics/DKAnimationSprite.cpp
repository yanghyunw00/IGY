/**
 * \file DKAnimationSprite.cpp
 * \brief
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "DKAnimationSprite.hpp"
#include "DKTexture.hpp"

namespace Dokev
{
    AnimatedSprite::AnimatedSprite(const std::filesystem::path& spt_file) : spriteSheet(spt_file)
    {
        const std::string& animFile = spriteSheet.GetAnimationFile();
        if (!animFile.empty())
        {
            try
            {
                AddAnimationFromFile("default", animFile);
                PlayAnimation("default", false);
            }
            catch (const std::exception& e)
            {
                printf("DEBUG: Failed to load animation: %s\n", e.what());
            }
        }
        else
        {
            printf("DEBUG: No animation file specified\n");
        }
    }

    AnimatedSprite::AnimatedSprite([[maybe_unused]] Texture2D texture, [[maybe_unused]] Math::ivec2 frameSize)
    {
    }

    void AnimatedSprite::AddAnimation(const std::string& name, Animation animation)
    {
        animations[name] = std::move(animation);

        if (currentAnimation == nullptr)
        {
            PlayAnimation(name, false);
        }
    }

    void AnimatedSprite::AddAnimationFromFile(const std::string& name, const std::filesystem::path& anm_file)
    {
        Animation anim(anm_file);
        AddAnimation(name, std::move(anim));
    }

    void AnimatedSprite::PlayAnimation(const std::string& name, bool reset)
    {
        auto it = animations.find(name);
        if (it == animations.end())
        {
            printf("DEBUG: Animation '%s' not found!\n", name.c_str());
            return;
        }

        if (currentAnimationName == name && !reset)
        {
            return;
        }

        currentAnimationName = name;
        currentAnimation     = &it->second;

        if (reset)
        {
            currentAnimation->Reset();
        }
    }

    void AnimatedSprite::Update(double dt)
    {
        if (currentAnimation != nullptr && spriteSheet.IsValid())
        {
            currentAnimation->Update(dt);
        }
    }

    void AnimatedSprite::Draw(Math::TransformationMatrix transform) const
    {
        if (currentAnimation == nullptr)
        {
            printf("currentAnimation is null\n");
            return;
        }

        if (!spriteSheet.IsValid())
        {
            printf("spriteSheet is invalid\n");
            return;
        }

        Texture2D texture = spriteSheet.GetTexture();
        if (!texture.IsValid())
        {
            printf("texture is invalid\n");
            return;
        }

        int frameIndex = currentAnimation->GetCurrentSpriteFrame();

        // Validate frame index
        if (frameIndex < 0 || frameIndex >= spriteSheet.GetFrameCount())
        {
            printf("Frame index %d out of range (0-%d)\n", frameIndex, spriteSheet.GetFrameCount() - 1);
            return;
        }

        Math::ivec2 framePos  = spriteSheet.GetFramePosition(frameIndex);
        Math::ivec2 frameSize = spriteSheet.GetFrameSize();

        Math::vec2 position = transform * Math::vec2(1.0, 1.0);

        Dokev::DrawTextureRec(texture, framePos.x, framePos.y, frameSize.x, frameSize.y, position);
    }

    const std::string& AnimatedSprite::GetCurrentAnimationName() const
    {
        return currentAnimationName;
    }

    bool AnimatedSprite::IsAnimationComplete() const
    {
        return currentAnimation != nullptr && currentAnimation->IsComplete();
    }

    Animation& AnimatedSprite::GetAnimation(const std::string& name)
    {
        return animations.at(name);
    }

    const Animation& AnimatedSprite::GetAnimation(const std::string& name) const
    {
        return animations.at(name);
    }

    void AnimatedSprite::DrawAtPosition(int posX, int posY) const
    {
        if (currentAnimation == nullptr || !spriteSheet.IsValid())
            return;

        int         frameIndex = currentAnimation->GetCurrentSpriteFrame();
        Math::ivec2 fp         = spriteSheet.GetFramePosition(frameIndex);
        Math::ivec2 fs         = spriteSheet.GetFrameSize();
        Texture2D   tex        = spriteSheet.GetTexture();

        int destX = posX - fs.x / 2;
        int destY = posY - fs.y * 3 / 2;
        tex.DrawPro(fp.x, fp.y, fs.x, fs.y, destX, destY);
    }

    Math::vec4 AnimatedSprite::GetCurrentFrameUV() const
    {
        if (currentAnimation == nullptr)
            return { 0.0f, 0.0f, 1.0f, 1.0f };

        int frameIndex = currentAnimation->GetCurrentSpriteFrame();
        return spriteSheet.GetFrameUV(frameIndex);
    }
}