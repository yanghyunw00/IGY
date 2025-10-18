#pragma once
#include <glad/glad.h>

struct GLFBO {
    GLuint fbo=0, color=0, rbo=0; int w=0,h=0;
};

bool glfbo_create(GLFBO& rt, int w, int h);
void glfbo_begin(const GLFBO& rt);
void glfbo_end();
void glfbo_destroy(GLFBO& rt);
