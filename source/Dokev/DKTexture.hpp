/**
 * \file DKTexture.hpp
 * \brief texture loading and drawing functions
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

 /*
Texture2D(const std::filesystem::path& fileName): Load texture from file path.

Texture2D(): Default constructor for Texture2D.

LoadTextureFromImage(const Image& image): Create Texture2D from Image object.

IsValid(): Check if texture is loaded and valid.

GetWidth(): Get texture width in pixels.

GetHeight(): Get texture height in pixels.

Draw(int posX, int posY, CS200::RGBA tint): Draw texture at integer position with optional tint.

Draw(Math::vec2 position, CS200::RGBA tint): Draw texture at vector position with optional tint.

DrawEx(int posX, int posY, float rotation, float scale, CS200::RGBA tint): Draw texture at integer position with rotation, scale, and tint.

DrawEx(Math::vec2 position, float rotation, Math::vec2 scale, CS200::RGBA tint): Draw texture at vector position with rotation, scale vector, and tint.

DrawPro(int sourceX, int sourceY, int sourceWidth, int sourceHeight, int destX, int destY, CS200::RGBA tint): Draw part of texture defined by source rectangle to integer destination position with tint.

DrawPro(Math::vec2 position, float rotation, Math::vec2 scale, Math::vec2 origin, CS200::RGBA tint): Draw texture with advanced transforms (position, rotation, scale, origin) and tint.

LoadTexture(const std::string& fileName): Load texture by filename and return Texture2D.

UnLoadTexture(Texture2D texture): Free texture resources.

DrawTexture(const Texture2D& texture, int posX, int posY, CS200::RGBA tint): Draw texture at integer position with tint.

DrawTextureV(const Texture2D& texture, Math::vec2 position, CS200::RGBA tint): Draw texture at vector position with tint.

DrawTextureEx(const Texture2D& texture, Math::vec2 position, float rotation, float scale, CS200::RGBA tint): Draw texture with rotation and scale at vector position with tint.

DrawTextureRec(const Texture2D& texture, int sourceX, int sourceY, int sourceWidth, int sourceHeight, Math::vec2 position, CS200::RGBA tint): Draw part of texture at vector position with tint.
 */

#pragma once

#include "CS200/RGBA.hpp"
#include "Engine/Vec2.hpp"
#include <filesystem>
#include <memory>
#include <string>

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

        void Draw(int posX, int posY, CS200::RGBA tint = CS200::WHITE) const;

        void Draw(Math::vec2 position, CS200::RGBA tint = CS200::WHITE) const;

        void DrawEx(int posX, int posY, float rotation, float scale, CS200::RGBA tint = CS200::WHITE) const;

        void DrawEx(Math::vec2 position, float rotation, Math::vec2 scale, CS200::RGBA tint = CS200::WHITE) const;

        void DrawPro(int sourceX, int sourceY, int sourceWidth, int sourceHeight,
                     int destX, int destY, CS200::RGBA tint = CS200::WHITE) const;

        void DrawPro(Math::vec2 position, float rotation, Math::vec2 scale,
                     Math::vec2 origin, CS200::RGBA tint = CS200::WHITE) const;

    private:
        std::shared_ptr<CS230::Texture> texture;
    };


    Texture2D LoadTexture(const std::string& fileName);

    void UnLoadTexture(Texture2D texture);

    void DrawTexture(const Texture2D& texture, int posX, int posY, CS200::RGBA tint = CS200::WHITE);

    void DrawTextureV(const Texture2D& texture, Math::vec2 position, CS200::RGBA tint = CS200::WHITE);

    void DrawTextureEx(const Texture2D& texture, Math::vec2 position, float rotation, float scale, CS200::RGBA tint = CS200::WHITE);

    void DrawTextureRec(const Texture2D& texture, int sourceX, int sourceY, int sourceWidth, int sourceHeight,
                        Math::vec2 position, CS200::RGBA tint = CS200::WHITE);

    Texture2D LoadTextureFromImage(const Image& image);
    Texture2D LoadImageFromTextrue(const Texture2D& texture);

}
