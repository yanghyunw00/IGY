

#include "CS200/BatchRenderer2D.hpp"
#include "Engine/Path.hpp"
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
    BatchRenderer2D::BatchRenderer2D(unsigned max_quads)
    {
        maxVertices = max_quads * 4;
        maxIndices  = max_quads * 6;


        vertexData.resize(maxVertices);

        // for each quad there are normally 4 vertices and 6 indices (2 triangles)

    }

    void CS200::BatchRenderer2D::Init() // initiate the renderer
    {
        
        //Load the shaders
        const std::filesystem::path vertex_file   = assets::locate_asset("Assets/shaders/BatchRenderer2D/batch.vert");
        const std::filesystem::path fragment_file = assets::locate_asset("Assets/shaders/BatchRenderer2D/batch.frag");
        shader                                    = OpenGL::CreateShader(vertex_file, fragment_file);      

        Gl::GenBuffers(1, &vertexBuffer);


    }

    void CS200::BatchRenderer2D::Shutdown()
    {
    }

    void CS200::BatchRenderer2D::BeginScene(const Math::TransformationMatrix& view_projection)
    {
    }

    void CS200::BatchRenderer2D::EndScene()
    {
    }

    void CS200::BatchRenderer2D::DrawQuad(const Math::TransformationMatrix& transform, OpenGL::TextureHandle texture, Math::vec2 texture_coord_bl, Math::vec2 texture_coord_tr, CS200::RGBA tintColor)
    {
    }
}

