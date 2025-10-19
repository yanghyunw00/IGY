/**
 * \file GamePlayState.cpp
 * \author Hyunwoo Yang
 * \author
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

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
#include <imgui.h>
#include <iomanip>
#include <numbers>
#include <sstream>

void GamePlayState::Load()
{
    CS200::RenderingAPI::SetClearColor(0x252525FF);
    testTexture   = Dokev::LoadTexture("Assets/images/textures/IsoPeak.png");
    auto perlin   = Dokev::GenImagePerlinNoise(256, 256, 0, 0, 0.01f);
    auto perlin2  = Dokev::GenImagePerlinNoise(256, 256, 256, 256, 0.01f);
    noiseTexture  = Dokev::LoadTextureFromImage(perlin);
    noiseTexture2 = Dokev::LoadTextureFromImage(perlin2);

    map.ResetPerlinTexture();
    map.ResetIsoGridMapArray();

    camera.SetZoom(0.3f);
}

void GamePlayState::Update()
{
    [[maybe_unused]] const auto& environment = Engine::GetWindowEnvironment();
    mouse.Update();  // Add this to update mouse state each frame
    updateInput();
}

void GamePlayState::Draw() const
{
    CS200::RenderingAPI::Clear();
    auto& renderer2d = Engine::GetRenderer2D();

    auto windowSize = Engine::GetWindow().GetSize();
    auto ndcMatrix  = CS200::build_ndc_matrix(windowSize);

    auto cameraTransform = camera.GetViewMatrix();

    renderer2d.BeginScene(cameraTransform * ndcMatrix);


    map.IsometricDrawMap(index);


    // How to draw on window, not camera view?? -> shouldnt appply the matrix translation hmmmmmmm
    // 1. make override function withouth translation
    // 2. make teammate to solve
    // 3. make camera component well

    // Dokev::DrawRectangle(0,0,100,100, Dokev::ColorPalette::RED); // issue!  when we change the camera view

    testTexture.DrawEx(0, 0, 90.0f, 1.0f);

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


    if (mouse.MouseJustPressed(Dokev::Mouse::Button::Left))
    {
        index = (index - 1 + 4) % 4;
    }
    else if (mouse.MouseJustPressed(Dokev::Mouse::Button::Right))
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
