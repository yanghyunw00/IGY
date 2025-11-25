/**
 * \file DkTexture.cpp
 * \brief Implementation of texture functions
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "DKTexture.hpp"
#include "DKImage.hpp"
#include "DKMath.hpp"

#include "Engine/Engine.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Texture.hpp"
#include "Engine/TextureManager.hpp"

#include "OpenGL/GL.hpp"
#include "OpenGL/GLTypes.hpp"
#include "OpenGL/Texture.hpp"
#include <numbers>

namespace Dokev
{
    

    Texture2D::Texture2D(const std::filesystem::path& fileName)
    {
        texture = Engine::GetTextureManager().Load(fileName);
    }

    int Texture2D::GetWidth() const
    {
        if (!texture)
        {
            return 1;
        }
        return texture->GetSize().x;
    }

    int Texture2D::GetHeight() const
    {
        if (!texture)
        {
            return 1;
        }
        return texture->GetSize().y;
    }

    Math::ivec2 Texture2D::GetSize() const
    {
        if (!texture)
        {
            return { 1, 1 };
        }
        return texture->GetSize();
    }

    void Texture2D::Draw(int posX, int posY, CS200::RGBA tint,Dokev::RenderState selected_renderer) const
    {
        if (!texture)
        {
            return;
        }

        const auto size = texture->GetSize();
        Math::vec2 adjustedPosition{ static_cast<double>(posX), static_cast<double>(posY) - static_cast<double>(size.y) };

        auto transform = Math::TranslationMatrix(adjustedPosition);

        texture->Draw(transform, tint,selected_renderer);
    }

    void Texture2D::Draw(Math::vec2 position, CS200::RGBA tint) const
    {
        Draw(static_cast<int>(position.x), static_cast<int>(position.y), tint);
    }

    void Texture2D::DrawEx(int posX, int posY, float rotation, float scale, CS200::RGBA tint) const
    {
        if (!texture)
        {
            return;
        }

        Math::vec2 position{ static_cast<double>(posX), static_cast<double>(posY) };

        double rad = static_cast<double>(rotation) * PI / 180.0;
        Math::vec2 scaleVec{ scale, scale };

        auto transform = Math::TranslationMatrix(position) * Math::RotationMatrix(rad) * Math::ScaleMatrix(scaleVec);

        texture->Draw(transform, tint);
    }

    void Texture2D::DrawEx(Math::vec2 position, float rotation, Math::vec2 scale, CS200::RGBA tint) const
    {
        if (!texture)
        {
            return;
        }

        double rad = static_cast<double>(rotation) * PI / 180.0;

        auto transform = Math::TranslationMatrix(position) * Math::RotationMatrix(rad) * Math::ScaleMatrix(scale);

        texture->Draw(transform, tint);
    }

    void Texture2D::DrawPro(int sourceX, int sourceY, int sourceWidth, int sourceHeight, int destX, int destY, CS200::RGBA tint) const
    {
        if (!texture)
        {
            return;
        }

        Math::vec2 center{ static_cast<double>(destX) + static_cast<double>(sourceWidth) * 0.5, static_cast<double>(destY) + static_cast<double>(sourceHeight) * 0.5 };

        Math::vec2 scale{ 1.0, 1.0 };

        auto transform = Math::TranslationMatrix(center) * Math::ScaleMatrix(scale);

        Math::ivec2 texelPos{ sourceX, sourceY };
        Math::ivec2 frameSize{ sourceWidth, sourceHeight };

        texture->Draw(transform, texelPos, frameSize, tint);
    }

    void Texture2D::DrawPro(Math::vec2 position, float rotation, Math::vec2 scale, Math::vec2 origin, CS200::RGBA tint) const
    {
        if (!texture)
        {
            return;
        }

        const auto size = texture->GetSize();

        double rad = static_cast<double>(rotation) * PI / 180.0;

        Math::vec2 finalScale{ static_cast<double>(size.x) * scale.x, static_cast<double>(size.y) * scale.y };

        Math::vec2 center{ position.x + static_cast<double>(size.x) * 0.5 * scale.x, position.y + static_cast<double>(size.y) * 0.5 * scale.y };

        auto transform = Math::TranslationMatrix(center) * Math::RotationMatrix(rad) * Math::TranslationMatrix(-origin) * Math::ScaleMatrix(finalScale);

        texture->Draw(transform, tint);
    }

    Texture2D LoadTexture(const std::string& fileName)
    {
        return Texture2D(fileName);
    }

    void UnLoadTexture([[maybe_unused]] Texture2D texture)
    {
        // dont need to implement -> sharedptr!
    }

    void DrawTexture(const Texture2D& texture, int posX, int posY, CS200::RGBA tint,Dokev::RenderState selected_renderer)
    {
        texture.Draw(posX, posY, tint,selected_renderer);
    }

    void DrawTextureV(const Texture2D& texture, Math::vec2 position, CS200::RGBA tint)
    {
        texture.Draw(position, tint);
    }

    void DrawTextureEx(const Texture2D& texture, Math::vec2 position, float rotation, float scale, CS200::RGBA tint)
    {
        texture.DrawEx(position, rotation, Math::vec2{ scale, scale }, tint);
    }

    void DrawTextureRec(const Texture2D& texture, int sourceX, int sourceY, int sourceWidth, int sourceHeight, Math::vec2 position, CS200::RGBA tint)
    {
        texture.DrawPro(sourceX, sourceY, sourceWidth, sourceHeight, static_cast<int>(position.x), static_cast<int>(position.y), tint);
    }

    Texture2D LoadTextureFromImage(const Image& image)
    {
        if (!image.IsValid())
        {
            return Texture2D();
        }

        Math::ivec2 size{ image.GetWidth(), image.GetHeight() };
        auto        engineTexture = Engine::GetTextureManager().CreateFromPixels(size, image.GetPixels());

        Texture2D result;
        result.texture = engineTexture;
        return result;
    }

    Texture2D LoadImageFromTextrue(const Texture2D& texture)
    {
        if (!texture.IsValid())
        {
            return Texture2D();
        }

        int   width  = texture.GetWidth();
        int   height = texture.GetHeight();
        Image img(width, height);

        // to read the texture pixel
        GLuint fbo = 0;
        GL::GenFramebuffers(1, &fbo);
        GL::BindFramebuffer(GL_FRAMEBUFFER, fbo);

        auto handle = texture.texture->GetHandle();
        GL::FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, handle, 0);
        
        GL::ReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, img.GetPixels().data());

        GL::BindFramebuffer(GL_FRAMEBUFFER, 0);
        GL::DeleteFramebuffers(1, &fbo);

        ImageVerticalFlip(img);

        return LoadTextureFromImage(img);
    }


}
