/**
 * \file FontManager.cpp
 * \brief
 * \author
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "FontManager.hpp"

namespace CS230
{


    void FontManager::Load(const std::string& name, const std::filesystem::path& path, int pointSize)
    {
        if (fonts.find(name) == fonts.end())
        {
            fonts[name] = std::make_shared<Font>(path, pointSize);
        }
    }

    std::shared_ptr<Font> FontManager::Get(const std::string& name) const
    {
        auto it = fonts.find(name);
        if (it == fonts.end())
        {
            return nullptr;
        }
        return it->second;
    }

    void FontManager::DrawText(const std::string& fontName,
                               const std::string& text,
                               Math::fvec2        pos,
                               TextAlign          align,
                               CS200::RGBA        color,
                               Math::vec2         scale)
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
}
