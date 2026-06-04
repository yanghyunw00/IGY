/**
 * \file SplashGame.cpp
 * \author Dongyun Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "SplashGame.hpp"
#include "CS200/NDC.hpp"
#include "CS200/RGBA.hpp"
#include "CS200/RenderingAPI.hpp"
#include "Engine/AudioManager.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Window.hpp"
#include "MainMenu.hpp"

SplashGame::SplashGame()
{
}

void SplashGame::Load()
{
    if (!loaded)
    {
        CS200::RenderingAPI::SetClearColor(CS200::BLACK);
        logoTexture = Dokev::LoadTexture("Assets/images/Futsal_manager_logo.png");
        CS230::AudioManager::Instance().PlaySFX("Assets/sounds/Whistlesound.mp3", 0);
        loaded = true;
    }
}

void SplashGame::Update(double dt)
{
    timercounter += dt;
    if (timercounter >= timerLimit)
    {
        Engine::GetGameStateManager().PopState();
        Engine::GetGameStateManager().PushState<MainMenu>();
    }
}

void SplashGame::Unload()
{
}

gsl::czstring SplashGame::GetName() const
{
    return "SplashGame";
}

void SplashGame::Draw() const
{
    if (!loaded)
        return;

    CS200::RenderingAPI::Clear();
    auto& renderer2d = Engine::GetRenderer2D();
    auto  windowSize = Engine::GetWindow().GetSize();
    auto  ndcMatrix  = CS200::build_ndc_matrix(windowSize);
    renderer2d.BeginScene(ndcMatrix);

    auto       size = logoTexture.GetSize();
    Math::vec2 pos  = { static_cast<double>(windowSize.x - size.x) / 2.0,
                        static_cast<double>(windowSize.y + size.y) / 2.0 };
    logoTexture.Draw(pos, 0xFFFFFFFF);

    renderer2d.EndScene();
}

void SplashGame::DrawImGui()
{
}
