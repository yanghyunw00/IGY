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
#include "Dokev/DKColor.hpp"
#include "Dokev/DKImage.hpp"
#include "Dokev/DKParticles.hpp"
#include "Dokev/DKShapes.hpp"
#include "Dokev/DKTexture.hpp"
#include "Dokev/DKGameObjectManager.hpp"
#include "Human.hpp"
#include "GridMap.hpp"

#include "Engine/Path.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Input.hpp"
#include "Engine/Logger.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Window.hpp"
#include "OpenGL/GL.hpp"
#include <GL/glew.h>
#include <SDL.h>
#include <cmath>
#include <filesystem>
#include <imgui.h>
#include <iomanip>
#include <numbers>
#include <sstream>

void GamePlayState::Load()
{
    CS200::RenderingAPI::SetClearColor(0x252525FF);

    auto perlin   = Dokev::GenImagePerlinNoise(256, 256, 0, 0, 0.01f);
    auto perlin2  = Dokev::GenImagePerlinNoise(256, 256, 256, 256, 0.01f);
    noiseTexture  = Dokev::LoadTextureFromImage(perlin);
    noiseTexture2 = Dokev::LoadTextureFromImage(perlin2);

    // Initialize MapManager component
    AddGSComponent<MapManager>();
    GetGSComponent<MapManager>()->Initialize();

    // Initialize GameObjectManager component
    AddGSComponent<Dokev::GameObjectManager>();
    GetGSComponent<Dokev::GameObjectManager>()->AddHuman(new Human(Math::fvec2(5.0f, 5.0f)));
    GetGSComponent<Dokev::GameObjectManager>()->AddHuman(new Human(Math::fvec2(78.0f, 13.0f)));

    camera.SetZoom(0.3f);

    try
    {
        smokeSprite = Dokev::AnimatedSprite(assets::locate_asset("Assets/images/Smoke.spt"));
    }
    catch (const std::exception& e)
    {
        printf("Failed to load smoke animation: %s\n", e.what());
    }

    worldEditor.Initialize(GetGSComponent<MapManager>().get(), &camera, &index,
                          &immigrationTimer, &immigrationInterval,
                          GetGSComponent<Dokev::GameObjectManager>().get());
}

void GamePlayState::Update(double dt)
{
    [[maybe_unused]] const auto& environment = Engine::GetWindowEnvironment();
    worldEditor.UpdateInput();
    const auto& input = Engine::GetInput();

    UpdateGSComponents(dt);

    // Update all humans
    auto mapMgr = GetGSComponent<MapManager>();
    auto& humans = GetGSComponent<Dokev::GameObjectManager>()->GetHumans();
    for (auto& human : humans)
    {
        human->Update(static_cast<float>(dt), mapMgr->GetMap(), mapMgr->GetRoad());
    }
    GetGSComponent<Dokev::GameObjectManager>()->Refresh();

    smokeEmitter.Update(environment.DeltaTime);

    if (input.KeyJustPressed(CS230::Input::Keys::P))
    {
        Math::vec2 emit_pos = camera.GetPosition();
        Math::vec2 emit_vel(0.0, 0.0);
        Math::vec2 direction(0.0, 200.0);
        smokeEmitter.Emit(5, emit_pos, emit_vel, direction, 3.14159 / 4, 0.5);
    }

    // Auto immigration system
    immigrationTimer += static_cast<float>(dt);
    if (immigrationTimer >= immigrationInterval)
    {
        immigrationTimer = 0.0f;
        //GetGSComponent<Dokev::GameObjectManager>()->AddHuman(new Human(Human::CreateImmigrant()));
    }
}

void GamePlayState::Draw() const
{
    CS200::RenderingAPI::Clear();
    auto& renderer2d = Engine::GetRenderer2D(selected_renderer);

    auto windowSize = Engine::GetWindow().GetSize();
    auto ndcMatrix  = CS200::build_ndc_matrix(windowSize);

    auto cameraTransform = camera.GetViewMatrix();

    renderer2d.BeginScene(cameraTransform * ndcMatrix);

    GL::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    auto mapMgr = GetGSComponent<MapManager>();
    mapMgr->GetMap()->IsometricDrawMap(index, selected_renderer);

    testTexture.DrawEx(0, 0, 90.0f, 1.0f);

    // Draw smoke animation at position (200, 200)
    Math::TransformationMatrix smokeTransform;
    smokeTransform = Math::TranslationMatrix(Math::vec2(200.0, 200.0));
    // smokeSprite.Draw(smokeTransform);

    // Draw particles
    smokeEmitter.Draw(cameraTransform * ndcMatrix);

    // Draw all humans
    auto& humans = GetGSComponent<Dokev::GameObjectManager>()->GetHumans();
    for (const auto& human : humans)
    {
        human->Draw(index, mapMgr->GetMap());
    }

    renderer2d.EndScene();
}

void GamePlayState::DrawImGui()
{
    worldEditor.DrawImGui();
}

void GamePlayState::Unload()
{
    Engine::GetLogger().LogEvent("GamePlayState: Unloaded");
}

gsl::czstring GamePlayState::GetName() const
{
    return "IGY";
}
