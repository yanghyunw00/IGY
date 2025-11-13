/**
 * \file GamePlayState.cpp
 * \author JUNSEOK LEE
 * \author
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include <GL/glew.h>
#include "DEMO.hpp"
#include "CS200/IRenderer2D.hpp"
#include "CS200/NDC.hpp"
#include "CS200/RGBA.hpp"
#include "CS200/RenderingAPI.hpp"
#include "CS200/InstancedRenderer2D.hpp"
#include "Dokev/DKImage.hpp"
#include "Dokev/DKShapes.hpp"
#include "Dokev/DKColor.hpp"
#include "Dokev/DKTexture.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Input.hpp"
#include "Engine/Logger.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Window.hpp"
#include <cmath>
#include <filesystem>
#include <imgui.h>
#include <iomanip>
#include <numbers>
#include <sstream>
#include "OpenGL/GL.hpp" 

void DEMO::Load()
{
    CS200::RenderingAPI::SetClearColor(0x252525FF);

    auto perlin   = Dokev::GenImagePerlinNoise(256, 256, 0, 0, 0.01f);
    auto perlin2  = Dokev::GenImagePerlinNoise(256, 256, 256, 256, 0.01f);
    noiseTexture  = Dokev::LoadTextureFromImage(perlin);
    noiseTexture2 = Dokev::LoadTextureFromImage(perlin2);

    map.ResetPerlinTexture();
    map.ResetIsoGridMapArray();
    map.InitializePathFinding();

    camera.SetZoom(0.3f);

}


void DEMO::Update(double dt)
{

    [[maybe_unused]] const auto& environment = Engine::GetWindowEnvironment();

    updateInput();

}


void DEMO::Draw() const
{
    CS200::RenderingAPI::Clear();
    // use the currently selected renderer (allows IMMEDIATE / INSTANCED switching)
    auto& renderer2d = Engine::GetRenderer2D(selected_renderer);

    auto& renderer2d_IMMEDIATE = Engine::GetRenderer2D(Dokev::RenderState::IMMEDIATE);

    auto windowSize = Engine::GetWindow().GetSize();
    auto ndcMatrix  = CS200::build_ndc_matrix(windowSize);

    auto cameraTransform = camera.GetViewMatrix();

    renderer2d.BeginScene(cameraTransform * ndcMatrix);

    renderer2d_IMMEDIATE.BeginScene(cameraTransform * ndcMatrix);

    GL::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    
    if(show_atlas)
    {
        map.IsometricDrawMouseAtlas(index,selected_renderer);
    }
    else
    {
        map.IsometricDrawMap(index,selected_renderer);
    }
    renderer2d.EndScene();
    renderer2d_IMMEDIATE.EndScene();
}


void DEMO::DrawImGui()
{
    if (ImGui::Begin("Controller"))
    {
        float zoom = camera.GetZoom();
        if (ImGui::SliderFloat("Zoom", &zoom, 0.1f, 1.0f))
        {
            camera.SetZoom(zoom);
        }

        ImGui::Separator();

        if (ImGui::Button("Rotate to Map right"))
        {
            index = (index - 1 + 4) % 4;
        }

        if (ImGui::Button("Rotate Map to Left"))
        {
            index = (index + 1) % 4;
        }
        if (ImGui::Button("show mouse atlas"))
        {
            show_atlas = true;
        }
        if (ImGui::Button("dont show mouse atlas"))
        {
            show_atlas = false;
        }

        ImGui::Text("Rotate Index: %d", index);
         const auto &env = Engine::GetWindowEnvironment();
        ImGui::Text("FPS: %d", env.FPS);
        ImGui::Text("Frame time: %.2f ms", static_cast<float>(env.DeltaTime * 1000.0));

       

        ImGui::Separator();
        ImGui::Text("select renderer");
        if (ImGui::Button("INSTANCED"))
        {
            SwitchRenderer(Dokev::RenderState::INSTANCED);
        }
         if (ImGui::Button("IMMEDIATE"))
        {
            SwitchRenderer(Dokev::RenderState::IMMEDIATE);
        }

    }
    ImGui::End();
}

void DEMO::SwitchRenderer(Dokev::RenderState type)
{
    if (selected_renderer == type) // if we switched to the same type
        return;

    if (customRenderer)
    {
        customRenderer->Shutdown();
        customRenderer.reset();
    }

    selected_renderer = type;

    switch (type)
    {
        case Dokev::RenderState::IMMEDIATE: break;

        case Dokev::RenderState::INSTANCED:
            customRenderer = std::make_unique<CS200::InstancedRenderer2D>();
            customRenderer->Init();
            break;
    }
}

void DEMO::updateInput()
{
    const auto& input       = Engine::GetInput();
    const auto& environment = Engine::GetWindowEnvironment();


    // Re gen
    if (input.KeyJustPressed(CS230::Input::Keys::Space))
    {
        map.ReGenerateMap();
        map.InitializePathFinding();
        map.ClearPath();
        printf("Map regenerated and pathfinding reinitialized\n");
    }

    // for future road, human
    // int currentView = (index % 4) + 1;


    if (input.KeyDown(CS230::Input::Keys::Q))
        camera.AdjustZoom(0.95f);
    if (input.KeyDown(CS230::Input::Keys::E))
        camera.AdjustZoom(1.05f);

    camera.ClampZoom(0.1f, 5.0f);

    constexpr double camera_speed = 1.0;

    if (input.KeyDown(CS230::Input::Keys::A))
    {
        camera.Move({ camera_speed * environment.DeltaTime, 0 });
    }
    if (input.KeyDown(CS230::Input::Keys::D))
    {
        camera.Move({ -camera_speed * environment.DeltaTime, 0 });
    }
    if (input.KeyDown(CS230::Input::Keys::W))
    {
        camera.Move({ 0, -camera_speed * environment.DeltaTime });
    }
    if (input.KeyDown(CS230::Input::Keys::S))
    {
        camera.Move({ 0, camera_speed * environment.DeltaTime });
    }
    // reset
    if (input.KeyDown(CS230::Input::Keys::R))
    {
        camera.Reset();
    }
    if (input.KeyJustPressed(CS230::Input::Keys::Z))
    {
        index = (index - 1 + 4) % 4;
    }
    else if (input.KeyJustPressed(CS230::Input::Keys::C))
    {
        index = (index + 1) % 4;
    }
    mouse.Update();
    if (mouse.MouseJustPressed(Dokev::Mouse::Button::Left))
    {

        map.IsometricDrawMouseAtlas(index, selected_renderer);// to update the atlas

        GLubyte pixel[4] = { 0 };
        glBindFramebuffer(GL_FRAMEBUFFER, map.mouseAtlasFBO);

        int         windowHeight = 800;
        Math::ivec2 mouseScreen  = mouse.GetPosition();
        mouseScreen.y            = windowHeight - mouseScreen.y;

        glReadPixels(static_cast<int>(mouseScreen.x), static_cast<int>(mouseScreen.y), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        float tileX = pixel[0] * static_cast<float>(GRID_SIZE_X - 1 ) / 255.f;
        float tileY = pixel[1] * static_cast<float>(GRID_SIZE_Y - 1 ) / 255.f;

        
        std::cout << "Clicked tile (" << std::ceil(tileX) << ", " << std::ceil(tileY) << ")\n";
    }



}

void DEMO::Unload()
{
    Engine::GetLogger().LogEvent("DEMO: Unloaded");
}

gsl::czstring DEMO::GetName() const
{
    return "IGYDEMO";
}
