/**
 * \file FontManager.hpp
 * \brief
 * \author
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once

#include "Font.hpp"
#include "Vec2.hpp"
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

namespace CS230
{
    enum class TextAlign
    {
        Left,
        Center,
        Right
    };

    class FontManager
    {
    public:
        static FontManager& Instance()
        {
            static FontManager instance;
            return instance;
        }

        void Load(const std::string& name, const std::filesystem::path& path, int pointSize = 24);

        std::shared_ptr<Font> Get(const std::string& name) const;

        void DrawText(
            const std::string& fontName, const std::string& text, Math::fvec2 pos, TextAlign align = TextAlign::Left, CS200::RGBA color = 0xFFFFFFFF, Math::vec2 scale = Math::vec2{ 1.0, 1.0 });

        void Clear() { fonts.clear(); }

    private:
        std::unordered_map<std::string, std::shared_ptr<Font>> fonts;

        FontManager() = default;
    };
}
