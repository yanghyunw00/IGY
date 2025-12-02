/**
 * \file
 * \author Rudy Castan
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */


#include "OffscreenFramebuffer.hpp"
#include <algorithm>
#include <iostream>

#include "OpenGL/GL.hpp"

namespace
{
    int ValidateMSAASamples(int samples)
    {
        GLint max_samples = 0;
        glGetIntegerv(GL_MAX_SAMPLES, &max_samples);

        samples = std::max(2, std::min(samples, max_samples));

        samples = samples & ~1;

        return samples;
    }
}

OffscreenFramebuffer::~OffscreenFramebuffer()
{
    Shutdown();
}

void OffscreenFramebuffer::Initialize(int width, int height, [[maybe_unused]] MSAA use_msaa, [[maybe_unused]] int msaa_samples)
{
    currentWidth  = width;
    currentHeight = height;
    // TODO
    useMSAA       = (use_msaa == MSAA::True);
    msaaSamples   = ValidateMSAASamples(msaa_samples);

    createResolveFramebuffer();
    createMSAAFramebuffer();
}

void OffscreenFramebuffer::BindForRendering()
{
    // TODO 
    GLuint target = useMSAA ? msaaFramebuffer : resolveFramebuffer;
    GL::BindFramebuffer(GL_FRAMEBUFFER, target);
}

GLuint OffscreenFramebuffer::GetTexture()
{
    resolveMSAA();
    return resolveTexture;
}

void OffscreenFramebuffer::Resize(int width, int height)
{
    currentWidth  = width;
    currentHeight = height;

    createResolveFramebuffer();
    createMSAAFramebuffer();
}

void OffscreenFramebuffer::SetMSAA([[maybe_unused]] MSAA use_msaa, [[maybe_unused]] int msaa_samples)
{
    // TODO
    useMSAA       = (use_msaa == MSAA::True) ? true : false;
    msaaSamples = ValidateMSAASamples(msaa_samples);

    if (currentWidth > 0 && currentHeight > 0)
    {
        createMSAAFramebuffer();
    }
}

void OffscreenFramebuffer::Shutdown()
{
    if (resolveFramebuffer != 0)
    {
        GL::DeleteFramebuffers(1, &resolveFramebuffer);
        resolveFramebuffer = 0;
    }
    if (resolveTexture != 0)
    {
        glDeleteTextures(1, &resolveTexture);
        resolveTexture = 0;
    }
    // TODO
    if (msaaFramebuffer != 0)
    {
        GL::DeleteFramebuffers(1, &msaaFramebuffer);
        msaaFramebuffer = 0;
    }
    if (msaaColorRenderbuffer != 0)
    {
        GL::DeleteRenderbuffers(1, &msaaColorRenderbuffer);
        msaaColorRenderbuffer = 0;
    }
}

void OffscreenFramebuffer::createResolveFramebuffer()
{
    if (resolveFramebuffer == 0)
    {
        GL::GenFramebuffers(1, &resolveFramebuffer);
    }

    if (resolveTexture != 0)
    {
        GL::DeleteTextures(1, &resolveTexture);
        resolveTexture = 0;
    }

    GL::GenTextures(1, &resolveTexture);
    GL::BindTexture(GL_TEXTURE_2D, resolveTexture);
    GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

#ifdef IS_WEBGL2
    GL:GL:::TexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, currentWidth, currentHeight);
#else
    GL::TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, currentWidth, currentHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
#endif
    GL::BindTexture(GL_TEXTURE_2D, 0);

    GL::BindFramebuffer(GL_FRAMEBUFFER, resolveFramebuffer);
    GL::FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, resolveTexture, 0);

    auto status = GL::CheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Failed to create resolve framebuffer\n";
        std::exit(-1);
    }
    GL::BindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OffscreenFramebuffer::createMSAAFramebuffer()
{
    // TODO
    if (!useMSAA)
        return;

    if (msaaFramebuffer == 0)
    {
        GL::GenFramebuffers(1, &msaaFramebuffer);
    }

    if (msaaColorRenderbuffer != 0)
    {
        GL::DeleteRenderbuffers(1, &msaaColorRenderbuffer);
        msaaColorRenderbuffer = 0;
    }

    GL::GenRenderbuffers(1, &msaaColorRenderbuffer);
    GL::BindRenderbuffer(GL_RENDERBUFFER, msaaColorRenderbuffer);
    GL::RenderbufferStorageMultisample(GL_RENDERBUFFER, msaaSamples, GL_RGBA8, currentWidth, currentHeight);
    GL::BindRenderbuffer(GL_RENDERBUFFER, 0);

    GL::BindFramebuffer(GL_FRAMEBUFFER, msaaFramebuffer);
    GL::FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, msaaColorRenderbuffer);

    auto status = GL::CheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Failed to create MSAA framebuffer\n";
        std::exit(-1);
    }
    GL::BindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OffscreenFramebuffer::resolveMSAA()
{
    // TODO
    if (!useMSAA)
        return;

    GL::BindFramebuffer(GL_READ_FRAMEBUFFER, msaaFramebuffer);
    GL::BindFramebuffer(GL_DRAW_FRAMEBUFFER, resolveFramebuffer);
    glBlitFramebuffer(0, 0, currentWidth, currentHeight, 0, 0, currentWidth, currentHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    GL::BindFramebuffer(GL_FRAMEBUFFER, 0);
}
