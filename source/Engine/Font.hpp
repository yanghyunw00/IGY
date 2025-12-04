/**
 * \file
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#pragma once

#include "Matrix.hpp"
#include "Rect.hpp"
#include "Texture.hpp"
#include "Vec2.hpp"
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>


constexpr int CHAR_COUNT = 'z' - ' ' + 1;

namespace CS230
{
    /**
     * \brief Bitmap font system for rendering text to textures
     *
     * Font parses bitmap font images and renders text strings to textures
     * with caching for performance optimization.
     */
    class Font
    {
    public:

        explicit Font(const std::filesystem::path& file_name);
        static void IncrementFrame() { current_frame++; }

        std::shared_ptr<Texture> PrintToTexture(const std::string& text, CS200::RGBA color = 0xFFFFFFFF);

    private:

        struct Cache
        {
            std::shared_ptr<Texture> texture;
            int                      leastUsedFrame;
        };

        void         FindCharRects(const CS200::Image& image);
        Math::ivec2  measureText(const std::string& text);
        void         drawChar(char c, Math::TransformationMatrix& matrix, CS200::RGBA color);
        Math::irect& GetCharRect(char c);

        std::shared_ptr<Texture>               font_texture;
        Math::irect                            char_rects[CHAR_COUNT];
        std::unordered_map<std::string, Cache> text_cache;
        static int                             current_frame;
    };
}