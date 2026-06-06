/**
 * \file DKImage.hpp
 * \brief
 * \author Hyunseok Choi
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once

#include "CS200/RGBA.hpp"
#include <string>
#include <vector>

namespace Dokev
{

    enum class NoisePattern
    {
        Perlin = 0,
        Turbulence,
        Wood,
        Marble
    };

    class Image
    {
    public:

        Image() = default;

        Image(int width, int height);

        explicit Image(const std::string& fileName);

        int GetWidth() const
        {
            return width;
        }

        int GetHeight() const
        {
            return height;
        }

        const std::vector<CS200::RGBA>& GetPixels() const
        {
            return pixels;
        }

        std::vector<CS200::RGBA>& GetPixels()
        {
            return pixels;
        }

        bool IsValid() const
        {
            return width > 0 && height > 0 && !pixels.empty();
        }

        CS200::RGBA GetPixel(int x, int y) const;

        void SetPixel(int x, int y, CS200::RGBA color);

        int                      width  = 0;
        int                      height = 0;
        std::vector<CS200::RGBA> pixels;

    private:

        // PerlinNoise
        friend Image        GenImagePerlinNoise(int width, int height, int offsetX, int offsetY, float scale);
        friend Image        GenImageNoise(int width, int height, int offsetX, int offsetY, float scale, NoisePattern pattern);
        friend CS200::RGBA* LoadImageColors(const Image& image);
        friend CS200::RGBA  GetImageColor(const Image& image, int x, int y);
        friend void         ImageVerticalFlip(Image& image);
    };

    // ===== Image Generation Functions =====

    Image GenImagePerlinNoise(int width, int height, int offsetX, int offsetY, float scale);

    Image GenImageNoise(int width, int height, int offsetX, int offsetY, float scale, NoisePattern pattern);

    Image GenImageColor(int width, int height, CS200::RGBA color);

    Image LoadImage(const std::string& fileName);

    void UnloadImage(Image& image);

    bool ExportImage(const Image& image, const std::string& fileName);


    // ===== Image Pixel Access Functions =====

    CS200::RGBA* LoadImageColors(const Image& image);

    void UnloadImageColors(CS200::RGBA* colors);

    CS200::RGBA GetImageColor(const Image& image, int x, int y);


    // ===== Image Manipulation Functions =====

    void ImageVerticalFlip(Image& image);

    void ImageHorizontalFlip(Image& image);

    void ImageRotateCW(Image& image);

    void ImageRotateCCW(Image& image);

    void ImageColorTint(Image& image, CS200::RGBA color);

}
