/**
 * \file DKImage.hpp
 * \brief image manipulation functions
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

/*
Image(): Default constructor for Image class.

Image(int width, int height): Create image with given width and height.

Image(const std::string& fileName): Load image from file.

GetWidth(): Get image width.

GetHeight(): Get image height.

GetPixels(): Get pixel colors (const and non-const versions).

IsValid(): Check if image is valid (has size and pixels).

GetPixel(int x, int y): Get pixel color at position.

SetPixel(int x, int y, CS200::RGBA color): Set pixel color at position.

GenImagePerlinNoise(int width, int height, int offsetX, int offsetY, float scale): Generate Perlin noise image.

GenImageColor(int width, int height, CS200::RGBA color): Generate solid color image.

LoadImage(const std::string& fileName): Load image from file.

UnloadImage(Image& image): Free image resources.

ExportImage(const Image& image, const std::string& fileName): Save image to file.

LoadImageColors(const Image& image): Get pointer to image pixel colors.

UnloadImageColors(CS200::RGBA* colors): Free pixel color pointer.

GetImageColor(const Image& image, int x, int y): Get pixel color at position.

ImageVerticalFlip(Image& image): Flip image vertically.

ImageHorizontalFlip(Image& image): Flip image horizontally.

ImageRotateCW(Image& image): Rotate image clockwise.

ImageRotateCCW(Image& image): Rotate image counter-clockwise.

ImageColorTint(Image& image, CS200::RGBA color): Apply color tint to image.
*/

#pragma once

#include "CS200/RGBA.hpp"
#include <string>
#include <vector>

namespace Dokev
{

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
        friend CS200::RGBA* LoadImageColors(const Image& image);
        friend CS200::RGBA  GetImageColor(const Image& image, int x, int y);
        friend void         ImageVerticalFlip(Image& image);
    };

    // ===== Image Generation Functions =====

    Image GenImagePerlinNoise(int width, int height, int offsetX, int offsetY, float scale);

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
