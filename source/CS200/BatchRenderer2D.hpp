/**
 * \file
 * \author Rudy Castan
 * \author JUNSEOK LEE
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#pragma once

#include "Engine/Matrix.hpp"
#include "IRenderer2D.hpp"
#include "OpenGL/Shader.hpp"
#include <filesystem>
#include <cstdint>
#include "OpenGL/VertexArray.hpp"
#include <array>
#include <vector>

namespace CS200
{

    class BatchRenderer2D : public IRenderer2D
    {
    public:

           BatchRenderer2D(unsigned max_quads = 10000); // get the max numbers in the constructor


        // BatchRenderer2D(const BatchRenderer2D& other) = delete;

        // BatchRenderer2D(BatchRenderer2D&& other) noexcept;

        // BatchRenderer2D& operator=(const BatchRenderer2D& other) = delete;

        // BatchRenderer2D& operator=(BatchRenderer2D&& other) noexcept;

        // ~BatchRenderer2D() override;

        void Init() override;

        void Shutdown() override;

        void BeginScene(const Math::TransformationMatrix& view_projection) override;

        void EndScene() override;

        void DrawQuad(const Math::TransformationMatrix& transform, OpenGL::TextureHandle texture, Math::vec2 texture_coord_bl, Math::vec2 texture_coord_tr, CS200::RGBA tintColor) override;

   

    private:

        struct QuadVertex
        {
            float                        x = 0, y = 0;
            float                        s = 0, t = 0;
            //float                        model[9];
            std::array<unsigned char, 4> tint{};
        };

        std::vector<QuadVertex> vertexData;
        OpenGL::CompiledShader  shader{};
        OpenGL::Handle          vertexBuffer = 0, indexBuffer = 0, vertexArrayObject = 0;

        unsigned maxVertices = 0; // limit for max vertices in a single batch
        unsigned maxIndices  = 0;

        QuadVertex* vertexDataEnd = nullptr; // where are we in the current vertex buffer
        unsigned    indexCount    = 0;
           
        void flush();

        void startBatch();

    };
}
