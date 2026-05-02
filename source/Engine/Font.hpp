/**
 * \file
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#pragma once

#include "Texture.hpp"
#include "Vec2.hpp"
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

namespace CS230
{
    class Font
    {
    public:
        Font(const std::filesystem::path& file_name, int point_size = 24);
        ~Font();

        static void IncrementFrame() { current_frame++; }

        std::shared_ptr<Texture> PrintToTexture(const std::string& text, CS200::RGBA color = 0xFFFFFFFF);
        Math::ivec2 measureText(const std::string& text);

    private:
        struct Cache
        {
            std::shared_ptr<Texture> texture;
            int                      leastUsedFrame;
        };

        void*                                     ttf_font     = nullptr; // TTF_Font*, hidden to avoid SDL_ttf.h in header
        std::unordered_map<std::string, Cache>    text_cache;
        static int                                current_frame;
    };
}
