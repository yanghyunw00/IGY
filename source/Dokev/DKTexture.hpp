/**
 * \file DKTexture.hpp
 * \brief texture loading and drawing functions
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once

#include "CS200/RGBA.hpp"
#include "Engine/Vec2.hpp"
#include <filesystem>
#include <memory>
#include <string>
#include "Dokev/DKRenderState.hpp"
namespace CS230
{
    class Texture;
}

namespace Dokev
{
    class Image;


    class Texture2D
    {
    public:
 
        explicit Texture2D(const std::filesystem::path& fileName);

        Texture2D() = default;

        friend Texture2D LoadTextureFromImage(const Image& image);
        friend Texture2D LoadImageFromTextrue(const Texture2D& texture);

        bool IsValid() const { return texture != nullptr; }

        int GetWidth() const;

        int GetHeight() const;

        Math::ivec2 GetSize() const;

        void Draw(int posX, int posY, CS200::RGBA tint = CS200::WHITE,Dokev::RenderState selected_renderer = Dokev::RenderState::INSTANCED) const;

        void Draw(Math::vec2 position, CS200::RGBA tint = CS200::WHITE) const;

        void DrawEx(int posX, int posY, float rotation, float scale, CS200::RGBA tint = CS200::WHITE) const;

        void DrawEx(Math::vec2 position, float rotation, Math::vec2 scale, CS200::RGBA tint = CS200::WHITE) const;

        void DrawPro(int sourceX, int sourceY, int sourceWidth, int sourceHeight,
                     int destX, int destY, CS200::RGBA tint = CS200::WHITE) const;

        void DrawPro(Math::vec2 position, float rotation, Math::vec2 scale,
                     Math::vec2 origin, CS200::RGBA tint = CS200::WHITE) const;


        void setTexture(std::shared_ptr<CS230::Texture> texture_doodoobird)
        {
            texture = texture_doodoobird;
        }

    private:
        std::shared_ptr<CS230::Texture> texture;
    };
    

    Texture2D LoadTexture(const std::string& fileName);

    void UnLoadTexture(Texture2D texture);

    void DrawTexture(const Texture2D& texture, int posX, int posY, CS200::RGBA tint,Dokev::RenderState selected_renderer = Dokev::RenderState::INSTANCED);

    void DrawTextureV(const Texture2D& texture, Math::vec2 position, CS200::RGBA tint = CS200::WHITE);

    void DrawTextureEx(const Texture2D& texture, Math::vec2 position, float rotation, float scale, CS200::RGBA tint = CS200::WHITE);

    void DrawTextureRec(const Texture2D& texture, int sourceX, int sourceY, int sourceWidth, int sourceHeight,
                        Math::vec2 position, CS200::RGBA tint = CS200::WHITE);

    Texture2D LoadTextureFromImage(const Image& image);
    Texture2D LoadImageFromTextrue(const Texture2D& texture);

}
