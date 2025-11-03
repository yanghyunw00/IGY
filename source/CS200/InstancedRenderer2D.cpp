#include "InstancedRenderer2D.hpp"
#include "Engine/Path.hpp"
#include "OpenGL/GL.hpp"
#include "CS200/Renderer2DUtils.hpp"
#include <fstream>
#include <numeric>
#include <sstream>
#include <cmath>

InstancedRenderer2D::InstancedRenderer2D(unsigned max_sprites)
{
    maxInstances = max_sprites;
    instanceData.reserve(max_sprites);
}

void InstancedRenderer2D::Init()
{
    // get max texture units
    // get glsl code and update the fragment shader
    // create the shader
    // set the binding values for textures array

    GLint max_tex_units = 0;
    GL::GetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_tex_units);
    textureSlots.resize(std::min(max_tex_units, 64));

    // load shaders
    const std::filesystem::path vertex_file = assets::locate_asset("Assets/shaders/instance.vert");
    std::ifstream               vert_stream(vertex_file);
    std::stringstream           vert_text_stream;
    vert_text_stream << vert_stream.rdbuf();
    const std::string vertex_glsl = vert_text_stream.str();


    const std::filesystem::path fragment_file = assets::locate_asset("Assets/shaders/instance.frag");
    std::ifstream               frag_stream(fragment_file);
    std::stringstream           frag_text_stream;
    frag_text_stream << frag_stream.rdbuf();
    std::string       frag_glsl     = frag_text_stream.str();
    const size_t      first_newline = frag_glsl.find('\n');
    const std::string define_line   = "\n#define MAX_TEXTURE_SLOTS " + std::to_string(textureSlots.size());
    frag_glsl.insert(first_newline, define_line);
    shader = OpenGL::CreateShader(std::string_view{ vertex_glsl }, std::string_view{ frag_glsl });

    GL::UseProgram(shader.Shader);

    std::vector<int> sampler_binding_values(textureSlots.size());
    std::iota(sampler_binding_values.begin(), sampler_binding_values.end(), 0);
    const GLint location = GL::GetUniformLocation(shader.Shader, "uTextures");
    GL::Uniform1iv(location, static_cast<GLsizei>(textureSlots.size()), sampler_binding_values.data());
    GL::UseProgram(0);

    // create our fixed buffer data
    //  create index buffer data
    // create our instanced buffer
    //  create VAO

    constexpr float fixed_sprite_vertices[] = { 
        // bottom left
        -0.5f, -0.5f, 0.0f, 0.0f,
        // bottom right
        0.5f, -0.5f, 1.0f, 0.0f,
        // top right
        0.5f, 0.5f, 1.0f, 1.0f,
        // top left
        -0.5f, 0.5f, 0.0f, 1.0f
    };
    constexpr unsigned char indicies[] = { 0, 1, 2, 0, 2, 3 };

    GL::GenBuffers(1, &fixedVertexBuffer);
    GL::BindBuffer(GL_ARRAY_BUFFER, fixedVertexBuffer);
    GL::BufferData(GL_ARRAY_BUFFER, sizeof(fixed_sprite_vertices), fixed_sprite_vertices, GL_STATIC_DRAW);
    GL::BindBuffer(GL_ARRAY_BUFFER, 0);

    GL::GenBuffers(1, &indexBuffer);
    GL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    GL::BufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);
    GL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GL::GenBuffers(1, &instanceBuffer);
    GL::BindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
    GL::BufferData(GL_ARRAY_BUFFER, sizeof(QuadInstance) * maxInstances, nullptr, GL_DYNAMIC_DRAW);
    GL::BindBuffer(GL_ARRAY_BUFFER, 0);

    GL::GenVertexArrays(1, &vertexArrayObject);
    GL::BindVertexArray(vertexArrayObject);
    GL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

    GL::BindBuffer(GL_ARRAY_BUFFER, fixedVertexBuffer);

    // Per-vertex attributes
    GL::EnableVertexAttribArray(0);
    GL::VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, nullptr);
    GL::VertexAttribDivisor(0, 0);

    GL::EnableVertexAttribArray(1);
    const ptrdiff_t texcoord_offset = 2 * sizeof(float);
    GL::VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, reinterpret_cast<void*>(texcoord_offset));
    GL::VertexAttribDivisor(1, 0);

    // Per-instance attributes
    GL::BindBuffer(GL_ARRAY_BUFFER, instanceBuffer);

    // Transform matrix row 0 (location 2)
    GL::EnableVertexAttribArray(2);
    const ptrdiff_t transformRow0_offset = offsetof(QuadInstance, transformRow0);
    GL::VertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(QuadInstance), reinterpret_cast<void*>(transformRow0_offset));
    GL::VertexAttribDivisor(2, 1);

    // Transform matrix row 1 (location 3)
    GL::EnableVertexAttribArray(3);
    const ptrdiff_t transformRow1_offset = offsetof(QuadInstance, transformRow1);
    GL::VertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(QuadInstance), reinterpret_cast<void*>(transformRow1_offset));
    GL::VertexAttribDivisor(3, 1);

    // Tint (location 4)
    GL::EnableVertexAttribArray(4);
    const ptrdiff_t tint_offset = offsetof(QuadInstance, tint);
    GL::VertexAttribPointer(4, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(QuadInstance), reinterpret_cast<void*>(tint_offset));
    GL::VertexAttribDivisor(4, 1);

    // Texture scale (location 5)
    GL::EnableVertexAttribArray(5);
    const ptrdiff_t tex_scale_offset = offsetof(QuadInstance, texScale);
    GL::VertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, sizeof(QuadInstance), reinterpret_cast<void*>(tex_scale_offset));
    GL::VertexAttribDivisor(5, 1);

    // Texture offset (location 6)
    GL::EnableVertexAttribArray(6);
    const ptrdiff_t tex_offset_offset = offsetof(QuadInstance, texOffset);
    GL::VertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, sizeof(QuadInstance), reinterpret_cast<void*>(tex_offset_offset));
    GL::VertexAttribDivisor(6, 1);

    // Texture index (location 7)
    GL::EnableVertexAttribArray(7);
    const ptrdiff_t tex_index_offset = offsetof(QuadInstance, textureIndex);
    GL::VertexAttribIPointer(7, 1, GL_INT, sizeof(QuadInstance), reinterpret_cast<void*>(tex_index_offset));
    GL::VertexAttribDivisor(7, 1);

    // SDF shape type (location 8)
    GL::EnableVertexAttribArray(8);
    const ptrdiff_t shape_type_offset = offsetof(QuadInstance, shapeType);
    GL::VertexAttribIPointer(8, 1, GL_INT, sizeof(QuadInstance), reinterpret_cast<void*>(shape_type_offset));
    GL::VertexAttribDivisor(8, 1);

    // SDF world size (location 9)
    GL::EnableVertexAttribArray(9);
    const ptrdiff_t world_size_offset = offsetof(QuadInstance, worldSize);
    GL::VertexAttribPointer(9, 2, GL_FLOAT, GL_FALSE, sizeof(QuadInstance), reinterpret_cast<void*>(world_size_offset));
    GL::VertexAttribDivisor(9, 1);

    // SDF line width (location 10)
    GL::EnableVertexAttribArray(10);
    const ptrdiff_t line_width_offset = offsetof(QuadInstance, lineWidth);
    GL::VertexAttribPointer(10, 1, GL_FLOAT, GL_FALSE, sizeof(QuadInstance), reinterpret_cast<void*>(line_width_offset));
    GL::VertexAttribDivisor(10, 1);

    // SDF line color (location 11)
    GL::EnableVertexAttribArray(11);
    const ptrdiff_t line_color_offset = offsetof(QuadInstance, lineColor);
    GL::VertexAttribPointer(11, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(QuadInstance), reinterpret_cast<void*>(line_color_offset));
    GL::VertexAttribDivisor(11, 1);

    GL::BindVertexArray(0);
    GL::BindBuffer(GL_ARRAY_BUFFER, 0);
    GL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void InstancedRenderer2D::Shutdown()
{
    GL::DeleteVertexArrays(1, &vertexArrayObject);
    GL::DeleteBuffers(1, &fixedVertexBuffer);
    GL::DeleteBuffers(1, &instanceBuffer);
    GL::DeleteBuffers(1, &indexBuffer);
    GL::DeleteProgram(shader.Shader);

    vertexArrayObject = 0;
    fixedVertexBuffer = 0;
    instanceBuffer    = 0;
    indexBuffer       = 0;
    shader.Shader     = 0;
}

void InstancedRenderer2D::BeginScene(const Math::TransformationMatrix& view_projection)
{
    viewProjection = view_projection;

    // Convert double matrix to float for OpenGL (column-major order)
    float matrixFloat[9];
    matrixFloat[0] = static_cast<float>(viewProjection[0][0]); // column 0
    matrixFloat[1] = static_cast<float>(viewProjection[1][0]);
    matrixFloat[2] = static_cast<float>(viewProjection[2][0]);

    matrixFloat[3] = static_cast<float>(viewProjection[0][1]); // column 1
    matrixFloat[4] = static_cast<float>(viewProjection[1][1]);
    matrixFloat[5] = static_cast<float>(viewProjection[2][1]);

    matrixFloat[6] = static_cast<float>(viewProjection[0][2]); // column 2
    matrixFloat[7] = static_cast<float>(viewProjection[1][2]);
    matrixFloat[8] = static_cast<float>(viewProjection[2][2]);

    GL::UseProgram(shader.Shader);
    GL::UniformMatrix3fv(shader.UniformLocations.at("uToNDC"), 1, GL_FALSE, matrixFloat);
    GL::UseProgram(0);

    startBatch();
}

void InstancedRenderer2D::EndScene()
{
    flush();
}

void InstancedRenderer2D::DrawQuad(const Math::TransformationMatrix& transform, OpenGL::TextureHandle texture, Math::vec2 texture_coord_bl, Math::vec2 texture_coord_tr, CS200::RGBA tintColor)
{
    if (instanceData.size() >= maxInstances)
    {
        flush();
    }

    // find texture slot
    int  tex_index = 0;
    bool found     = false;
    for (size_t i = 0; i < activeTexuresSize; ++i)
    {
        if (textureSlots[i] == texture)
        {
            found     = true;
            tex_index = static_cast<int>(i);
        }
    }
    if (!found)
    {
        if (activeTexuresSize >= textureSlots.size())
        {
            flush();
        }
        tex_index                       = static_cast<int>(activeTexuresSize);
        textureSlots[activeTexuresSize] = texture;
        ++activeTexuresSize;
    }

    float left   = static_cast<float>(texture_coord_bl.x);
    float bottom = static_cast<float>(texture_coord_bl.y);
    float right  = static_cast<float>(texture_coord_tr.x);
    float top    = static_cast<float>(texture_coord_tr.y);

    QuadInstance instance;

    // Texture mode
    instance.shapeType = -1;
    instance.textureIndex = tex_index;

    instance.texScale[0]  = right - left;
    instance.texScale[1]  = top - bottom;
    instance.texOffset[0] = left;
    instance.texOffset[1] = bottom;

    instance.transformRow0[0] = static_cast<float>(transform[0][0]);
    instance.transformRow0[1] = static_cast<float>(transform[0][1]);
    instance.transformRow0[2] = static_cast<float>(transform[0][2]);

    instance.transformRow1[0] = static_cast<float>(transform[1][0]);
    instance.transformRow1[1] = static_cast<float>(transform[1][1]);
    instance.transformRow1[2] = static_cast<float>(transform[1][2]);

    instance.tint[0] = static_cast<unsigned char>((tintColor & 0xff000000) >> 24); // Red
    instance.tint[1] = static_cast<unsigned char>((tintColor & 0x00ff0000) >> 16); // Green
    instance.tint[2] = static_cast<unsigned char>((tintColor & 0x0000ff00) >> 8);  // Blue
    instance.tint[3] = static_cast<unsigned char>((tintColor & 0x000000ff) >> 0);  // Alpha

    instance.worldSize[0] = 0.0f;
    instance.worldSize[1] = 0.0f;
    instance.lineWidth = 0.0f;
    instance.lineColor[0] = 0;
    instance.lineColor[1] = 0;
    instance.lineColor[2] = 0;
    instance.lineColor[3] = 0;

    instanceData.push_back(instance);
}

void InstancedRenderer2D::DrawCircle(const Math::TransformationMatrix& transform, CS200::RGBA fill_color, CS200::RGBA line_color, double line_width)
{
    DrawSDF(transform, fill_color, line_color, line_width, SDFShape::Circle);
}

void InstancedRenderer2D::DrawRectangle(const Math::TransformationMatrix& transform, CS200::RGBA fill_color, CS200::RGBA line_color, double line_width)
{
    DrawSDF(transform, fill_color, line_color, line_width, SDFShape::Rectangle);
}

void InstancedRenderer2D::DrawLine(const Math::TransformationMatrix& transform, Math::vec2 startPoint, Math::vec2 endPoint, CS200::RGBA line_color, double line_width)
{
    const auto line_transform = CS200::Renderer2DUtils::CalculateLineTransform(transform, startPoint, endPoint, line_width);
    DrawSDF(line_transform, line_color, line_color, line_width, SDFShape::Rectangle);
}

void InstancedRenderer2D::DrawLine(Math::vec2 start_point, Math::vec2 end_point, CS200::RGBA line_color, double line_width)
{
    DrawLine(Math::TransformationMatrix{}, start_point, end_point, line_color, line_width);
}

void InstancedRenderer2D::DrawSDF(const Math::TransformationMatrix& transform, CS200::RGBA fill_color, CS200::RGBA line_color, double line_width, SDFShape sdf_shape)
{
    if (instanceData.size() >= maxInstances)
    {
        flush();
    }

    QuadInstance instance;

    instance.shapeType = static_cast<int>(sdf_shape);
    instance.textureIndex = 0; 

    double scaleX = std::sqrt(transform[0][0] * transform[0][0] + transform[1][0] * transform[1][0]);
    double scaleY = std::sqrt(transform[0][1] * transform[0][1] + transform[1][1] * transform[1][1]);
    
    instance.worldSize[0] = static_cast<float>(scaleX);
    instance.worldSize[1] = static_cast<float>(scaleY);
    instance.lineWidth = static_cast<float>(line_width);

    instance.transformRow0[0] = static_cast<float>(transform[0][0]);
    instance.transformRow0[1] = static_cast<float>(transform[0][1]);
    instance.transformRow0[2] = static_cast<float>(transform[0][2]);

    instance.transformRow1[0] = static_cast<float>(transform[1][0]);
    instance.transformRow1[1] = static_cast<float>(transform[1][1]);
    instance.transformRow1[2] = static_cast<float>(transform[1][2]);

    instance.tint[0] = static_cast<unsigned char>((fill_color & 0xff000000) >> 24); // Red
    instance.tint[1] = static_cast<unsigned char>((fill_color & 0x00ff0000) >> 16); // Green
    instance.tint[2] = static_cast<unsigned char>((fill_color & 0x0000ff00) >> 8);  // Blue
    instance.tint[3] = static_cast<unsigned char>((fill_color & 0x000000ff) >> 0);  // Alpha

    instance.lineColor[0] = static_cast<unsigned char>((line_color & 0xff000000) >> 24); // Red
    instance.lineColor[1] = static_cast<unsigned char>((line_color & 0x00ff0000) >> 16); // Green
    instance.lineColor[2] = static_cast<unsigned char>((line_color & 0x0000ff00) >> 8);  // Blue
    instance.lineColor[3] = static_cast<unsigned char>((line_color & 0x000000ff) >> 0);  // Alpha

    instance.texScale[0] = 0.0f;
    instance.texScale[1] = 0.0f;
    instance.texOffset[0] = 0.0f;
    instance.texOffset[1] = 0.0f;

    instanceData.push_back(instance);
}

void InstancedRenderer2D::startBatch()
{
    instanceData.clear();
    activeTexuresSize = 0;
}

void InstancedRenderer2D::flush()
{
    if (instanceData.empty()) [[unlikely]]
        return;

    // update the instance buffer data
    GL::BindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
    GL::BufferSubData(GL_ARRAY_BUFFER, 0, sizeof(QuadInstance) * instanceData.size(), instanceData.data());
    GL::BindBuffer(GL_ARRAY_BUFFER, 0);

    // set texture samplers
    for (size_t i = 0; i < activeTexuresSize; ++i)
    {
        GL::ActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + i));
        GL::BindTexture(GL_TEXTURE_2D, textureSlots[i]);
    }

    GL::UseProgram(shader.Shader);
    GL::BindVertexArray(vertexArrayObject);
    GL::DrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr, static_cast<GLsizei>(instanceData.size()));

    GL::BindVertexArray(0);
    GL::UseProgram(0);
    GL::BindTexture(GL_TEXTURE_2D, 0);

    startBatch();
}