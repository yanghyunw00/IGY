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

namespace CS200
{

    class BatchRenderer2D : public IRenderer2D
    {
    public:

        // BatchRenderer2D() = default;

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

        enum class SDFShape : uint8_t
        {
            Circle    = 0,
            Rectangle = 1,
        };

        void DrawSDF(const Math::TransformationMatrix& transform, CS200::RGBA fill_color, CS200::RGBA line_color, double line_width, SDFShape sdf_shape);


    private:
        OpenGL::VertexArrayHandle vao{}; // Vertex Array Object
        OpenGL::BufferHandle      vbo{}; //  Vertex Buffer Object
        OpenGL::BufferHandle      ibo{}; //  Index Buffer Object
        OpenGL::BufferHandle      uniformBlock{};

        GLsizei indicesCount = 0;

        OpenGL::VertexArrayHandle sdf_vao{};
        OpenGL::BufferHandle      sdfVbo{};

        OpenGL::CompiledShader textureShader{};
        OpenGL::CompiledShader sdfShader{};

        OpenGL::BufferHandle       uboCamera{};
        Math::TransformationMatrix viewProjection;
    };
}
