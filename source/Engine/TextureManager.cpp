/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "TextureManager.hpp"
#include "CS200/IRenderer2D.hpp"
#include "CS200/Image.hpp"
#include "CS200/NDC.hpp"
#include "Engine.hpp"
#include "Engine/Window.hpp"
#include "Logger.hpp"
#include "OpenGL/GL.hpp"
#include <iostream>


namespace CS230
{

    std::shared_ptr<CS230::Texture> CS230::TextureManager::Load(const std::filesystem::path& file_name)
    {
        const std::string key = file_name.string();

        auto it = texture_cache.find(key);
        if (it != texture_cache.end())
        {
            return it->second;
        }

        try
        {
            CS200::Image                image(file_name, true);
            const OpenGL::TextureHandle handle      = OpenGL::CreateTextureFromImage(image, OpenGL::Filtering::NearestPixel, OpenGL::Wrapping::ClampToEdge);
            auto                        new_texture = std::shared_ptr<Texture>(new Texture(handle, image.GetSize()));
            texture_cache[key]                      = new_texture;
            return new_texture;
        }
        catch (const std::runtime_error& e)
        {
            Engine::GetLogger().LogError("Failed to load texture: " + std::string(e.what()));
            return nullptr;
        }
    }

    std::shared_ptr<Texture> TextureManager::CreateFromPixels(Math::ivec2 size, std::span<const CS200::RGBA> pixels)
    {
        try
        {
            const OpenGL::TextureHandle handle      = OpenGL::CreateTextureFromMemory(size, pixels, OpenGL::Filtering::NearestPixel, OpenGL::Wrapping::ClampToEdge);
            auto                        new_texture = std::shared_ptr<Texture>(new Texture(handle, size));
            return new_texture;
        }
        catch (const std::runtime_error& e)
        {
            Engine::GetLogger().LogError("Failed to create texture from pixels: " + std::string(e.what()));
            return nullptr;
        }
    }

    void TextureManager::Unload()
    {
        texture_cache.clear();
    }

    void CS230::TextureManager::StartRenderTextureMode(int width, int height)
    {
        if (is_rendering)
        {
            return;
        }
        Engine::GetRenderer2D().EndScene();
        GL::GetIntegerv(GL_VIEWPORT, curr.viewport);
        GL::GetFloatv(GL_COLOR_CLEAR_VALUE, curr.clearColor);
        curr.size = { width, height };
        curr.fbo  = OpenGL::CreateFramebufferWithColor({ width, height });

        OpenGL::Bind(curr.fbo);

        GL::Viewport(0, 0, width, height);
        GL::ClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        GL::Clear(GL_COLOR_BUFFER_BIT);

        Math::TransformationMatrix ndc = CS200::build_ndc_matrix({ width, height });
        Engine::GetRenderer2D().BeginScene(ndc);

        is_rendering = true;
    }

    std::shared_ptr<CS230::Texture> CS230::TextureManager::EndRenderTextureMode()
    {
        if (!is_rendering)
        {
            return nullptr;
        }

        Engine::GetRenderer2D().EndScene();

        OpenGL::Unbind();

        GL::ClearColor(curr.clearColor[0], curr.clearColor[1], curr.clearColor[2], curr.clearColor[3]);
        GL::Viewport(curr.viewport[0], curr.viewport[1], curr.viewport[2], curr.viewport[3]);

        OpenGL::TextureHandle texture_handle = curr.fbo.ColorAttachment;

        GL::DeleteFramebuffers(1, &curr.fbo.Framebuffer);

        auto final_texture = std::shared_ptr<Texture>(new Texture(texture_handle, curr.size));
        is_rendering       = false;
        curr.fbo           = {};

        Math::ivec2                window_size = Engine::GetWindow().GetSize();
        Math::TransformationMatrix ndc         = CS200::build_ndc_matrix(window_size);
        Engine::GetRenderer2D().BeginScene(ndc);

        return final_texture;
    }

}
