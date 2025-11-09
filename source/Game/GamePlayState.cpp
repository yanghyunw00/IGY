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
#include "Engine/Path.hpp"
#include "CS200/RenderingAPI.hpp"
#include "Dokev/DKColor.hpp"
#include "Dokev/DKImage.hpp"
#include "Dokev/DKShapes.hpp"
#include "Dokev/DKTexture.hpp"
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

    map.ResetPerlinTexture();
    map.ResetIsoGridMapArray();
    map.InitializePathFinding();

    camera.SetZoom(0.3f);

    try
    {
        smokeSprite = Dokev::AnimatedSprite(assets::locate_asset("Assets/images/Smoke.spt"));
    }
    catch (const std::exception& e)
    {
        printf("Failed to load smoke animation: %s\n", e.what());
    }

    smokeEmitter = std::make_unique<Dokev::ParticleEmitter>(assets::locate_asset("Assets/images/Smoke.spt"), 20);
    smokeEmitter->SetMaxLife(1.0); 

    m_goManager = std::make_unique<GameObjectManager>();

    human.reserve(100);
    human.push_back(Human(Math::fvec2(5.0f, 5.0f)));
    human.push_back(Human(Math::fvec2(78.0f, 13.0f)));;

    road = std::make_unique<Road>();

    // Initialize world editor
    worldEditor.Initialize(&map, &camera, &human, road.get(), &index, &immigrationTimer, &immigrationInterval);
}

void GamePlayState::Update()
{
    [[maybe_unused]] const auto& environment = Engine::GetWindowEnvironment();
    worldEditor.UpdateInput();
    const auto& input = Engine::GetInput();

    //나중에
    //m_goManager->Update(dt);
    
    
    smokeSprite.Update(environment.DeltaTime);

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
            Math::vec2 direction(0.0, 200.0);
            smokeEmitter->Emit(5, emit_pos, emit_vel, direction, 3.14159 / 4, 0.5);
            printf("Emitted particles at (%.1f, %.1f)! Active: %d\n", emit_pos.x, emit_pos.y, smokeEmitter->GetActiveCount());
        }
    }
    for (auto& h : human)
    {
        h.Update(static_cast<float>(environment.DeltaTime), &map);
    }

    // Auto immigration system
    immigrationTimer += static_cast<float>(environment.DeltaTime);
    if (immigrationTimer >= immigrationInterval)
    {
        immigrationTimer = 0.0f;
        human.push_back(Human::CreateImmigrant());
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

    map.IsometricDrawMap(index,selected_renderer);

    testTexture.DrawEx(0, 0, 90.0f, 1.0f);

    // Draw smoke animation at position (200, 200)
    Math::TransformationMatrix smokeTransform;
    smokeTransform = Math::TranslationMatrix(Math::vec2(200.0, 200.0));
    // smokeSprite.Draw(smokeTransform);

    // Draw particles
    if (smokeEmitter)
    {
        smokeEmitter->Draw(cameraTransform * ndcMatrix);
    }
    
    // Draw human
    int currentView = (index % 4) + 1;
    for (auto& h : human)
    {
        h.Draw(currentView, &map);
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
