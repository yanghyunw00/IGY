#pragma once
#include <glad/glad.h>
#include "gl_image.h"

struct GLTexture {
    GLuint id=0; int width=0, height=0;
};

bool gltex_from_image(const Image& img, GLTexture& out);
bool gltex_from_file(const char* path, GLTexture& out);
void gltex_destroy(GLTexture& t);
