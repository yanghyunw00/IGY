/**
 * \file
 * \author Rudy Castan
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "VertexArray.hpp"
#include "GL.hpp"

namespace OpenGL
{
    /**
     * \brief Creates and configures a Vertex Array Object (VAO) with multiple vertex buffers and optional index buffer
     *
     * This function sets up a complete VAO by:
     * 1. Generating a new VAO handle
     * 2. Binding vertex buffers and configuring their attribute layouts
     * 3. Setting up vertex attribute pointers for each attribute (position, color, texture coords, etc.)
     * 4. Handling both regular floating-point and integer vertex attributes
     * 5. Configuring instanced rendering divisors if needed
     * 6. Optionally binding an index buffer for indexed drawing
     *
     * A VAO encapsulates all the vertex attribute state, so once created, you can simply bind
     * the VAO to use all the configured vertex buffers and their layouts for rendering.
     *
     * \param vertices An initializer list of VertexBuffer objects, each containing:
     *                 - buffer_handle: The OpenGL buffer handle containing vertex data
     *                 - buffer_layout: Description of how the data is organized (attributes, stride, offset)
     * \param index_buffer Optional index buffer handle for indexed rendering (0 if not used)
     *
     * \return VertexArrayHandle The OpenGL handle to the created VAO
     *
     * \note The VAO will be unbound (set to 0) before returning to avoid affecting subsequent OpenGL state
     * \note Each vertex attribute will be assigned sequential attribute indices starting from 0
     */
    VertexArrayHandle CreateVertexArrayObject(std::initializer_list<VertexBuffer> vertices, BufferHandle index_buffer)
    {
        VertexArrayHandle vao{};

        GL::GenVertexArrays(1, &vao);

        GL::BindVertexArray(vao);

        GLuint attribute_index = 0;


        for (const auto& [buffer_handle, buffer_layout] : vertices)
        {
            GL::BindBuffer(GL_ARRAY_BUFFER, buffer_handle);

            GLsizei stride = 0;
            for (Attribute::Type type : buffer_layout.Attributes)
            {
                stride += type.SizeBytes;
            }

            GLintptr offset               = 0;
            GLintptr starting_byte_offset = static_cast<GLintptr>(buffer_layout.BufferStartingByteOffset);

            for (Attribute::Type attr_type : buffer_layout.Attributes)
            {
                if (attr_type == Attribute::None)
                    continue;
                GL::EnableVertexAttribArray(attribute_index);

                const GLenum    gl_type         = attr_type.GLType;
                const GLint     component_count = attr_type.ComponentCount;
                const GLboolean normalized      = attr_type.Normalize;
                const bool      is_integer      = attr_type.IntAttribute;
                const GLuint    divisor         = attr_type.Divisor;

                if (is_integer == true)
                {
                    GL::VertexAttribIPointer(attribute_index, component_count, gl_type, stride, reinterpret_cast<GLvoid*>(starting_byte_offset + offset));
                }
                else
                {
                    GL::VertexAttribPointer(attribute_index, component_count, gl_type, normalized, stride, reinterpret_cast<GLvoid*>(starting_byte_offset + offset));
                }

                GL::VertexAttribDivisor(attribute_index, divisor);

                offset += attr_type.SizeBytes;
                attribute_index++;
            }
        }

        if (index_buffer != 0)
        {
            GL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
        }

        GL::BindVertexArray(0);
        return vao;
    }

    VertexArrayHandle CreateVertexArrayObject(VertexBuffer vertices, BufferHandle index_buffer)
    {
        return CreateVertexArrayObject({ vertices }, index_buffer);
    }

}
