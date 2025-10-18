#include "Framebuffer.hpp"
/**
 * \file
 * \author Rudy Castan
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "Engine/Engine.hpp"
#include "Engine/Logger.hpp"
#include "Framebuffer.hpp"
#include "GL.hpp"

namespace
{
    void verify_framebuffer_complete(OpenGL::FramebufferHandle framebuffer);
}

namespace OpenGL
{
   FramebufferWithColor CreateFramebufferWithColor(Math::ivec2 size)
    {
        FramebufferWithColor fb{};
        fb.size_ = size;
        GL::GenFramebuffers(1, &fb.Framebuffer);
        GL::BindFramebuffer(GL_FRAMEBUFFER, fb.Framebuffer);

        GL::GenTextures(1, &fb.ColorAttachment);
        GL::BindTexture(GL_TEXTURE_2D, fb.ColorAttachment);

        GL::TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        fb.ColorAttachment = CreateRGBATexture(size);

        GL::FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb.ColorAttachment, 0);
        constexpr GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0 };
        GL::DrawBuffers(1, draw_buffers);

        verify_framebuffer_complete(fb.Framebuffer);
        GL::BindFramebuffer(GL_FRAMEBUFFER, 0);
        return fb;
    }

    void DestroyFramebufferWithColor(FramebufferWithColor& framebuffer_with_color) noexcept
    {
        GL::DeleteFramebuffers(1, &framebuffer_with_color.Framebuffer);
        GL::DeleteTextures(1, &framebuffer_with_color.ColorAttachment);
        framebuffer_with_color.ColorAttachment = 0;
        framebuffer_with_color.Framebuffer     = 0;
    }

    void Bind(const FramebufferWithColor& framebuffer_with_color)
    {
        GL::BindFramebuffer(GL_FRAMEBUFFER, framebuffer_with_color.Framebuffer);
    }

    void Unbind()
    {
        GL::BindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

namespace
{
    void verify_framebuffer_complete(OpenGL::FramebufferHandle framebuffer)
    {
        GL::BindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        const auto status_result = GL::CheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status_result == GL_FRAMEBUFFER_COMPLETE)
        {
            GL::BindFramebuffer(GL_FRAMEBUFFER, 0);
            return; 
        }

        std::ostringstream message;
        message << "Framebuffer creation failed: status = 0x" << std::hex << status_result << std::dec << "\n";

        switch (status_result)
        {
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                message << "\t[Attachment Error] One or more framebuffer attachments (e.g. textures or renderbuffers) are incomplete or incompatible.\n"
                        << "\t-> Double-check format consistency and ensure all attached resources are properly initialized.\n";
                break;

            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                message << "\t[Missing Attachment] No color, depth, or stencil attachment was provided.\n"
                        << "\t-> Make sure at least one valid image is attached using glFramebufferTexture or glFramebufferRenderbuffer.\n";
                break;

            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                message << "\t[Draw Buffer Error] Draw buffers are referencing attachments that don't exist or are incomplete.\n"
                        << "\t-> Confirm that GL_COLOR_ATTACHMENTx exists for each buffer in gl_Drawbuffer().\n";
                break;

            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                message << "\t[Read Buffer Error] Read buffer (glReadBuffer) is referencing an incomplete or missing attachment.\n"
                        << "\t-> Check if glReadBuffer is targeting an attachment that's properly set up.\n";
                break;

            case GL_FRAMEBUFFER_UNSUPPORTED:
                message << "\t[Unsupported Configuration] This framebuffer configuration is not supported by the OpenGL implementation.\n"
                        << "\t-> Try simplifying formats, avoid non-standard combinations, and confirm hardware support.\n";
                break;

            default:
                message << "\t[Unknown Error] Framebuffer status not recognized. Raw code: " << status_result << "\n"
                        << "\t-> Investigate texture formats and attachment completeness. Enable OpenGL debug output for deeper insights.\n";
                break;
        }

        Engine::GetLogger().LogError(message.str());
        GL::BindFramebuffer(GL_FRAMEBUFFER, 0);
        throw std::runtime_error{ message.str() };
    }
}
