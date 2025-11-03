/**
 * \file
 * \author Rudy Castan
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#pragma once

#include "CS200/IRenderer2D.hpp"
#include "Engine/Matrix.hpp"
#include "OpenGL/Shader.hpp"
#include <array>
#include <vector>

class InstancedRenderer2D : public CS200::IRenderer2D
{
private:
    enum class SDFShape : uint8_t
    {
        Circle    = 0,
        Rectangle = 1,
    };

public:
    InstancedRenderer2D(unsigned max_sprites = 10'000);

    void Init() override;
    void Shutdown() override;
    void BeginScene(const Math::TransformationMatrix& view_projection) override;
    void EndScene() override;
    void DrawQuad(
        const Math::TransformationMatrix& transform, OpenGL::TextureHandle texture, Math::vec2 texture_coord_bl = Math::vec2{ 0.0, 0.0 }, Math::vec2 texture_coord_tr = Math::vec2{ 1.0, 1.0 },
        CS200::RGBA tintColor = CS200::WHITE) override;

    void DrawCircle(const Math::TransformationMatrix& transform, CS200::RGBA fill_color = CS200::CLEAR, CS200::RGBA line_color = CS200::WHITE, double line_width = 2.0) override;
    void DrawRectangle(const Math::TransformationMatrix& transform, CS200::RGBA fill_color = CS200::CLEAR, CS200::RGBA line_color = CS200::WHITE, double line_width = 2.0) override;
    void DrawLine(const Math::TransformationMatrix& transform, Math::vec2 startPoint, Math::vec2 endPoint, CS200::RGBA line_color = CS200::WHITE, double line_width = 2.0) override;
    void DrawLine(Math::vec2 start_point, Math::vec2 end_point, CS200::RGBA line_color = CS200::WHITE, double line_width = 2.0) override;
    void DrawSDF(const Math::TransformationMatrix& transform, CS200::RGBA fill_color, CS200::RGBA line_color, double line_width, SDFShape sdf_shape);

private:

    struct QuadInstance
    {
        float                        transformRow0[3]; // First row of 3x3 transformation matrix
        float                        transformRow1[3]; // Second row of 3x3 transformation matrix
        std::array<unsigned char, 4> tint{};
        float                        texScale[2];
        float                        texOffset[2];
        int                          textureIndex = 0;

        int           shapeType;
        float         worldSize[2];
        float         lineWidth;
        unsigned char lineColor[4];
    };

    std::vector<QuadInstance> instanceData{};
    OpenGL::CompiledShader    shader{};
    OpenGL::Handle            fixedVertexBuffer = 0, instanceBuffer = 0, indexBuffer = 0, vertexArrayObject = 0;

    unsigned maxInstances = 0;

    std::vector<OpenGL::Handle> textureSlots;
    size_t                      activeTexuresSize = 0;
    Math::TransformationMatrix  viewProjection;


private:
    void flush();
    void startBatch();
};