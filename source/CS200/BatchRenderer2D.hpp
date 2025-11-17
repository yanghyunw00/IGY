/**
 * \file BatchRenderer2D.hpp
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
    class BatchRenderer2D : public IRenderer2D
    {
    public:
        BatchRenderer2D() : BatchRenderer2D(10'000)
        {
        }

        BatchRenderer2D(unsigned max_quads);

        BatchRenderer2D(const BatchRenderer2D& other) = delete;
        BatchRenderer2D(BatchRenderer2D&& other) noexcept;

        BatchRenderer2D& operator=(const BatchRenderer2D& other) = delete;
        BatchRenderer2D& operator=(BatchRenderer2D&& other) noexcept;

        ~BatchRenderer2D() override;

        void Init() override;
        void Shutdown() override;

        void Flush();
        void startBatch();

        void BeginScene(const Math::TransformationMatrix& view_projection) override;
        void EndScene() override;

        void DrawQuad(const Math::TransformationMatrix& transform, OpenGL::TextureHandle texture, Math::vec2 texture_coord_bl, Math::vec2 texture_coord_tr, CS200::RGBA tintColor) override;

    private:
        OpenGL::CompiledShader     batchShader{};
        Math::TransformationMatrix viewProjection;

        size_t MaxQuadCount   = 0;
        size_t MaxVertexCount = 0;
        size_t MaxIndexCount  = 0;
        size_t MaxTextures    = 64;

        struct Vertex
        {
            float       x, y;
            float       s, t;
            CS200::RGBA tint{};
            int         texIndex;
        };

        OpenGL::Handle QuadVA = 0;
        OpenGL::Handle QuadVB = 0;
        OpenGL::Handle QuadIB = 0;

        unsigned                    indexCount = 0;
        std::vector<Vertex>         vertexBase;
        Vertex*                     vertexPtr = nullptr;
        std::vector<OpenGL::Handle> textureSlots;
    };
}