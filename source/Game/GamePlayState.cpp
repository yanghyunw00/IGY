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
    testTexture = Dokev::LoadTexture("Assets/images/textures/IsoPeak.png");
    auto perlin = Dokev::GenImagePerlinNoise(256, 256, 0, 0, 0.01f);
    auto perlin2 = Dokev::GenImagePerlinNoise(256, 256, 256, 256, 0.01f);
    noiseTexture = Dokev::LoadTextureFromImage(perlin);
    noiseTexture2 = Dokev::LoadTextureFromImage(perlin2);

    map.ResetPerlinTexture();
    map.ResetIsoGridMapArray();
}

void GamePlayState::Update()
{
    [[maybe_unused]] const auto& environment = Engine::GetWindowEnvironment();
    updateInput();

}

void GamePlayState::Draw() const
{
    CS200::RenderingAPI::Clear();
    auto& renderer2d = Engine::GetRenderer2D();

    auto windowSize = Engine::GetWindow().GetSize();
    auto ndcMatrix = CS200::build_ndc_matrix(windowSize);

    auto cameraTransform = Math::TranslationMatrix(cameraPosition) *
                           Math::ScaleMatrix({cameraZoom, cameraZoom});

    renderer2d.BeginScene(cameraTransform * ndcMatrix);

    draw();

    map.IsometricDrawMap(1);
    //Dokev::DrawRectangle(0,0,100,100, Dokev::ColorPalette::RED); // issue!  when we change the camera view  
    renderer2d.EndScene();
}

void GamePlayState::DrawImGui()
{
    // if (ImGui::Begin("Controller"))
    //{
    //     //nothing
    // }
    // ImGui::End();
}

void GamePlayState::Unload()
{
    Engine::GetLogger().LogEvent("GamePlayState: Unloaded");
}

gsl::czstring GamePlayState::GetName() const
{
    return "IGY";
}

void GamePlayState::updateInput()
{
    const auto& input       = Engine::GetInput();
    const auto& environment = Engine::GetWindowEnvironment();

    if (input.KeyDown(CS230::Input::Keys::Q))
        cameraZoom *= 0.9f; 
    if (input.KeyDown(CS230::Input::Keys::E))
        cameraZoom *= 1.1f; 

    if (cameraZoom < 0.1f) cameraZoom = 0.1f;
    if (cameraZoom > 5.0f) cameraZoom = 5.0f;

    if (input.KeyDown(CS230::Input::Keys::Left) || input.KeyDown(CS230::Input::Keys::A))
        cameraPosition.x += 1 *environment.DeltaTime;
    if (input.KeyDown(CS230::Input::Keys::Right) || input.KeyDown(CS230::Input::Keys::D))
        cameraPosition.x -= 1 *environment.DeltaTime;
    if (input.KeyDown(CS230::Input::Keys::Up) || input.KeyDown(CS230::Input::Keys::W))
        cameraPosition.y -= 1 *environment.DeltaTime;
    if (input.KeyDown(CS230::Input::Keys::Down) || input.KeyDown(CS230::Input::Keys::S))
        cameraPosition.y += 1 *environment.DeltaTime;

    if (input.KeyDown(CS230::Input::Keys::R))
    {
        cameraPosition = {0, 0};
        cameraZoom = 1.0f;
    }
}

void GamePlayState::draw() const
{
    // Dokev::DrawPixel(1, 1, Dokev::ColorPalette::RED);
    // testTexture.Draw(0, 0);
    // testTexture.Draw(256, 256);
    // testTexture.Draw(100, 100);

    // Dokev::DrawTexture(noiseTexture, 300, 100, Dokev::ColorPalette::WHITE);
    // Dokev::DrawTexture(noiseTexture2, 500, 100, Dokev::ColorPalette::WHITE);

    // Dokev::DrawRectangleLines(30, 400, 325, 30, Dokev::ColorPalette::WHITE);

}
