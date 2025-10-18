#include "gl_platform.hpp"
#include <cstdio>

static void on_error(int code, const char* desc){ std::fprintf(stderr,"GLFW %d: %s\n",code,desc); }
static void on_resize(GLFWwindow*,int w,int h){ glViewport(0,0,w,h); }

bool glapp_init(GLApp& app, int w, int h, const char* title){
    glfwSetErrorCallback(on_error);
    if(!glfwInit()) return false;

#if defined(__APPLE__)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
#endif

    app.window = glfwCreateWindow(w,h,title,nullptr,nullptr);
    if(!app.window){ glfwTerminate(); return false; }
    app.width=w; app.height=h;

    glfwMakeContextCurrent(app.window);
    if(!gladLoadGL()){
        std::fprintf(stderr,"[glad] failed to load OpenGL functions.\n");
        return false;
    }
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(app.window,on_resize);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    return true;
}

void glapp_begin_frame(){
    glClearColor(0.1f,0.12f,0.15f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void glapp_end_frame(GLApp& app){
    glfwSwapBuffers(app.window);
    glfwPollEvents();
}

void glapp_shutdown(GLApp& app){
    if(app.window){ glfwDestroyWindow(app.window); app.window=nullptr; }
    glfwTerminate();
}
