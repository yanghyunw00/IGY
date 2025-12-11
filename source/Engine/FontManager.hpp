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

        void Load(const std::string& name, const std::filesystem::path& path)
        {
            if (fonts.find(name) == fonts.end())
            {
                fonts[name] = std::make_shared<Font>(path);
            }
        }

        std::shared_ptr<Font> Get(const std::string& name) const
        {
            auto it = fonts.find(name);
            if (it == fonts.end())
            {
                return nullptr;
            }
            return it->second;
        }

        void DrawText(const std::string& fontName,
                      const std::string& text,
                      Math::fvec2        pos,
                      TextAlign          align = TextAlign::Left,
                      CS200::RGBA        color = 0xFFFFFFFF,
                      Math::vec2         scale = Math::vec2{ 1.0, 1.0 })
        {
            auto font = Get(fontName);
            if (!font)
            {
                return;
            }

            Math::ivec2 size    = font->measureText(text);
            Math::vec2  drawPos = Math::vec2{ static_cast<double>(pos.x), static_cast<double>(pos.y) };

            const double scaledWidth = static_cast<double>(size.x) * scale.x;

            if (align == TextAlign::Center)
            {
                drawPos.x -= scaledWidth * 0.5;
            }
            else if (align == TextAlign::Right)
            {
                drawPos.x -= scaledWidth;
            }

            const auto transform = Math::TranslationMatrix(drawPos) * Math::ScaleMatrix(scale);

            if (auto tex = font->PrintToTexture(text, color))
            {
                tex->Draw(transform, color);
            }
        }

    private:
        std::unordered_map<std::string, std::shared_ptr<Font>> fonts;

        FontManager() = default;
    };
}
