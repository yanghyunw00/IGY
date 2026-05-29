/**
 * \file InstancedRenderer2D.hpp
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#pragma once

#include "Engine/Matrix.hpp"
#include "IRenderer2D.hpp"
#include "OpenGL/Shader.hpp"
#include "OpenGL/VertexArray.hpp"
#include <array>
#include <cstdint>
#include <vector>

namespace CS200
{
    class InstancedRenderer2D : public IRenderer2D
    {
    private:
        enum class SDFShape : uint8_t
        {
            Circle    = 0,
            Rectangle = 1,
        };

    public:
        InstancedRenderer2D() : InstancedRenderer2D(10'000)
        {
        }

        InstancedRenderer2D(unsigned max_quads);

        InstancedRenderer2D(const InstancedRenderer2D& other) = delete;
        InstancedRenderer2D(InstancedRenderer2D&& other) noexcept;

        InstancedRenderer2D& operator=(const InstancedRenderer2D& other) = delete;
        InstancedRenderer2D& operator=(InstancedRenderer2D&& other) noexcept;

        ~InstancedRenderer2D() override;

        void Init() override;
        void Shutdown() override;

        void Flush();
        void startBatch();

        void BeginScene(const Math::TransformationMatrix& view_projection) override;
        void EndScene() override;

        void DrawQuad(const Math::TransformationMatrix& transform, OpenGL::TextureHandle texture, Math::vec2 texture_coord_bl, Math::vec2 texture_coord_tr, CS200::RGBA tintColor) override;

        void DrawCircle(const Math::TransformationMatrix& transform, CS200::RGBA fill_color = CS200::CLEAR, CS200::RGBA line_color = CS200::WHITE, double line_width = 2.0) override ;
        void DrawRectangle(const Math::TransformationMatrix& transform, CS200::RGBA fill_color = CS200::CLEAR, CS200::RGBA line_color = CS200::WHITE, double line_width = 2.0) override;
        void DrawLine(const Math::TransformationMatrix& transform, Math::vec2 startPoint, Math::vec2 endPoint, CS200::RGBA line_color = CS200::WHITE, double line_width = 2.0) override;
        void DrawLine(Math::vec2 start_point, Math::vec2 end_point, CS200::RGBA line_color = CS200::WHITE, double line_width = 2.0) override;
        void DrawSDF(const Math::TransformationMatrix& transform, CS200::RGBA fill_color, CS200::RGBA line_color, double line_width, SDFShape sdf_shape);


    private:
        OpenGL::CompiledShader     instancedShader{};
        Math::TransformationMatrix viewProjection;

        size_t MaxQuadCount = 0;
        size_t MaxTextures  = 64;

        struct InstanceData
        {
            float                        transformRow0[3];     // aTransformRow0 (location 1)
            float                        transformRow1[3];     // aTransformRow1 (location 2)
            float                        transformRow2[3];     // aTransformRow2 (location 3)
            float                        texCoord0[2];         // aTexCoord0 (location 4)
            float                        texCoord1[2];         // aTexCoord1 (location 5)
            float                        texCoord2[2];         // aTexCoord2 (location 6)
            float                        texCoord3[2];         // aTexCoord3 (location 7)
            std::array<unsigned char, 4> tint{};              // aTint (location 8)
            int                          textureIndex;         // aTextureIndex (location 9)
            int                          shapeType;            // aShapeType (location 10)
            float                        worldSize[2];         // aWorldSize (location 11)
            float                        lineWidth;            // aLineWidth (location 12)
            std::array<unsigned char, 4> lineColor{};         // aLineColor (location 13)
        };

        OpenGL::Handle QuadVA     = 0;
        OpenGL::Handle QuadVB     = 0;
        OpenGL::Handle QuadIB     = 0;
        OpenGL::Handle InstanceVB = 0;

        std::vector<InstanceData>   instanceData;
        InstanceData*               instanceDataEnd = nullptr;
        std::vector<OpenGL::Handle> textureSlots;
    };
}