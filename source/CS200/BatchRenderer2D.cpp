/**
 * \file BatchRenderer2D.cpp
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "BatchRenderer2D.hpp"
#include "CS200/RGBA.hpp"
#include "CS200/Renderer2DUtils.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Path.hpp"
#include "Engine/Vec2.hpp"
#include "OpenGL/GL.hpp"
#include "OpenGL/Shader.hpp"
#include <array>
#include <cstring>
#include <fstream>
#include <numeric>
#include <sstream>
#include <string>

CS200::BatchRenderer2D::BatchRenderer2D(unsigned max_quads)
{
    MaxQuadCount   = max_quads;
    MaxVertexCount = MaxQuadCount * 4;
    MaxIndexCount  = MaxQuadCount * 6;
    vertexBase.resize(MaxVertexCount);
}

CS200::BatchRenderer2D::BatchRenderer2D(BatchRenderer2D&& other) noexcept : batchShader(std::move(other.batchShader)), viewProjection(other.viewProjection)
{
}

CS200::BatchRenderer2D& CS200::BatchRenderer2D::operator=(BatchRenderer2D&& other) noexcept
{
    if (this != &other)
    {
        std::swap(batchShader, other.batchShader);
        viewProjection = other.viewProjection;
    }
    return *this;
}

CS200::BatchRenderer2D::~BatchRenderer2D()
{
    Shutdown();
}

void CS200::BatchRenderer2D::Init()
{
    GLint max_tex_units = 0;
    GL::GetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_tex_units);
    MaxTextures = std::min(static_cast<size_t>(max_tex_units), size_t{ 64 });

    textureSlots.resize(MaxTextures);

    const std::filesystem::path vertex_file   = assets::locate_asset("Assets/shaders/BatchRenderer2D/batch.vert");
    const std::filesystem::path fragment_file = assets::locate_asset("Assets/shaders/BatchRenderer2D/batch.frag");

    std::ifstream     vert_stream(vertex_file);
    std::stringstream vert_buffer;
    vert_buffer << vert_stream.rdbuf();
    std::string vertex_src = vert_buffer.str();

    std::ifstream     frag_stream(fragment_file);
    std::stringstream frag_buffer;
    frag_buffer << frag_stream.rdbuf();
    std::string fragment_src = frag_buffer.str();

    size_t      version_end = fragment_src.find('\n');
    std::string define      = "\n#define MAX_TEXTURE_SLOTS " + std::to_string(textureSlots.size());
    fragment_src.insert(version_end, define);

    batchShader = OpenGL::CreateShader(std::string_view{ vertex_src }, std::string_view{ fragment_src });

    GL::UseProgram(batchShader.Shader);
    std::vector<int> sampler_binding_values(textureSlots.size());
    std::iota(sampler_binding_values.begin(), sampler_binding_values.end(), 0);
    const GLint location = GL::GetUniformLocation(batchShader.Shader, "uTextures");
    GL::Uniform1iv(location, static_cast<GLsizei>(textureSlots.size()), sampler_binding_values.data());
    GL::UseProgram(0);

    GL::GenBuffers(1, &QuadVB);
    GL::BindBuffer(GL_ARRAY_BUFFER, QuadVB);
    GL::BufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(MaxVertexCount * sizeof(Vertex)), nullptr, GL_DYNAMIC_DRAW);
    GL::BindBuffer(GL_ARRAY_BUFFER, 0);

    std::vector<unsigned> indices(MaxIndexCount);
    unsigned              offset = 0;
    for (size_t i = 0; i < MaxIndexCount; i += 6)
    {
        indices[i + 0] = 0 + offset;
        indices[i + 1] = 1 + offset;
        indices[i + 2] = 2 + offset;
        indices[i + 3] = 2 + offset;
        indices[i + 4] = 3 + offset;
        indices[i + 5] = 0 + offset;
        offset += 4;
    }

    GL::GenBuffers(1, &QuadIB);
    GL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, QuadIB);
    GL::BufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(MaxIndexCount * sizeof(uint32_t)), indices.data(), GL_STATIC_DRAW);
    GL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GL::GenVertexArrays(1, &QuadVA);
    GL::BindVertexArray(QuadVA);
    GL::BindBuffer(GL_ARRAY_BUFFER, QuadVB);
    GL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, QuadIB);

    // Position (location 0)
    GL::EnableVertexAttribArray(0);
    GL::VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(0));

    // TexCoord (location 1)
    GL::EnableVertexAttribArray(1);
    GL::VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(2 * sizeof(float)));

    // Tint (location 2)
    GL::EnableVertexAttribArray(2);
    GL::VertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, tint)));

    // TextureIndex (location 3)
    GL::EnableVertexAttribArray(3);
    GL::VertexAttribIPointer(3, 1, GL_INT, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texIndex)));

    // Unbind
    GL::BindVertexArray(0);
    GL::BindBuffer(GL_ARRAY_BUFFER, 0);
    GL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void CS200::BatchRenderer2D::Shutdown()
{
    if (QuadVA != 0)
    {
        GL::DeleteVertexArrays(1, &QuadVA);
        QuadVA = 0;
    }

    if (QuadVB != 0)
    {
        GL::DeleteBuffers(1, &QuadVB);
        QuadVB = 0;
    }

    if (QuadIB != 0)
    {
        GL::DeleteBuffers(1, &QuadIB);
        QuadIB = 0;
    }

    if (batchShader.Shader != 0)
    {
        OpenGL::DestroyShader(batchShader);
    }
}

void CS200::BatchRenderer2D::startBatch()
{
    vertexPtr  = vertexBase.data();
    indexCount = 0;
    textureSlots.clear();
}

void CS200::BatchRenderer2D::Flush()
{
    if (indexCount == 0)
        return;

    const auto vertex_count = static_cast<size_t>(vertexPtr - vertexBase.data());
    const auto size_bytes   = static_cast<GLsizeiptr>(vertex_count * sizeof(Vertex));
    GL::BindBuffer(GL_ARRAY_BUFFER, QuadVB);
    GL::BufferSubData(GL_ARRAY_BUFFER, 0, size_bytes, vertexBase.data());

    for (size_t i = 0; i < textureSlots.size(); i++)
    {
        GL::ActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + i));
        GL::BindTexture(GL_TEXTURE_2D, textureSlots[i]);
    }

    GL::UseProgram(batchShader.Shader);
    GL::BindVertexArray(QuadVA);
    GL::DrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, nullptr);

    // Unbind
    GL::UseProgram(0);
    GL::BindVertexArray(0);
    GL::BindTexture(GL_TEXTURE_2D, 0);
    GL::BindBuffer(GL_ARRAY_BUFFER, 0);

    startBatch();
}

void CS200::BatchRenderer2D::BeginScene(const Math::TransformationMatrix& view_projection)
{
    viewProjection = view_projection;

    GL::UseProgram(batchShader.Shader);
    const std::array<float, 9> to_ndc = CS200::Renderer2DUtils::to_opengl_mat3(viewProjection);
    GL::UniformMatrix3fv(batchShader.UniformLocations.at("uToNDC"), 1, GL_FALSE, to_ndc.data());
    GL::UseProgram(0);
    startBatch();
}

void CS200::BatchRenderer2D::EndScene()
{
    Flush();
}

void CS200::BatchRenderer2D::DrawQuad(const Math::TransformationMatrix& transform, OpenGL::TextureHandle texture, Math::vec2 texture_coord_bl, Math::vec2 texture_coord_tr, CS200::RGBA tintColor)
{
    Math::vec2 quadVertices[4] = {
        { -0.5, -0.5 }, // Bottom left
        {  0.5, -0.5 }, // Bottom right
        {  0.5,  0.5 }, // Top right
        { -0.5,  0.5 }  // Top left
    };

    if (indexCount + 6 > MaxIndexCount)
    {
        Flush();
    }

    int    textureIndex = 0;
    size_t size_        = textureSlots.size();

    for (size_t i = 0; i < size_; i++)
    {
        if (textureSlots[i] == texture)
        {
            textureIndex = static_cast<int>(i);
            goto found;
        }
    }

    if (textureSlots.size() >= MaxTextures)
    {
        Flush();
    }

    textureIndex = static_cast<int>(textureSlots.size());
    textureSlots.push_back(texture);

found:

    const Math::vec2 texCoords[4] = {
        { texture_coord_bl.x, texture_coord_bl.y }, // Bottom left
        { texture_coord_tr.x, texture_coord_bl.y }, // Bottom right
        { texture_coord_tr.x, texture_coord_tr.y }, // Top right
        { texture_coord_bl.x, texture_coord_tr.y }  // Top left
    };


    for (int i = 0; i < 4; ++i)
    {
        double x       = quadVertices[i].x;
        double y       = quadVertices[i].y;
        double world_x = x * transform[0][0] + y * transform[0][1] + transform[0][2];
        double world_y = x * transform[1][0] + y * transform[1][1] + transform[1][2];

        vertexPtr->x        = static_cast<float>(world_x);
        vertexPtr->y        = static_cast<float>(world_y);
        vertexPtr->s        = static_cast<float>(texCoords[i].x);
        vertexPtr->t        = static_cast<float>(texCoords[i].y);
        vertexPtr->tint     = tintColor;
        vertexPtr->texIndex = textureIndex;

        ++vertexPtr;
    }

    indexCount += 6;
}