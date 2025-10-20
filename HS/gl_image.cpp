/**
 * \file gl_image.cpp
 * \brief 
 * \author Hyunseok Choi 
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "external/stb_image_write.h"
#include "gl_image.hpp"
#include <cstring>

bool image_load_rgba(const char* path, Image& out){
    int w,h,c;
    unsigned char* data = stbi_load(path,&w,&h,&c,4);
    if(!data) return false;
    out.width=w; out.height=h; out.channels=4;
    out.pixels.assign(data, data + (size_t)w*h*4);
    stbi_image_free(data);
    return true;
}

Color image_get_pixel(const Image& img, int x, int y){
    if(x<0||y<0||x>=img.width||y>=img.height) return Color{};
    size_t i = (size_t(y)*img.width + x)*4;
    return Color{ img.pixels[i+0], img.pixels[i+1], img.pixels[i+2], img.pixels[i+3] };
}

bool image_save_png(const char* path, const Image& img){
    if(img.width<=0 || img.height<=0 || img.pixels.empty()) return false;
    return stbi_write_png(path, img.width, img.height, 4, img.pixels.data(), img.width*4) != 0;
}
