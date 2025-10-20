/**
 * \file gl_texture.cpp
 * \brief 
 * \author Hyunseok Choi 
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "gl_texture.hpp"

bool gltex_from_image(const Image& img, GLTexture& out){
    if(out.id==0) glGenTextures(1,&out.id);
    out.width=img.width; out.height=img.height;
    glBindTexture(GL_TEXTURE_2D, out.id);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,img.width,img.height,0,GL_RGBA,GL_UNSIGNED_BYTE,img.pixels.data());
    return true;
}

bool gltex_from_file(const char* path, GLTexture& out){
    Image img; if(!image_load_rgba(path,img)) return false;
    return gltex_from_image(img,out);
}

void gltex_destroy(GLTexture& t){
    if(t.id){ glDeleteTextures(1,&t.id); t.id=0; }
}
