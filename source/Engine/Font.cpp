/**
 * \file
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "Font.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Logger.hpp"
#include "OpenGL/Texture.hpp"

#include <SDL_ttf.h>
#include <vector>

namespace CS230
{
    int Font::current_frame = 0;

    // Convenience cast — ttf_font stored as void* to hide SDL_ttf.h from the header
    static TTF_Font* AsFont(void* p) { return static_cast<TTF_Font*>(p); }

    Font::Font(const std::filesystem::path& file_name, int point_size)
    {
        ttf_font = TTF_OpenFont(file_name.string().c_str(), point_size);
        if (!ttf_font)
            Engine::GetLogger().LogError(std::string("Failed to load font: ") + file_name.string() + " - " + TTF_GetError());
    }

    Font::~Font()
    {
        text_cache.clear();
        if (ttf_font)
        {
            TTF_CloseFont(AsFont(ttf_font));
            ttf_font = nullptr;
        }
    }

    Math::ivec2 Font::measureText(const std::string& text)
    {
        if (!ttf_font || text.empty())
            return { 0, 0 };
        int w = 0, h = 0;
        TTF_SizeUTF8(AsFont(ttf_font), text.c_str(), &w, &h);
        return { w, h };
    }

    std::shared_ptr<Texture> Font::PrintToTexture(const std::string& text, CS200::RGBA /*color*/)
    {
        if (!ttf_font || text.empty())
            return nullptr;

        // Evict stale cache entries
        {
            std::vector<std::string> trash;
            for (const auto& [key, cache] : text_cache)
            {
                if ((current_frame - cache.leastUsedFrame > 60) && cache.texture.use_count() == 1)
                    trash.push_back(key);
            }
            for (const auto& key : trash)
                text_cache.erase(key);
        }

        // Cache key is text only; colour tinting is applied by DrawQuad at draw time
        const std::string& cache_key = text;
        auto it = text_cache.find(cache_key);
        if (it != text_cache.end())
        {
            it->second.leastUsedFrame = current_frame;
            return it->second.texture;
        }

        // Render text in white — colour tint is applied later by the shader
        const SDL_Color white{ 255, 255, 255, 255 };
        SDL_Surface* surface = TTF_RenderUTF8_Blended(AsFont(ttf_font), text.c_str(), white);
        if (!surface)
        {
            Engine::GetLogger().LogError(std::string("TTF_RenderUTF8_Blended failed: ") + TTF_GetError());
            return nullptr;
        }

        const int w = surface->w;
        const int h = surface->h;

        // Convert SDL ARGB8888 to GL_RGBA byte order (R,G,B,A in memory) with vertical flip.
        // SDL_GetRGBA handles the pixel format portably.
        std::vector<CS200::RGBA> pixels(static_cast<size_t>(w * h));

        SDL_LockSurface(surface);
        const int   pitch_px = surface->pitch / 4;
        const auto* src      = static_cast<const uint32_t*>(surface->pixels);
        for (int row = 0; row < h; ++row)
        {
            const int dst_row = h - 1 - row;   // flip Y for OpenGL bottom-up storage
            for (int col = 0; col < w; ++col)
            {
                uint8_t r, g, b, a;
                SDL_GetRGBA(src[row * pitch_px + col], surface->format, &r, &g, &b, &a);
                // Pack with R in lowest byte so GL_RGBA GL_UNSIGNED_BYTE reads correctly
                pixels[static_cast<size_t>(dst_row * w + col)] =
                    static_cast<uint32_t>(r)         |
                    (static_cast<uint32_t>(g) <<  8) |
                    (static_cast<uint32_t>(b) << 16) |
                    (static_cast<uint32_t>(a) << 24);
            }
        }
        SDL_UnlockSurface(surface);
        SDL_FreeSurface(surface);

        auto handle      = OpenGL::CreateTextureFromMemory({ w, h }, pixels, OpenGL::Filtering::Linear, OpenGL::Wrapping::ClampToEdge);
        auto new_texture = std::shared_ptr<Texture>(new Texture(handle, { w, h }));

        text_cache[cache_key] = { new_texture, current_frame };
        return new_texture;
    }
}
