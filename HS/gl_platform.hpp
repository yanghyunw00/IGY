#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct GLApp {
    GLFWwindow* window{};
    int width{}, height{};
};

bool glapp_init(GLApp& app, int w, int h, const char* title);
void glapp_begin_frame();
void glapp_end_frame(GLApp& app);
void glapp_shutdown(GLApp& app);
