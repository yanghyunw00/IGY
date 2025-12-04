

#include "CS200/BatchRenderer2D.hpp"
#include "CS200/RGBA.hpp"
#include "Engine/Path.hpp"
#include "CS200/Renderer2DUtils.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Logger.hpp"
#include "OpenGL/Buffer.hpp"
#include "OpenGL/GL.hpp"
#include <iostream>
#include <cstddef>
#include <utility>

namespace CS200
{
    BatchRenderer2D::BatchRenderer2D(unsigned max_quads)
    {
        maxVertices = max_quads * 4;
        maxIndices  = max_quads * 6;
        vertexData.resize(maxVertices);
    }

    void BatchRenderer2D::Init()
    {
        const std::filesystem::path vertex_file   = assets::locate_asset("Assets/shaders/BatchRenderer2D/batch.vert");
        const std::filesystem::path fragment_file = assets::locate_asset("Assets/shaders/BatchRenderer2D/batch.frag");
        shader                                    = OpenGL::CreateShader(vertex_file, fragment_file);

        GL::UseProgram(shader.Shader);
        GL::Uniform1i(shader.UniformLocations.at("uTexture"), 0);
        GL::UseProgram(0);

        GL::GenVertexArrays(1, &vertexArrayObject);
        GL::BindVertexArray(vertexArrayObject);

        GL::GenBuffers(1, &vertexBuffer);
        GL::BindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        GL::BufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(QuadVertex) * maxVertices), nullptr, GL_DYNAMIC_DRAW);

        GL::EnableVertexAttribArray(0);
        GL::VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), reinterpret_cast<void*>(offsetof(QuadVertex, x)));

        GL::EnableVertexAttribArray(1);
        GL::VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), reinterpret_cast<void*>(offsetof(QuadVertex, s)));

        GL::EnableVertexAttribArray(2);
        GL::VertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(QuadVertex), reinterpret_cast<void*>(offsetof(QuadVertex, tint)));

        std::vector<unsigned> indices(maxIndices);
        unsigned              offset = 0;
        for (unsigned i = 0; i < maxIndices; i += 6)
        {
            indices[i + 0] = offset + 0;
            indices[i + 1] = offset + 1;
            indices[i + 2] = offset + 2;
            indices[i + 3] = offset + 2;
            indices[i + 4] = offset + 3;
            indices[i + 5] = offset + 0;
            offset += 4;
        }

        GL::GenBuffers(1, &indexBuffer);
        GL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        GL::BufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(unsigned) * maxIndices), indices.data(), GL_STATIC_DRAW);

        GL::BindVertexArray(0);
        GL::BindBuffer(GL_ARRAY_BUFFER, 0);
        GL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        startBatch();
    }

    void BatchRenderer2D::Shutdown()
    {
        if (vertexArrayObject != 0)
        {
            GL::DeleteVertexArrays(1, &vertexArrayObject);
            vertexArrayObject = 0;
        }

        if (vertexBuffer != 0)
        {
            GL::DeleteBuffers(1, &vertexBuffer);
            vertexBuffer = 0;
        }

        if (indexBuffer != 0)
        {
            GL::DeleteBuffers(1, &indexBuffer);
            indexBuffer = 0;
        }

        if (shader.Shader != 0)
        {
            OpenGL::DestroyShader(shader);
        }
    }

    void BatchRenderer2D::BeginScene(const Math::TransformationMatrix& view_projection)
    {
        viewProjection = view_projection;
        startBatch();
    }

    void BatchRenderer2D::EndScene()
    {
        flush();
    }

    void BatchRenderer2D::DrawQuad(const Math::TransformationMatrix& transform, OpenGL::TextureHandle texture, Math::vec2 texture_coord_bl, Math::vec2 texture_coord_tr, CS200::RGBA tintColor)
    {
        if (texture == 0)
        {
            return;
        }
        if (indexCount >= maxIndices || (activeTexture != 0 && activeTexture != texture))
        {
            flush();
        }

        if (activeTexture == 0)
        {
            activeTexture = texture;
        }
        else if (activeTexture != texture)
        {
            flush();
            activeTexture = texture;
        }

        const auto tint_unpacked = CS200::unpack_color(tintColor);
        const std::array<unsigned char, 4> tint_bytes = {
            static_cast<unsigned char>(tint_unpacked[0] * 255.0f),
            static_cast<unsigned char>(tint_unpacked[1] * 255.0f),
            static_cast<unsigned char>(tint_unpacked[2] * 255.0f),
            static_cast<unsigned char>(tint_unpacked[3] * 255.0f)
        };

        const Math::vec2 positions[4] = {
            { -0.5, -0.5 },
            {  0.5, -0.5 },
            {  0.5,  0.5 },
            { -0.5,  0.5 }
        };

        const Math::vec2 texCoords[4] = {
            texture_coord_bl,
            { texture_coord_tr.x, texture_coord_bl.y },
            texture_coord_tr,
            { texture_coord_bl.x, texture_coord_tr.y }
        };

        for (int i = 0; i < 4; ++i)
        {
            const double worldX = transform[0][0] * positions[i].x + transform[0][1] * positions[i].y + transform[0][2];
            const double worldY = transform[1][0] * positions[i].x + transform[1][1] * positions[i].y + transform[1][2];

            vertexDataEnd->x    = static_cast<float>(worldX);
            vertexDataEnd->y    = static_cast<float>(worldY);
            vertexDataEnd->s    = static_cast<float>(texCoords[i].x);
            vertexDataEnd->t    = static_cast<float>(texCoords[i].y);
            vertexDataEnd->tint = tint_bytes;
            ++vertexDataEnd;
        }

        indexCount += 6;
    }

    void BatchRenderer2D::DrawCircle(const Math::TransformationMatrix& transform, CS200::RGBA fill_color, CS200::RGBA line_color, double line_width)
    {
        flush();
        auto& instanced = Engine::GetRenderer2D(Dokev::RenderState::INSTANCED);
        instanced.BeginScene(viewProjection);
        instanced.DrawCircle(transform, fill_color, line_color, line_width);
        instanced.EndScene();
    }

    void BatchRenderer2D::DrawRectangle(const Math::TransformationMatrix& transform, CS200::RGBA fill_color, CS200::RGBA line_color, double line_width)
    {
        flush();
        auto& instanced = Engine::GetRenderer2D(Dokev::RenderState::INSTANCED);
        instanced.BeginScene(viewProjection);
        instanced.DrawRectangle(transform, fill_color, line_color, line_width);
        instanced.EndScene();
    }

    void BatchRenderer2D::DrawLine(const Math::TransformationMatrix& transform, Math::vec2 startPoint, Math::vec2 endPoint, CS200::RGBA line_color, double line_width)
    {
        flush();
        auto& instanced = Engine::GetRenderer2D(Dokev::RenderState::INSTANCED);
        instanced.BeginScene(viewProjection);
        instanced.DrawLine(transform, startPoint, endPoint, line_color, line_width);
        instanced.EndScene();
    }

    void BatchRenderer2D::DrawLine(Math::vec2 start_point, Math::vec2 end_point, CS200::RGBA line_color, double line_width)
    {
        flush();
        auto& instanced = Engine::GetRenderer2D(Dokev::RenderState::INSTANCED);
        instanced.BeginScene(viewProjection);
        instanced.DrawLine(start_point, end_point, line_color, line_width);
        instanced.EndScene();
    }

    void BatchRenderer2D::flush()
    {
        const auto vertex_count = static_cast<unsigned>(vertexDataEnd - vertexData.data());
        if (vertex_count == 0 || activeTexture == 0)
        {
            startBatch();
            return;
        }

        GL::UseProgram(shader.Shader);
        const auto to_ndc = CS200::Renderer2DUtils::to_opengl_mat3(viewProjection);
        GL::UniformMatrix3fv(shader.UniformLocations.at("uToNDC"), 1, GL_FALSE, to_ndc.data());

        GL::BindVertexArray(vertexArrayObject);
        GL::BindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        GL::BufferSubData(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(sizeof(QuadVertex) * vertex_count), vertexData.data());

        GL::ActiveTexture(GL_TEXTURE0);
        GL::BindTexture(GL_TEXTURE_2D, activeTexture);

        GL::DrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, nullptr);

        GL::BindTexture(GL_TEXTURE_2D, 0);
        GL::BindVertexArray(0);
        GL::UseProgram(0);

        startBatch();
    }

    void BatchRenderer2D::startBatch()
    {
        vertexDataEnd = vertexData.data();
        indexCount    = 0;
        activeTexture = 0;
    }
}
