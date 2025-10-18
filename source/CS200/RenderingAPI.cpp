/**
 * \file
 * \author Rudy Castan
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "RenderingAPI.hpp"

#include "../OpenGL/GL.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Error.hpp"
#include "Engine/Logger.hpp"
#include "OpenGL/Environment.hpp"
#include <GL/glew.h>
#include <cassert>

namespace
{
#if defined(DEVELOPER_VERSION) && not defined(IS_WEBGL2)
    void OpenGLMessageCallback(
        [[maybe_unused]] unsigned source, [[maybe_unused]] unsigned type, [[maybe_unused]] unsigned id, unsigned severity, [[maybe_unused]] int length, const char* message,
        [[maybe_unused]] const void* userParam)
    {
        switch (severity)
        {
            case GL_DEBUG_SEVERITY_HIGH: Engine::GetLogger().LogError(message); return;
            case GL_DEBUG_SEVERITY_MEDIUM: Engine::GetLogger().LogError(message); return;
            case GL_DEBUG_SEVERITY_LOW: Engine::GetLogger().LogVerbose(message); return;
            case GL_DEBUG_SEVERITY_NOTIFICATION: Engine::GetLogger().LogVerbose(message); return;
        }

        assert(false && "Unknown severity level!");
    }
#endif
}

namespace CS200::RenderingAPI
{
    void Init() noexcept
    {
        GLint major = 0, minor = 0;
        GL::GetIntegerv(GL_MAJOR_VERSION, &major);
        GL::GetIntegerv(GL_MINOR_VERSION, &minor);
        if (OpenGL::version(major, minor) < OpenGL::version(OpenGL::MinimumRequiredMajorVersion, OpenGL::MinimumRequiredMinorVersion))
            throw_error_message("Unsupported OpenGL version ", major, '.', minor, "\n We need OpenGL ", OpenGL::MinimumRequiredMajorVersion, '.', OpenGL::MinimumRequiredMinorVersion, " or higher");

        if (OpenGL::MajorVersion == 0)
        {
            OpenGL::MajorVersion = major;
            OpenGL::MinorVersion = minor;
        }

        GL::GetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &OpenGL::MaxTextureImageUnits);
        GL::GetIntegerv(GL_MAX_TEXTURE_SIZE, &OpenGL::MaxTextureSize);

#if defined(DEVELOPER_VERSION) && not defined(IS_WEBGL2)
        if (OpenGL::current_version() >= OpenGL::version(4, 3))
        {
            GL::Enable(GL_DEBUG_OUTPUT);
            GL::Enable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            GL::DebugMessageCallback(OpenGLMessageCallback, nullptr);
            GL::DebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
        }
#endif

        GL::Enable(GL_BLEND);
        GL::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        GL::Disable(GL_DEPTH_TEST);

        Engine::GetLogger().LogEvent("------------------------Graphics Card and Setting------------------------");

        const char* vendor               = reinterpret_cast<const char*>(GL::GetString(GL_VENDOR));
        const char* renderer             = reinterpret_cast<const char*>(GL::GetString(GL_RENDERER));
        const char* version              = reinterpret_cast<const char*>(GL::GetString(GL_VERSION));
        const char* shading_lang_version = reinterpret_cast<const char*>(GL::GetString(GL_SHADING_LANGUAGE_VERSION));

        Engine::GetLogger().LogEvent("Vendor: " + std::string(vendor));
        Engine::GetLogger().LogEvent("Renderer: " + std::string(renderer));
        Engine::GetLogger().LogEvent("Version: " + std::string(version));
        Engine::GetLogger().LogEvent("GLSL Version: " + std::string(shading_lang_version));

        GLint majorVersion = 0;
        GL::GetIntegerv(GL_MAJOR_VERSION, &majorVersion);
        Engine::GetLogger().LogEvent("Major Version: " + std::to_string(majorVersion));

        GLint minorVersion = 0;
        GL::GetIntegerv(GL_MINOR_VERSION, &minor);
        Engine::GetLogger().LogEvent("Minor Version: " + std::to_string(minorVersion));

        GLint vertices = 0;
        GL::GetIntegerv(GL_MAX_ELEMENTS_VERTICES, &vertices);
        Engine::GetLogger().LogEvent("Max Elements Vertices: " + std::to_string(vertices));

        GLint indices = 0;
        GL::GetIntegerv(GL_MAX_ELEMENTS_INDICES, &indices);
        Engine::GetLogger().LogEvent("Max Elements Indices: " + std::to_string(indices));

        GLint image_units = 0;
        GL::GetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &image_units);
        Engine::GetLogger().LogEvent("Max Texture Image Units: " + std::to_string(image_units));

        GLint texture_size = 0;
        GL::GetIntegerv(GL_MAX_TEXTURE_SIZE, &texture_size);
        Engine::GetLogger().LogEvent("Max Texture Size: " + std::to_string(texture_size) + " x " + std::to_string(texture_size));

        GLint dims[2] = { 0, 0 };
        GL::GetIntegerv(GL_MAX_VIEWPORT_DIMS, dims);
        Engine::GetLogger().LogEvent("Max Viewport Dimensions: " + std::to_string(dims[0]) + " x " + std::to_string(dims[1]));
    }

    void SetClearColor(CS200::RGBA color) noexcept
    {
        const auto rgba = CS200::unpack_color(color);
        GL::ClearColor(rgba[0], rgba[1], rgba[2], rgba[3]);
    }

    void Clear() noexcept
    {
        GL::Clear(GL_COLOR_BUFFER_BIT);
    }

    void SetViewport(Math::ivec2 size, Math::ivec2 anchor_left_bottom) noexcept
    {
        GL::Viewport(anchor_left_bottom.x, anchor_left_bottom.y, size.x, size.y);
    }
}
