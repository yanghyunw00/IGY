#pragma once
#include <glad/glad.h>

struct Quad {
    GLuint vao=0, vbo=0, ebo=0, prog=0; 
    GLint u_tex=-1, u_model=-1, u_viewproj=-1;
};

bool quad_init(Quad& q);
void quad_draw(const Quad& q, GLuint tex, float x, float y, float w, float h);
void quad_shutdown(Quad& q);
