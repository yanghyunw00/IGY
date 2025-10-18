#pragma once
#include <cstdint>
#include <vector>

struct Color {
    uint8_t r=0,g=0,b=0,a=255;
};

struct Image {
    int width=0, height=0, channels=4;
    std::vector<uint8_t> pixels; // RGBA
};

bool image_load_rgba(const char* path, Image& out);  // file -> Image (RGBA8)
Color image_get_pixel(const Image& img, int x, int y);
bool image_save_png(const char* path, const Image& img);
