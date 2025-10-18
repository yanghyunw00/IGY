/**
 * \file
 * \author Rudy Castan
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "Buffer.hpp"
#include "GL.hpp"

namespace OpenGL
{
    BufferHandle CreateBuffer(BufferType type, GLsizeiptr size_in_bytes) noexcept
    {
        GLenum bufferType    = static_cast<GLenum>(type);
        GLuint buffer_handle = 0;
        GL::GenBuffers(1, &buffer_handle);
        GL::BindBuffer(bufferType, buffer_handle);
        GL::BufferData(bufferType, size_in_bytes, nullptr, GL_DYNAMIC_DRAW);

        GLint size = 0;
        GL::GetBufferParameteriv(bufferType, GL_BUFFER_SIZE, &size);
        if (size_in_bytes != size)
        {
            GL::DeleteBuffers(1, &buffer_handle);
            return 0;
        }
        GL::BindBuffer(bufferType, 0);
        return buffer_handle;
    }

    BufferHandle CreateBuffer(BufferType type, std::span<const std::byte> static_buffer_data) noexcept
    {
        GLenum     bufferType    = static_cast<GLenum>(type);
        GLsizeiptr size_bytes    = static_cast<GLsizeiptr>(static_buffer_data.size_bytes());
        GLuint     buffer_handle = 0;
        GL::GenBuffers(1, &buffer_handle);
        GL::BindBuffer(bufferType, buffer_handle);
        GL::BufferData(bufferType, size_bytes, static_buffer_data.data(), GL_STATIC_DRAW);

        GLint size = 0;
        GL::GetBufferParameteriv(bufferType, GL_BUFFER_SIZE, &size);
        if (size_bytes != size)
        {
            GL::DeleteBuffers(1, &buffer_handle);
            return 0;
        }
        GL::BindBuffer(bufferType, 0);
        return buffer_handle;
    }

    void UpdateBufferData(BufferType type, BufferHandle buffer, std::span<const std::byte> data_to_copy, GLsizei starting_offset) noexcept
    {
        GLenum bufferType = static_cast<GLenum>(type);
        GL::BindBuffer(bufferType, buffer);

        GL::BufferSubData(bufferType, starting_offset, static_cast<GLsizeiptr>(data_to_copy.size_bytes()), data_to_copy.data());
        GL::BindBuffer(bufferType, 0);
    }
}
