#include "gl_fbo.hpp"

bool glfbo_create(GLFBO& rt, int w, int h){
    rt.w=w; rt.h=h;
    glGenTextures(1,&rt.color); glBindTexture(GL_TEXTURE_2D,rt.color);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,nullptr);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glGenRenderbuffers(1,&rt.rbo); glBindRenderbuffer(GL_RENDERBUFFER,rt.rbo);
    glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH24_STENCIL8,w,h);
    glGenFramebuffers(1,&rt.fbo); glBindFramebuffer(GL_FRAMEBUFFER,rt.fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt.color, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rt.rbo);
    bool ok = (glCheckFramebufferStatus(GL_FRAMEBUFFER)==GL_FRAMEBUFFER_COMPLETE);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    return ok;
}

void glfbo_begin(const GLFBO& rt){
    glBindFramebuffer(GL_FRAMEBUFFER, rt.fbo);
    glViewport(0,0,rt.w,rt.h);
}

void glfbo_end(){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void glfbo_destroy(GLFBO& rt){
    if(rt.fbo) glDeleteFramebuffers(1,&rt.fbo), rt.fbo=0;
    if(rt.rbo) glDeleteRenderbuffers(1,&rt.rbo), rt.rbo=0;
    if(rt.color) glDeleteTextures(1,&rt.color), rt.color=0;
}
