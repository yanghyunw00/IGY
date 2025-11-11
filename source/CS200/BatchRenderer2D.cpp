

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

        GL::GenBuffers(1, &vertexBuffer);
        GL::BindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        GL::BufferData(GL_ARRAY_BUFFER, sizeof(QuadVertex)* maxVertices, nullptr, GL_DYNAMIC_DRAW);
        GL::BindBuffer(GL_ARRAY_BUFFER, 0);

        std::vector<unsigned> indice_values(maxIndices);
        unsigned              offset = 0;
        for (unsigned i = 0; i < maxIndices; i += 6)
        {

            
            indice_values[i + 0] = offset + 0;
            indice_values[i + 1] = offset + 1;
            indice_values[i + 2] = offset + 2;
            indice_values[i + 3] = offset + 2;
            indice_values[i + 4] = offset + 3;
            indice_values[i + 5] = offset + 0;
            offset += 4;
        };

        GL::GenBuffers(1, &indexBuffer);
        GL::BindBuffer(GL_ARRAY_BUFFER, indexBuffer);
        GL::BufferData(GL_ARRAY_BUFFER, sizeof(unsigned) * maxIndices, indice_values.data(), GL_DYNAMIC_DRAW);
        GL::BindBuffer(GL_ARRAY_BUFFER, 0);
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

