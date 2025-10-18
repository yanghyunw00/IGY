/**
 * \file
 * \author Rudy Castan
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "Texture.hpp"
#include "CS200/Image.hpp"
#include "Engine/Logger.hpp"
#include "Engine/Engine.hpp"
#include "Environment.hpp"
#include "GL.hpp"

namespace OpenGL
{
    TextureHandle CreateTextureFromImage(const CS200::Image& image, Filtering filtering, Wrapping wrapping) noexcept
    {
        if (image.data() == nullptr)
        {
            Engine::GetLogger().LogError("No image data !!");
            return 0;
        }

        return CreateTextureFromMemory(image.GetSize(), std::span(image.data(), static_cast<size_t>(image.GetSize().x * image.GetSize().y)), filtering, wrapping);
    }

    TextureHandle CreateTextureFromMemory(Math::ivec2 size, std::span<const CS200::RGBA> colors, Filtering filtering, Wrapping wrapping) noexcept
    {
        TextureHandle textureHandle;
        GL::GenTextures(1, &textureHandle);
        GL::BindTexture(GL_TEXTURE_2D, textureHandle);

        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(filtering));
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(filtering));
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping));
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapping));

        GL::TexImage2D(
            GL_TEXTURE_2D,
            0, // base_mipmap_level
            GL_RGBA8, size.x, size.y,
            0, // zero_border
            GL_RGBA, GL_UNSIGNED_BYTE, colors.data());

        GL::BindTexture(GL_TEXTURE_2D, 0);

        return textureHandle;
    }

    TextureHandle CreateRGBATexture(Math::ivec2 size, Filtering filtering, Wrapping wrapping) noexcept
    {
        TextureHandle textureHandle;
        GL::GenTextures(1, &textureHandle);
        GL::BindTexture(GL_TEXTURE_2D, textureHandle);

        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(filtering));
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(filtering));
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping));
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapping));

        GL::TexImage2D(
            GL_TEXTURE_2D,
            0, // base_mipmap_level
            GL_RGBA8, size.x, size.y,
            0, // zero_border
            GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        GL::BindTexture(GL_TEXTURE_2D, 0);

        return textureHandle;
    }

    void SetFiltering(TextureHandle texture_handle, Filtering filtering) noexcept
    {
        GL::BindTexture(GL_TEXTURE_2D, texture_handle);
        auto filtering_ = static_cast<GLint>(filtering);

        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering_);
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering_);

        GL::BindTexture(GL_TEXTURE_2D, 0);
    }

    void SetWrapping(TextureHandle texture_handle, Wrapping wrapping, TextureCoordinate coord) noexcept
    {
        GL::BindTexture(GL_TEXTURE_2D, texture_handle);
        auto wrapping_ = static_cast<GLint>(wrapping);

        if (coord == TextureCoordinate::S || coord == TextureCoordinate::Both)
        {
            GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping_);
        }

        if (coord == TextureCoordinate::T || coord == TextureCoordinate::Both)
        {
            GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping_);
        }

        GL::BindTexture(GL_TEXTURE_2D, 0);
    }
}
