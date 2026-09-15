/**
 * \file InstancedRenderer2D.cpp
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "InstancedRenderer2D.hpp"
#include "CS200/RGBA.hpp"
#include "CS200/Renderer2DUtils.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/PerfStats.hpp"
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

CS200::InstancedRenderer2D::InstancedRenderer2D(unsigned max_quads)
{
    MaxQuadCount = max_quads;
    instanceData.resize(MaxQuadCount);
}

CS200::InstancedRenderer2D::InstancedRenderer2D(InstancedRenderer2D&& other) noexcept
    : instancedShader(std::move(other.instancedShader)), viewProjection(other.viewProjection)
{
}

CS200::InstancedRenderer2D& CS200::InstancedRenderer2D::operator=(InstancedRenderer2D&& other) noexcept
{
    if (this != &other)
    {
        std::swap(instancedShader, other.instancedShader);
        viewProjection = other.viewProjection;
    }
    return *this;
}

CS200::InstancedRenderer2D::~InstancedRenderer2D()
{
    Shutdown();
}

void CS200::InstancedRenderer2D::Init()
{
    GLint max_tex_units = 0;
    GL::GetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_tex_units);
    MaxTextures = std::min(static_cast<size_t>(max_tex_units), size_t{64});

    textureSlots.resize(MaxTextures);

    const std::filesystem::path vertex_file   = assets::locate_asset("Assets/shaders/InstancedRenderer2D/instance.vert");
    const std::filesystem::path fragment_file = assets::locate_asset("Assets/shaders/InstancedRenderer2D/instance.frag");

    std::ifstream     vert_stream(vertex_file);
    std::stringstream vert_buffer;
    vert_buffer << vert_stream.rdbuf();
    std::string vertex_src = vert_buffer.str();

    std::ifstream     frag_stream(fragment_file);
    std::stringstream frag_buffer;
    frag_buffer << frag_stream.rdbuf();
    std::string fragment_src = frag_buffer.str();

    size_t version_end = fragment_src.find('\n');
    if (version_end != std::string::npos)
    {
        std::string define_injection = "\n#define MAX_TEXTURE_SLOTS " + std::to_string(MaxTextures) + "\n";
        fragment_src.insert(version_end + 1, define_injection);
    }

    instancedShader = OpenGL::CreateShader(std::string_view{ vertex_src }, std::string_view{ fragment_src });

    GL::UseProgram(instancedShader.Shader);
    std::vector<int> sampler_binding_values(textureSlots.size());
    std::iota(sampler_binding_values.begin(), sampler_binding_values.end(), 0);
    const GLint location = GL::GetUniformLocation(instancedShader.Shader, "uTextures");
    GL::Uniform1iv(location, static_cast<GLsizei>(textureSlots.size()), sampler_binding_values.data());
    GL::UseProgram(0);

    float quadVerts[] = {
        -0.5f, -0.5f,  // Bottom left
         0.5f, -0.5f,  // Bottom right
         0.5f,  0.5f,  // Top right
        -0.5f,  0.5f   // Top left
    };

    GL::GenBuffers(1, &QuadVB);
    GL::BindBuffer(GL_ARRAY_BUFFER, QuadVB);
    GL::BufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);
    GL::BindBuffer(GL_ARRAY_BUFFER, 0);

    unsigned indices[] = { 0, 1, 2, 2, 3, 0 };
    GL::GenBuffers(1, &QuadIB);
    GL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, QuadIB);
    GL::BufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    GL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GL::GenBuffers(1, &InstanceVB);
    GL::BindBuffer(GL_ARRAY_BUFFER, InstanceVB);
    GL::BufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(MaxQuadCount * sizeof(InstanceData)), nullptr, GL_DYNAMIC_DRAW);
    GL::BindBuffer(GL_ARRAY_BUFFER, 0);

    GL::GenVertexArrays(1, &QuadVA);
    GL::BindVertexArray(QuadVA);
    GL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, QuadIB);

    GL::BindBuffer(GL_ARRAY_BUFFER, QuadVB);
    GL::EnableVertexAttribArray(0);
    GL::VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), reinterpret_cast<void*>(0));

    GL::BindBuffer(GL_ARRAY_BUFFER, InstanceVB);

    // Transform rows (locations 1-3)
    GL::EnableVertexAttribArray(1);
    GL::VertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(InstanceData), reinterpret_cast<void*>(offsetof(InstanceData, transformRow0)));
    GL::VertexAttribDivisor(1, 1);

    GL::EnableVertexAttribArray(2);
    GL::VertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(InstanceData), reinterpret_cast<void*>(offsetof(InstanceData, transformRow1)));
    GL::VertexAttribDivisor(2, 1);

    GL::EnableVertexAttribArray(3);
    GL::VertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(InstanceData), reinterpret_cast<void*>(offsetof(InstanceData, transformRow2)));
    GL::VertexAttribDivisor(3, 1);

    // TexCoords (locations 4-7)
    GL::EnableVertexAttribArray(4);
    GL::VertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceData), reinterpret_cast<void*>(offsetof(InstanceData, texCoord0)));
    GL::VertexAttribDivisor(4, 1);

    GL::EnableVertexAttribArray(5);
    GL::VertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceData), reinterpret_cast<void*>(offsetof(InstanceData, texCoord1)));
    GL::VertexAttribDivisor(5, 1);

    GL::EnableVertexAttribArray(6);
    GL::VertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceData), reinterpret_cast<void*>(offsetof(InstanceData, texCoord2)));
    GL::VertexAttribDivisor(6, 1);

    GL::EnableVertexAttribArray(7);
    GL::VertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceData), reinterpret_cast<void*>(offsetof(InstanceData, texCoord3)));
    GL::VertexAttribDivisor(7, 1);

    // Tint (location 8)
    GL::EnableVertexAttribArray(8);
    GL::VertexAttribPointer(8, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(InstanceData), reinterpret_cast<void*>(offsetof(InstanceData, tint)));
    GL::VertexAttribDivisor(8, 1);

    // TextureIndex (location 9)
    GL::EnableVertexAttribArray(9);
    GL::VertexAttribIPointer(9, 1, GL_INT, sizeof(InstanceData), reinterpret_cast<void*>(offsetof(InstanceData, textureIndex)));
    GL::VertexAttribDivisor(9, 1);

    // ShapeType (location 10)
    GL::EnableVertexAttribArray(10);
    GL::VertexAttribIPointer(10, 1, GL_INT, sizeof(InstanceData), reinterpret_cast<void*>(offsetof(InstanceData, shapeType)));
    GL::VertexAttribDivisor(10, 1);

    // WorldSize (location 11)
    GL::EnableVertexAttribArray(11);
    GL::VertexAttribPointer(11, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceData), reinterpret_cast<void*>(offsetof(InstanceData, worldSize)));
    GL::VertexAttribDivisor(11, 1);

    // LineWidth (location 12)
    GL::EnableVertexAttribArray(12);
    GL::VertexAttribPointer(12, 1, GL_FLOAT, GL_FALSE, sizeof(InstanceData), reinterpret_cast<void*>(offsetof(InstanceData, lineWidth)));
    GL::VertexAttribDivisor(12, 1);

    // LineColor (location 13)
    GL::EnableVertexAttribArray(13);
    GL::VertexAttribPointer(13, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(InstanceData), reinterpret_cast<void*>(offsetof(InstanceData, lineColor)));
    GL::VertexAttribDivisor(13, 1);

    GL::BindVertexArray(0);
    GL::BindBuffer(GL_ARRAY_BUFFER, 0);
    GL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void CS200::InstancedRenderer2D::Shutdown()
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

    if (InstanceVB != 0)
    {
        GL::DeleteBuffers(1, &InstanceVB);
        InstanceVB = 0;
    }

    if (instancedShader.Shader != 0)
    {
        OpenGL::DestroyShader(instancedShader);
    }
}

void CS200::InstancedRenderer2D::startBatch()
{
    instanceDataEnd = instanceData.data();
    textureSlots.clear();
}

void CS200::InstancedRenderer2D::Flush()
{
    const auto instance_count = static_cast<size_t>(instanceDataEnd - instanceData.data());
    if (instance_count == 0)
        return;

    const auto size_bytes = static_cast<GLsizeiptr>(instance_count * sizeof(InstanceData));

    auto& perf = Perf::Counters();
    ++perf.instancedFlushes;
    ++perf.instancedDrawCalls;
    perf.textureBinds += static_cast<unsigned>(textureSlots.size());
    perf.instanceBytesUploaded += static_cast<std::size_t>(size_bytes);

    GL::BindBuffer(GL_ARRAY_BUFFER, InstanceVB);
    GL::BufferSubData(GL_ARRAY_BUFFER, 0, size_bytes, instanceData.data());

    for (size_t i = 0; i < textureSlots.size(); i++)
    {
        GL::ActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + i));
        GL::BindTexture(GL_TEXTURE_2D, textureSlots[i]);
    }

    GL::UseProgram(instancedShader.Shader);
    GL::BindVertexArray(QuadVA);
    GL::DrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, static_cast<GLsizei>(instance_count));

    GL::UseProgram(0);
    GL::BindVertexArray(0);
    GL::BindTexture(GL_TEXTURE_2D, 0);
    GL::BindBuffer(GL_ARRAY_BUFFER, 0);

    startBatch();
}

void CS200::InstancedRenderer2D::BeginScene(const Math::TransformationMatrix& view_projection)
{
    viewProjection = view_projection;

    GL::UseProgram(instancedShader.Shader);
    const std::array<float, 9> to_ndc = CS200::Renderer2DUtils::to_opengl_mat3(viewProjection);
    GL::UniformMatrix3fv(instancedShader.UniformLocations.at("uToNDC"), 1, GL_FALSE, to_ndc.data());
    GL::UseProgram(0);

    startBatch();
}

void CS200::InstancedRenderer2D::EndScene()
{
    Flush();
}

void CS200::InstancedRenderer2D::DrawQuad(const Math::TransformationMatrix& transform, OpenGL::TextureHandle texture, Math::vec2 texture_coord_bl, Math::vec2 texture_coord_tr, CS200::RGBA tintColor)
{
    ++Perf::Counters().quadsSubmitted;

    if (static_cast<size_t>(instanceDataEnd - instanceData.data()) >= MaxQuadCount)
    {
        Flush();
    }

    int textureIndex = 0;

    for (size_t i = 0; i < textureSlots.size(); i++)
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

    auto                         tint_unpacked = CS200::unpack_color(tintColor);
    std::array<unsigned char, 4> tint_bytes    = { static_cast<unsigned char>(tint_unpacked[0] * 255.0f), static_cast<unsigned char>(tint_unpacked[1] * 255.0f),
                                                   static_cast<unsigned char>(tint_unpacked[2] * 255.0f), static_cast<unsigned char>(tint_unpacked[3] * 255.0f) };

    instanceDataEnd->transformRow0[0] = static_cast<float>(transform[0][0]);
    instanceDataEnd->transformRow0[1] = static_cast<float>(transform[1][0]);
    instanceDataEnd->transformRow0[2] = static_cast<float>(transform[2][0]);

    instanceDataEnd->transformRow1[0] = static_cast<float>(transform[0][1]);
    instanceDataEnd->transformRow1[1] = static_cast<float>(transform[1][1]);
    instanceDataEnd->transformRow1[2] = static_cast<float>(transform[2][1]);

    instanceDataEnd->transformRow2[0] = static_cast<float>(transform[0][2]);
    instanceDataEnd->transformRow2[1] = static_cast<float>(transform[1][2]);
    instanceDataEnd->transformRow2[2] = static_cast<float>(transform[2][2]);

    instanceDataEnd->texCoord0[0] = static_cast<float>(texture_coord_bl.x); // Bottom left
    instanceDataEnd->texCoord0[1] = static_cast<float>(texture_coord_bl.y);

    instanceDataEnd->texCoord1[0] = static_cast<float>(texture_coord_tr.x); // Bottom right
    instanceDataEnd->texCoord1[1] = static_cast<float>(texture_coord_bl.y);

    instanceDataEnd->texCoord2[0] = static_cast<float>(texture_coord_tr.x); // Top right
    instanceDataEnd->texCoord2[1] = static_cast<float>(texture_coord_tr.y);

    instanceDataEnd->texCoord3[0] = static_cast<float>(texture_coord_bl.x); // Top left
    instanceDataEnd->texCoord3[1] = static_cast<float>(texture_coord_tr.y);

    instanceDataEnd->tint = tint_bytes;
    instanceDataEnd->textureIndex = textureIndex;

    instanceDataEnd->shapeType = -1;
    instanceDataEnd->worldSize[0] = 0.0f;
    instanceDataEnd->worldSize[1] = 0.0f;
    instanceDataEnd->lineWidth = 0.0f;
    instanceDataEnd->lineColor = {0, 0, 0, 0};

    ++instanceDataEnd;
}

void CS200::InstancedRenderer2D::DrawCircle(const Math::TransformationMatrix& transform, CS200::RGBA fill_color, CS200::RGBA line_color, double line_width)
{
    DrawSDF(transform, fill_color, line_color, line_width, SDFShape::Circle);
}

void CS200::InstancedRenderer2D::DrawRectangle(const Math::TransformationMatrix& transform, CS200::RGBA fill_color, CS200::RGBA line_color, double line_width)
{
    DrawSDF(transform, fill_color, line_color, line_width, SDFShape::Rectangle);
}

void CS200::InstancedRenderer2D::DrawLine(const Math::TransformationMatrix& transform, Math::vec2 startPoint, Math::vec2 endPoint, CS200::RGBA line_color, double line_width)
{
    const auto line_transform = CS200::Renderer2DUtils::CalculateLineTransform(transform, startPoint, endPoint, line_width);
    DrawSDF(line_transform, line_color, line_color, line_width, SDFShape::Rectangle);
}

void CS200::InstancedRenderer2D::DrawLine(Math::vec2 start_point, Math::vec2 end_point, CS200::RGBA line_color, double line_width)
{
    DrawLine(Math::TransformationMatrix{}, start_point, end_point, line_color, line_width);
}

void CS200::InstancedRenderer2D::DrawSDF(const Math::TransformationMatrix& transform, CS200::RGBA fill_color, CS200::RGBA line_color, double line_width, SDFShape sdf_shape)
{
    ++Perf::Counters().quadsSubmitted;

    if (static_cast<size_t>(instanceDataEnd - instanceData.data()) >= MaxQuadCount)
    {
        Flush();
    }

    double scaleX = std::sqrt(transform[0][0] * transform[0][0] + transform[0][1] * transform[0][1]);
    double scaleY = std::sqrt(transform[1][0] * transform[1][0] + transform[1][1] * transform[1][1]);

    instanceDataEnd->transformRow0[0] = static_cast<float>(transform[0][0]);
    instanceDataEnd->transformRow0[1] = static_cast<float>(transform[1][0]);
    instanceDataEnd->transformRow0[2] = static_cast<float>(transform[2][0]);

    instanceDataEnd->transformRow1[0] = static_cast<float>(transform[0][1]);
    instanceDataEnd->transformRow1[1] = static_cast<float>(transform[1][1]);
    instanceDataEnd->transformRow1[2] = static_cast<float>(transform[2][1]);

    instanceDataEnd->transformRow2[0] = static_cast<float>(transform[0][2]);
    instanceDataEnd->transformRow2[1] = static_cast<float>(transform[1][2]);
    instanceDataEnd->transformRow2[2] = static_cast<float>(transform[2][2]);

    instanceDataEnd->texCoord0[0] = 0.0f;
    instanceDataEnd->texCoord0[1] = 0.0f;
    instanceDataEnd->texCoord1[0] = 1.0f;
    instanceDataEnd->texCoord1[1] = 0.0f;
    instanceDataEnd->texCoord2[0] = 1.0f;
    instanceDataEnd->texCoord2[1] = 1.0f;
    instanceDataEnd->texCoord3[0] = 0.0f;
    instanceDataEnd->texCoord3[1] = 1.0f;

    // Unpack fill color
    auto fill_unpacked = CS200::unpack_color(fill_color);
    instanceDataEnd->tint = {
        static_cast<unsigned char>(fill_unpacked[0] * 255.0f),
        static_cast<unsigned char>(fill_unpacked[1] * 255.0f),
        static_cast<unsigned char>(fill_unpacked[2] * 255.0f),
        static_cast<unsigned char>(fill_unpacked[3] * 255.0f)
    };

    // Unpack line color
    auto line_unpacked = CS200::unpack_color(line_color);
    instanceDataEnd->lineColor = {
        static_cast<unsigned char>(line_unpacked[0] * 255.0f),
        static_cast<unsigned char>(line_unpacked[1] * 255.0f),
        static_cast<unsigned char>(line_unpacked[2] * 255.0f),
        static_cast<unsigned char>(line_unpacked[3] * 255.0f)
    };

    instanceDataEnd->textureIndex = 0;

    instanceDataEnd->shapeType = static_cast<int>(sdf_shape);
    instanceDataEnd->worldSize[0] = static_cast<float>(scaleX);
    instanceDataEnd->worldSize[1] = static_cast<float>(scaleY);
    instanceDataEnd->lineWidth = static_cast<float>(line_width);

    ++instanceDataEnd;
}
