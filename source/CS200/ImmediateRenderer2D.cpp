/**
 * \file
 * \author Rudy Castan
 * \author Hyunwwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "ImmediateRenderer2D.hpp"
#include "CS200/Renderer2DUtils.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Texture.hpp"
#include "OpenGL/Buffer.hpp"
#include "OpenGL/GL.hpp"
#include <iostream>
#include <utility>

namespace CS200
{
    ImmediateRenderer2D::ImmediateRenderer2D(ImmediateRenderer2D&& other) noexcept
        : vao(other.vao), vbo(other.vbo), ibo(other.ibo), uniformBlock(other.uniformBlock), indicesCount(other.indicesCount), sdf_vao(other.sdf_vao), sdfVbo(other.sdfVbo),
          textureShader(std::move(other.textureShader)), sdfShader(std::move(other.sdfShader)), uboCamera(other.uboCamera), viewProjection(other.viewProjection)
    {
        other.vao          = 0;
        other.vbo          = 0;
        other.ibo          = 0;
        other.indicesCount = 0;
        other.sdf_vao       = 0;
        other.sdfVbo       = 0;
        other.uboCamera    = 0;
        other.uniformBlock = 0;
    }

    ImmediateRenderer2D& ImmediateRenderer2D::operator=(ImmediateRenderer2D&& other) noexcept
    {
        if (this != &other)
        {
            std::swap(vao, other.vao);
            std::swap(vbo, other.vbo);
            std::swap(ibo, other.ibo);
            std::swap(indicesCount, other.indicesCount);
            std::swap(sdf_vao, other.sdf_vao);
            std::swap(uniformBlock, other.uniformBlock);
            std::swap(sdfVbo, other.sdfVbo);
            std::swap(uboCamera, other.uboCamera);
            std::swap(viewProjection, other.viewProjection);
            std::swap(textureShader, other.textureShader);
            std::swap(sdfShader, other.sdfShader);
        }

        return *this;
    }

    ImmediateRenderer2D::~ImmediateRenderer2D()
    {
        Shutdown();
    }

    struct Vertex
    {
        float x;
        float y;
        float s;
        float t;
    };

    struct SDFVertex
    {
        float x;
        float y;
    };

    void ImmediateRenderer2D::Init()
    {
        const std::array<unsigned int, 6> indices  = { 0, 1, 2, 2, 3, 0 };
        const std::array<Vertex, 4>       vertices = {
            Vertex{ -0.5f, -0.5f, 0.0f, 0.0f },
            Vertex{  0.5f, -0.5f, 1.0f, 0.0f },
            Vertex{  0.5f,  0.5f, 1.0f, 1.0f },
            Vertex{ -0.5f,  0.5f, 0.0f, 1.0f }
        };

        const std::array<SDFVertex, 4> sdf_vertices = {
            SDFVertex{ -0.5f, -0.5f },
            SDFVertex{  0.5f, -0.5f },
            SDFVertex{  0.5f,  0.5f },
            SDFVertex{ -0.5f,  0.5f }
        };

        using filepath = std::filesystem::path;
        textureShader  = OpenGL::CreateShader(filepath{ "Assets/shaders/ImmediateRenderer2D/quad.vert" }, filepath{ "Assets/shaders/ImmediateRenderer2D/quad.frag" });
        sdfShader      = OpenGL::CreateShader(filepath{ "Assets/shaders/ImmediateRenderer2D/sdf.vert" }, filepath{ "Assets/shaders/ImmediateRenderer2D/sdf.frag" });


        vbo = OpenGL::CreateBuffer(OpenGL::BufferType::Vertices, std::as_bytes(std::span{ vertices }));
        ibo = OpenGL::CreateBuffer(OpenGL::BufferType::Indices, std::as_bytes(std::span{ indices }));

        sdfVbo       = OpenGL::CreateBuffer(OpenGL::BufferType::Vertices, std::as_bytes(std::span{ sdf_vertices }));
        indicesCount = static_cast<GLsizei>(indices.size());

        GL::GenBuffers(1, &uboCamera);
        GL::BindBuffer(GL_UNIFORM_BUFFER, uboCamera);
        GL::BufferData(GL_UNIFORM_BUFFER, 48, nullptr, GL_DYNAMIC_DRAW);
        GL::BindBufferBase(GL_UNIFORM_BUFFER, 0, uboCamera);

        GLuint blockIndex = GL::GetUniformBlockIndex(textureShader.Shader, "Camera");
        if (blockIndex != GL_INVALID_INDEX)
        {
            GL::UniformBlockBinding(textureShader.Shader, blockIndex, 0);
        }

        const auto layout = OpenGL::BufferLayout{
            { OpenGL::Attribute::Float2, OpenGL::Attribute::Float2 }
        };

        const auto sdf_layout    = OpenGL::BufferLayout{ { OpenGL::Attribute::Float2 } };
        const auto vertex_buffer = OpenGL::VertexBuffer{ vbo, layout };
        vao                      = OpenGL::CreateVertexArrayObject(vertex_buffer, ibo);

        const auto sdf_vertex_buffer = OpenGL::VertexBuffer{ sdfVbo, sdf_layout };
        sdf_vao                       = OpenGL::CreateVertexArrayObject(sdf_vertex_buffer, ibo);
    }

    void ImmediateRenderer2D::Shutdown()
    {
        OpenGL::DestroyShader(textureShader);
        OpenGL::DestroyShader(sdfShader);

        GL::DeleteBuffers(1, &vbo);
        GL::DeleteBuffers(1, &ibo);
        GL::DeleteBuffers(1, &uniformBlock);
        GL::DeleteBuffers(1, &sdfVbo);

        GL::DeleteVertexArrays(1, &vao);
        GL::DeleteVertexArrays(1, &sdf_vao);


        vao          = 0;
        vbo          = 0;
        ibo          = 0;
        uniformBlock = 0;
        indicesCount = 0;
        sdf_vao       = 0;
        sdfVbo       = 0;
        uboCamera    = 0;
    }

    void ImmediateRenderer2D::BeginScene(const Math::TransformationMatrix& view_projection)
    {
        this->viewProjection = view_projection;

        std::array<float, 12> ubo_data = { static_cast<float>(view_projection[0][0]), static_cast<float>(view_projection[1][0]), static_cast<float>(view_projection[2][0]), 0.0f,
                                           static_cast<float>(view_projection[0][1]), static_cast<float>(view_projection[1][1]), static_cast<float>(view_projection[2][1]), 0.0f,
                                           static_cast<float>(view_projection[0][2]), static_cast<float>(view_projection[1][2]), static_cast<float>(view_projection[2][2]), 0.0f };

        GL::BindBuffer(GL_UNIFORM_BUFFER, uboCamera);
        GL::BufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ubo_data), ubo_data.data());
        GL::BindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void ImmediateRenderer2D::EndScene()
    {
    }

    void ImmediateRenderer2D::DrawQuad(const Math::TransformationMatrix& transform, OpenGL::TextureHandle texture, Math::vec2 texture_coord_bl, Math::vec2 texture_coord_tr, CS200::RGBA tintColor)
    {
        if (vao == 0 || indicesCount == 0)
        {
            return;
        }

        GL::UseProgram(textureShader.Shader);

        const Math::vec2                 tex_scale          = texture_coord_tr - texture_coord_bl;
        const Math::TransformationMatrix texcoord_transform = Math::TranslationMatrix(texture_coord_bl) * Math::ScaleMatrix(tex_scale);

        const std::array<float, 9> model      = CS200::Renderer2DUtils::to_opengl_mat3(transform);
        const std::array<float, 9> tex_mat    = CS200::Renderer2DUtils::to_opengl_mat3(texcoord_transform);
        const std::array<float, 4> tint_array = CS200::unpack_color(tintColor);


        GL::UniformMatrix3fv(textureShader.UniformLocations.at("uModel"), 1, GL_FALSE, model.data());
        GL::UniformMatrix3fv(textureShader.UniformLocations.at("uTexCoordTransform"), 1, GL_FALSE, tex_mat.data());
        GL::Uniform4fv(textureShader.UniformLocations.at("uTint"), 1, tint_array.data());
        GL::Uniform1f(textureShader.UniformLocations.at("uDepth"), 0.0f);
        GL::Uniform1i(textureShader.UniformLocations.at("uTexture"), 0);

        GL::ActiveTexture(GL_TEXTURE0);
        GL::BindTexture(GL_TEXTURE_2D, texture);
        GL::BindVertexArray(vao);
        GL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        GL::DrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, nullptr);

        GL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        GL::BindVertexArray(0);
        GL::UseProgram(0);
    }
    void ImmediateRenderer2D::DrawSDF(const Math::TransformationMatrix& transform, CS200::RGBA fill_color, CS200::RGBA line_color, double line_width, SDFShape sdf_shape)
    {
        if (sdf_vao == 0 || indicesCount == 0)
        {
            return;
        }

        GL::UseProgram(sdfShader.Shader);

        const auto sdf_transform = CS200::Renderer2DUtils::CalculateSDFTransform(transform, line_width);

        const std::array<float, 9> model  = sdf_transform.QuadTransform;
        const std::array<float, 9> to_ndc = CS200::Renderer2DUtils::to_opengl_mat3(viewProjection);

        const std::array<float, 4> fill_array = CS200::unpack_color(fill_color);
        const std::array<float, 4> line_array = CS200::unpack_color(line_color);

        GL::UniformMatrix3fv(sdfShader.UniformLocations.at("uModel"), 1, GL_FALSE, model.data());
        GL::UniformMatrix3fv(sdfShader.UniformLocations.at("uToNDC"), 1, GL_FALSE, to_ndc.data());
        GL::Uniform2fv(sdfShader.UniformLocations.at("uSDFScale"), 1, sdf_transform.QuadSize.data());
        GL::Uniform4fv(sdfShader.UniformLocations.at("uFillColor"), 1, fill_array.data());
        GL::Uniform4fv(sdfShader.UniformLocations.at("uLineColor"), 1, line_array.data());
        GL::Uniform2fv(sdfShader.UniformLocations.at("uWorldSize"), 1, sdf_transform.WorldSize.data());
        GL::Uniform1f(sdfShader.UniformLocations.at("uLineWidth"), static_cast<float>(line_width));
        GL::Uniform1i(sdfShader.UniformLocations.at("uShape"), static_cast<GLint>(sdf_shape));

        GL::BindVertexArray(sdf_vao);
        GL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        GL::DrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, nullptr);

        GL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        GL::BindVertexArray(0);
        GL::UseProgram(0);
    }

    void ImmediateRenderer2D::DrawCircle(const Math::TransformationMatrix& transform, CS200::RGBA fill_color, CS200::RGBA line_color, double line_width)
    {
        DrawSDF(transform, fill_color, line_color, line_width, SDFShape::Circle);
    }

    void ImmediateRenderer2D::DrawRectangle(const Math::TransformationMatrix& transform, CS200::RGBA fill_color, CS200::RGBA line_color, double line_width)
    {
        DrawSDF(transform, fill_color, line_color, line_width, SDFShape::Rectangle);
    }

    void ImmediateRenderer2D::DrawLine(const Math::TransformationMatrix& transform, Math::vec2 start_point, Math::vec2 end_point, CS200::RGBA line_color, double line_width)
    {
        const auto line_transform = Renderer2DUtils::CalculateLineTransform(transform, start_point, end_point, line_width);
        DrawSDF(line_transform, line_color, line_color, line_width, SDFShape::Rectangle);
    }

    void ImmediateRenderer2D::DrawLine(Math::vec2 start_point, Math::vec2 end_point, CS200::RGBA line_color, double line_width)
    {
        DrawLine(Math::TransformationMatrix{}, start_point, end_point, line_color, line_width);
    }

}