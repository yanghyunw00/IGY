/**
 * \file
 * \author Rudy Castan
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "Image.hpp"
#include "Engine/Error.hpp"
#include "Engine/Path.hpp"
#include "OpenGL/GL.hpp"
#include <stb_image.h>

namespace CS200
{

    Image::Image(const std::filesystem::path& image_path, bool flip_vertical)
    {
        stbi_set_flip_vertically_on_load(flip_vertical);
        const std::filesystem::path image_ = assets::locate_asset(image_path);
        data_                              = stbi_load(image_.string().c_str(), &width, &height, &file_num_channels, num_channels);

        if (!data_)
        {
            throw std::runtime_error("Failed to load image");
        }
    }

    Image::Image(Image&& temporary) noexcept
    {
        data_             = temporary.data_;
        width             = temporary.width;
        height            = temporary.height;
        file_num_channels = temporary.file_num_channels;
        temporary.data_   = nullptr;
        temporary.width   = 0;
        temporary.height  = 0;
    }

    Image& Image::operator=(Image&& temporary) noexcept
    {
        if (this != &temporary)
        {
            std::swap(data_, temporary.data_);
            std::swap(width, temporary.width);
            std::swap(height, temporary.height);
            std::swap(file_num_channels, temporary.file_num_channels);
        }
        return *this;
    }

    Image::~Image()
    {
        if (data_ != nullptr)
        {
            stbi_image_free(data_);
        }
    }

    const RGBA* Image::data() const noexcept
    {
        return reinterpret_cast<const RGBA*>(data_);
    }

    RGBA* Image::data() noexcept
    {
        return reinterpret_cast<RGBA*>(data_);
    }

    Math::ivec2 Image::GetSize() const noexcept
    {
        return { width, height };
    }

}
