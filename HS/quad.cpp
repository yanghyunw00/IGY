#pragma once
#include <glad/glad.h>

struct Quad {
    GLuint vao=0, vbo=0, ebo=0, prog=0; 
    GLint u_tex=-1, u_model=-1, u_viewproj=-1;
};
#include "quad.hpp"
#include <cmath>

static const char* VS = R"(#version 330 core
layout(location=0) in vec2 aPos; layout(location=1) in vec2 aUV;
uniform mat4 u_model; uniform mat4 u_viewproj;
out vec2 vUV;
void main(){ vUV=aUV; gl_Position = u_viewproj * u_model * vec4(aPos,0,1); }
)";
static const char* FS = R"(#version 330 core
in vec2 vUV; uniform sampler2D u_tex; out vec4 FragColor;
void main(){ FragColor = texture(u_tex, vUV); }
)";

static GLuint mkshader(GLenum t, const char* s){
    GLuint sh = glCreateShader(t); glShaderSource(sh,1,&s,nullptr); glCompileShader(sh);
    GLint ok; glGetShaderiv(sh,GL_COMPILE_STATUS,&ok);
    if(!ok){ char log[1024]; glGetShaderInfoLog(sh,1024,nullptr,log); }
    return sh;
}
static GLuint mkprog(const char* vs, const char* fs){
    GLuint v=mkshader(GL_VERTEX_SHADER,vs), f=mkshader(GL_FRAGMENT_SHADER,fs), p=glCreateProgram();
    glAttachShader(p,v); glAttachShader(p,f); glLinkProgram(p);
    glDeleteShader(v); glDeleteShader(f); return p;
}
static void make_ortho(float l,float r,float b,float t,float* m){
    for(int i=0;i<16;++i) m[i]=0;
    m[0]=2.0f/(r-l); m[5]=2.0f/(t-b); m[10]=-1; m[15]=1;
    m[12]=-(r+l)/(r-l); m[13]=-(t+b)/(t-b);
}

bool quad_init(Quad& q){
    q.prog = mkprog(VS,FS);
    float v[] = { // x,y, u,v
        0,0, 0,0,
        1,0, 1,0,
        1,1, 1,1,
        0,1, 0,1
    };
    GLuint e[] = {0,1,2, 0,2,3};
    glGenVertexArrays(1,&q.vao); glBindVertexArray(q.vao);
    glGenBuffers(1,&q.vbo); glBindBuffer(GL_ARRAY_BUFFER,q.vbo);
    glBufferData(GL_ARRAY_BUFFER,sizeof(v),v,GL_STATIC_DRAW);
    glGenBuffers(1,&q.ebo); glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, q.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(e), e, GL_STATIC_DRAW);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);
    q.u_tex = glGetUniformLocation(q.prog,"u_tex");
    q.u_model = glGetUniformLocation(q.prog,"u_model");
    q.u_viewproj = glGetUniformLocation(q.prog,"u_viewproj");
    glBindVertexArray(0);
    return true;
}

void quad_draw(const Quad& q, GLuint tex, float x, float y, float w, float h) {
    GLint vpArr[4]; glGetIntegerv(GL_VIEWPORT, vpArr);
    float vpW = float(vpArr[2]), vpH = float(vpArr[3]);

    float vp[16]; // make_ortho(left, right, bottom, top, out)
    make_ortho(0, vpW, vpH, 0, vp);

    float M[16] = { w,0,0,0,  0,h,0,0,  0,0,1,0,  x,y,0,1 };

    glUseProgram(q.prog);
    glUniform1i(q.u_tex, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glUniformMatrix4fv(q.u_model, 1, GL_FALSE, M);
    glUniformMatrix4fv(q.u_viewproj, 1, GL_FALSE, vp);
    glBindVertexArray(q.vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void quad_shutdown(Quad& q){
    if(q.vbo){ glDeleteBuffers(1,&q.vbo); q.vbo=0; }
    if(q.ebo){ glDeleteBuffers(1,&q.ebo); q.ebo=0; }
    if(q.vao){ glDeleteVertexArrays(1,&q.vao); q.vao=0; }
    if(q.prog){ glDeleteProgram(q.prog); q.prog=0; }
}

bool quad_init(Quad& q);
void quad_draw(const Quad& q, GLuint tex, float x, float y, float w, float h);
void quad_shutdown(Quad& q);
