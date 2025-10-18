/**
 * \file DKImage.cpp
 * \brief implementation image functions
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "DKImage.hpp"
#include "CS200/Image.hpp"
#include "DKMath.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>

#define STB_PERLIN_IMPLEMENTATION
#include <stb_perlin.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace Dokev
{

    //==========Image class===============================
    Image::Image(int width, int height)
        : width(width), height(height)
    {
        pixels.resize(width * height, 0xFFFFFFFF); // White by default
    }

    Image::Image(const std::string& fileName)
    {
        auto engineImage = CS200::Image(fileName);
        width = engineImage.GetSize().x;
        height = engineImage.GetSize().y;

        // Convert BGRA to RGBA
        pixels.resize(width * height);
        const CS200::RGBA* imageData = engineImage.data();
        for (int i = 0; i < width * height; ++i)
        {
            pixels[i] = CS200::rgba_to_abgr(imageData[i]);
        }
    }

    CS200::RGBA Image::GetPixel(int x, int y) const
    {
        if (x < 0 || x >= width || y < 0 || y >= height)
            return 0x00000000;
        return pixels[y * width + x];
    }

    void Image::SetPixel(int x, int y, CS200::RGBA color)
    {
        if (x < 0 || x >= width || y < 0 || y >= height)
            return;
        pixels[y * width + x] = color;
    }

    //=================image generation functions===================

    Image GenImagePerlinNoise(int width, int height, int offsetX, int offsetY, float scale)
    {
        Image img(width, height);

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                float nx = (x + offsetX) * scale;
                float ny = (y + offsetY) * scale;

                float noiseValue = Dokev::DKMath::PerlinNoise(nx, ny, 0.0f, 0, 0, 0);

                noiseValue = (noiseValue + 1.0f) * 0.5f;

                noiseValue = std::max(0.0f, std::min(1.0f, noiseValue));

                uint8_t gray = static_cast<uint8_t>(noiseValue * 255.0f);
                CS200::RGBA color = (gray << 24) | (gray << 16) | (gray << 8) | 0xFF;

                img.SetPixel(x, y, color);
            }
        }

        return img;
    }

    Image GenImageColor(int width, int height, CS200::RGBA color)
    {
        Image img(width, height);
        std::fill(img.pixels.begin(), img.pixels.end(), color);
        return img;
    }


    // ===== Image Loading/Unloading =====

    Image LoadImage(const std::string& fileName)
    {
        return Image(fileName);
    }

    void UnloadImage(Image& image)
    {
        image.pixels.clear();
        image.width = 0;
        image.height = 0;
    }

    bool ExportImage(const Image& image, const std::string& fileName)
    {
        if (!image.IsValid())
            return false;

        std::vector<unsigned char> rgba_data(image.width * image.height * 4);

        for (int i = 0; i < image.width * image.height; ++i)
        {
            CS200::RGBA pixel = image.pixels[i];
            rgba_data[i * 4 + 0] = (pixel >> 24) & 0xFF; // R
            rgba_data[i * 4 + 1] = (pixel >> 16) & 0xFF; // G
            rgba_data[i * 4 + 2] = (pixel >> 8) & 0xFF;  // B
            rgba_data[i * 4 + 3] = pixel & 0xFF;         // A
        }

        int result = stbi_write_png(fileName.c_str(), image.width, image.height, 4, rgba_data.data(), image.width * 4);

        return result != 0;
    }

    // ===== Pixel Access Functions =====

    CS200::RGBA* LoadImageColors(const Image& image)
    {
        if (!image.IsValid())
            return nullptr;

        CS200::RGBA* colors = new CS200::RGBA[image.width * image.height];
        std::memcpy(colors, image.pixels.data(), image.pixels.size() * sizeof(CS200::RGBA));
        return colors;
    }

    void UnloadImageColors(CS200::RGBA* colors)
    {
        delete[] colors;
    }

    CS200::RGBA GetImageColor(const Image& image, int x, int y)
    {
        return image.GetPixel(x, y);
    }

    // ===== Image Manipulation =====

    void ImageVerticalFlip(Image& image)
    {
        for (int y = 0; y < image.height / 2; y++)
        {
            for (int x = 0; x < image.width; x++)
            {
                int topIndex = y * image.width + x;
                int bottomIndex = (image.height - 1 - y) * image.width + x;
                std::swap(image.pixels[topIndex], image.pixels[bottomIndex]);
            }
        }
    }

    void ImageHorizontalFlip(Image& image)
    {
        for (int y = 0; y < image.height; y++)
        {
            for (int x = 0; x < image.width / 2; x++)
            {
                int leftIndex = y * image.width + x;
                int rightIndex = y * image.width + (image.width - 1 - x);
                std::swap(image.pixels[leftIndex], image.pixels[rightIndex]);
            }
        }
    }

    void ImageRotateCW(Image& image)
    {
        std::vector<CS200::RGBA> newPixels(image.width * image.height);
        int newWidth = image.height;
        int newHeight = image.width;

        for (int y = 0; y < image.height; y++)
        {
            for (int x = 0; x < image.width; x++)
            {
                int newX = image.height - 1 - y;
                int newY = x;
                newPixels[newY * newWidth + newX] = image.pixels[y * image.width + x];
            }
        }

        image.pixels = std::move(newPixels);
        image.width = newWidth;
        image.height = newHeight;
    }

    void ImageRotateCCW(Image& image)
    {
        std::vector<CS200::RGBA> newPixels(image.width * image.height);
        int newWidth = image.height;
        int newHeight = image.width;

        for (int y = 0; y < image.height; y++)
        {
            for (int x = 0; x < image.width; x++)
            {
                int newX = y;
                int newY = image.width - 1 - x;
                newPixels[newY * newWidth + newX] = image.pixels[y * image.width + x];
            }
        }

        image.pixels = std::move(newPixels);
        image.width = newWidth;
        image.height = newHeight;
    }

    void ImageColorTint(Image& image, CS200::RGBA tint)
    {
        float r = ((tint >> 24) & 0xFF) / 255.0f;
        float g = ((tint >> 16) & 0xFF) / 255.0f;
        float b = ((tint >> 8) & 0xFF) / 255.0f;
        float a = (tint & 0xFF) / 255.0f;

        for (auto& pixel : image.pixels)
        {
            float pr = ((pixel >> 24) & 0xFF) / 255.0f;
            float pg = ((pixel >> 16) & 0xFF) / 255.0f;
            float pb = ((pixel >> 8) & 0xFF) / 255.0f;
            float pa = (pixel & 0xFF) / 255.0f;

            pixel = (static_cast<uint8_t>(pr * r * 255) << 24) |
                    (static_cast<uint8_t>(pg * g * 255) << 16) |
                    (static_cast<uint8_t>(pb * b * 255) << 8) |
                    static_cast<uint8_t>(pa * a * 255);
        }
    }

}
