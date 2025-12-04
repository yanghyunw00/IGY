/**  * \file
  * \author Hyunwoo Yang
  * \date 2025 Fall
  * \par CS200 Computer Graphics I
  * \copyright DigiPen Institute of Technology
  */

#include "Font.hpp"
#include "CS200/IRenderer2D.hpp"
#include "CS200/Image.hpp"
#include "CS200/NDC.hpp"
#include "CS200/RenderingAPI.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Logger.hpp"
#include "Engine/TextureManager.hpp"
#include "Engine/Window.hpp"
#include "OpenGL/Framebuffer.hpp"
#include "OpenGL/GL.hpp"
#include <format>
#include <iostream>

namespace CS230
{
    int Font::current_frame = 0;

    Font::Font(const std::filesystem::path& file_name)
    {
        CS200::Image image(file_name, false);
        if (!image.data())
        {
            Engine::GetLogger().LogError("Failed to load font: " + file_name.string());
            return;
        }

        const CS200::RGBA white = 0xFFFFFFFF;
        const CS200::RGBA color = *reinterpret_cast<const CS200::RGBA*>(image.data());
        if (color != white)
        {
            Engine::GetLogger().LogError("first pixel is not white");
            return;
        }

        FindCharRects(image);
        CS200::Image flip(file_name, true);
        font_texture = std::shared_ptr<Texture>(new Texture(OpenGL::CreateTextureFromImage(flip), flip.GetSize()));
    }

    void Font::FindCharRects(const CS200::Image& image)
    {
        const CS200::RGBA* first_row = image.data();
        const Math::ivec2  size      = image.GetSize();
        const int          height    = size.y;

        CS200::RGBA check_color = first_row[0]; // 0,0
        CS200::RGBA next_color = 0xFFFFFFFF;

        int x = 1;
        for (int index = 0; index < CHAR_COUNT; index++)
        {
            int width = 0;

            do
            {
                width++;
                next_color = first_row[x + width];
            } while (check_color == next_color);

            check_color = next_color;

            char_rects[index].point1 = { x + width - 1, 1 };
            char_rects[index].point2 = { x, char_rects[index].point1.y + height - 1 };
            x += width;
        }
    }

    Math::ivec2 Font::measureText(const std::string& text)
    {
        int total_width = 0;
        int max_height  = 0;
        for (char c : text)
        {
            Math::irect& rect = GetCharRect(c);

            total_width += rect.Size().x;
            if (rect.Size().y > max_height)
            {
                max_height = rect.Size().y;
            }
        }

        return { total_width, max_height };
    }

    void Font::drawChar(char c, Math::TransformationMatrix& matrix, CS200::RGBA color)
    {
        Math::irect&      rect     = GetCharRect(c);
        const Math::ivec2 top_left = { rect.point2.x, rect.point1.y };
        if (c != ' ')
        {
            font_texture->Draw(matrix, top_left, rect.Size(), color);
        }
        matrix *= Math::TranslationMatrix(Math::ivec2{ (rect.Size().x), 0 });
    }

    Math::irect& Font::GetCharRect(char c)
    {
        if (c >= ' ' && c <= 'z')
        {
            return char_rects[c - ' '];
        }
        else
        {
            Engine::GetLogger().LogError("Char '" + std::to_string(c) + "' not found");
            return char_rects[0];
        }
    }

    std::shared_ptr<Texture> Font::PrintToTexture(const std::string& text, CS200::RGBA color)
    {
        {
            std::vector<std::string> trash_key;
            for (const auto& tc : text_cache)
            {
                const Cache& cache = tc.second;

                const bool old_frame = (current_frame - cache.leastUsedFrame > 60);
                const bool is_referenced = (cache.texture.use_count() == 1);

                if (old_frame && is_referenced)
                {
                    trash_key.push_back(tc.first);
                }
            }
            for (const auto& key : trash_key)
            {
                text_cache.erase(key);
            }
        }

        std::string cache_key = text + "_" + std::to_string(color);

        //cache hit
        auto it = text_cache.find(cache_key);
        if (it != text_cache.end())
        {
            it->second.leastUsedFrame = current_frame;
            return it->second.texture;
        }


        //-------------------------------------------//
        const Math::ivec2 text_size = measureText(text);
        if (text_size.x <= 0 || text_size.y <= 0)
        {
            return nullptr;
        }

        auto& texture_manager = Engine::GetTextureManager();
        texture_manager.StartRenderTextureMode(text_size.x, text_size.y);

        Math::TransformationMatrix transform_matrix{};
        for (char c : text)
        {
            drawChar(c, transform_matrix, color);
        }

        std::shared_ptr<Texture> new_texture = texture_manager.EndRenderTextureMode();

        if (new_texture)
        {
            text_cache[cache_key] = { new_texture, current_frame };
        }

        return new_texture;
    }

}