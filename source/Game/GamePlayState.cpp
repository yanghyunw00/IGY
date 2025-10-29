/**
 * \file GamePlayState.cpp
 * \author Hyunwoo Yang
 * \author
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include <GL/glew.h>
#include "GamePlayState.hpp"
#include "CS200/IRenderer2D.hpp"
#include "CS200/NDC.hpp"
#include "CS200/RGBA.hpp"
#include "CS200/RenderingAPI.hpp"
#include "Dokev/DKImage.hpp"
#include "Dokev/DKShapes.hpp"
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
#include "OpenGL/GL.hpp" // added, might be aping the code.
void GamePlayState::Load()
{
    GL::Enable(GL_STENCIL_TEST); //enable for mouse clicking.
    GL::ClearStencil(0); // reset. might have leftover shit from other gamestates
    CS200::RenderingAPI::SetClearColor(0x252525FF);
    testTexture   = Dokev::LoadTexture("Assets/images/textures/IsoPeak.png");
    auto perlin   = Dokev::GenImagePerlinNoise(256, 256, 0, 0, 0.01f);
    auto perlin2  = Dokev::GenImagePerlinNoise(256, 256, 256, 256, 0.01f);
    noiseTexture  = Dokev::LoadTextureFromImage(perlin);
    noiseTexture2 = Dokev::LoadTextureFromImage(perlin2);

    map.ResetPerlinTexture();
    map.ResetIsoGridMapArray();

    camera.SetZoom(0.3f);

    // Load smoke animation
    try
    {
        smokeSprite = Dokev::AnimatedSprite("Assets/images/Smoke.spt");
    }
    catch (const std::exception& e)
    {
        printf("Failed to load smoke animation: %s\n", e.what());
    }

    smokeEmitter = std::make_unique<Dokev::ParticleEmitter>("Assets/images/Smoke.spt", 20);
    smokeEmitter->SetMaxLife(1.0); // Particles live for 3 seconds
}

void GamePlayState::Update()
{


    [[maybe_unused]] const auto& environment = Engine::GetWindowEnvironment();
    mouse.Update(); 
    updateInput();
    const auto& input       = Engine::GetInput();

    // Update smoke animation
    smokeSprite.Update(environment.DeltaTime);

    // Update particles
    if (smokeEmitter)
    {
        smokeEmitter->Update(environment.DeltaTime);
    }

    // Emit particles when pressing p
    if (input.KeyJustPressed(CS230::Input::Keys::P))
    {
        if (smokeEmitter)
        {
            // Emit particles at camera center (world space)
            Math::vec2 emit_pos = camera.GetPosition();
            Math::vec2 emit_vel(0.0, 0.0);
            Math::vec2 direction(0.0, 200.0);  // Upward (larger value because of camera zoom)
            smokeEmitter->Emit(5, emit_pos, emit_vel, direction, 3.14159 / 4, 0.5);
            printf("Emitted particles at (%.1f, %.1f)! Active: %d\n",
                   emit_pos.x, emit_pos.y, smokeEmitter->GetActiveCount());
        }
    }
}

void GamePlayState::Draw() const
{
    CS200::RenderingAPI::Clear();
    auto& renderer2d = Engine::GetRenderer2D();

    auto windowSize = Engine::GetWindow().GetSize();
    auto ndcMatrix  = CS200::build_ndc_matrix(windowSize);

    auto cameraTransform = camera.GetViewMatrix();

    renderer2d.BeginScene(cameraTransform * ndcMatrix);

    GL::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    map.IsometricDrawMap(index);


    // How to draw on window, not camera view?? -> shouldnt appply the matrix translation hmmmmmmm
    // 1. make override function withouth translation
    // 2. make teammate to solve
    // 3. make camera component well

    // Dokev::DrawRectangle(0,0,100,100, Dokev::ColorPalette::RED); // issue!  when we change the camera view

    testTexture.DrawEx(0, 0, 90.0f, 1.0f);

    // Draw smoke animation at position (200, 200)
    Math::TransformationMatrix smokeTransform;
    smokeTransform = Math::TranslationMatrix(Math::vec2(200.0, 200.0));
    //smokeSprite.Draw(smokeTransform);

    // Draw particles
    if (smokeEmitter)
    {
        smokeEmitter->Draw(cameraTransform * ndcMatrix);
    }

    renderer2d.EndScene();
}

void GamePlayState::DrawImGui()
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

        if (ImGui::Button("Roatate Map to Left"))
        {
            index = (index + 1) % 4;
        }

        ImGui::Text("Rotate Index: %d", index);
         const auto &env = Engine::GetWindowEnvironment();
        ImGui::Text("FPS: %d", env.FPS);
        ImGui::Text("Frame time: %.2f ms", static_cast<float>(env.DeltaTime * 1000.0));

    }
    ImGui::End();
}

void GamePlayState::updateInput()
{
    const auto& input       = Engine::GetInput();
    const auto& environment = Engine::GetWindowEnvironment();


    // Re gen
    if (input.KeyJustPressed(CS230::Input::Keys::Space))
    {
        map.ReGenerateMap();
    }

    // for future road, human
    // int currentView = (index % 4) + 1;


    // Map zoomin zoom out
    if (input.KeyDown(CS230::Input::Keys::Q))
        camera.AdjustZoom(0.9f);
    if (input.KeyDown(CS230::Input::Keys::E))
        camera.AdjustZoom(1.1f);

    // Limit
    camera.ClampZoom(0.1f, 5.0f);

    if (input.KeyDown(CS230::Input::Keys::A))
    {
        camera.Move({ 1 * environment.DeltaTime, 0 });
    }
    if (input.KeyDown(CS230::Input::Keys::D))
    {
        camera.Move({ -1 * environment.DeltaTime, 0 });
    }
    if (input.KeyDown(CS230::Input::Keys::W))
    {
        camera.Move({ 0, -1 * environment.DeltaTime });
    }
    if (input.KeyDown(CS230::Input::Keys::S))
    {
        camera.Move({ 0, 1 * environment.DeltaTime });
    }
    // reset
    if (input.KeyDown(CS230::Input::Keys::R))
    {
        camera.Reset();
    }

    // if (mouse.MouseJustPressed(Dokev::Mouse::Button::Left))
    // {
    //     //int x = stencil_value & 0xFF; int y = (stencil_value >> 8) & 0xFF;
    //     int stencil_value;
    //     glReadPixels(mouse.GetPosition().x,mouse.GetPosition().y, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &stencil_value);
    //     int x = stencil_value & 0xFF; int y = (stencil_value >> 8) & 0xFF;
    //     std::cout << "x : " << x << " y : " << y << std::endl;
    // }

    if (input.KeyDown(CS230::Input::Keys::Z))
    {
        index = (index - 1 + 4) % 4;
    }
    else if (input.KeyDown(CS230::Input::Keys::C))
    {
        index = (index + 1) % 4;
    }
}

void GamePlayState::Unload()
{
    Engine::GetLogger().LogEvent("GamePlayState: Unloaded");
}

gsl::czstring GamePlayState::GetName() const
{
    return "IGY";
}
