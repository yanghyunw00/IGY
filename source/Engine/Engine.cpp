/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "Engine.hpp"
#include "CS200/ImGuiHelper.hpp"
#include "CS200/InstancedRenderer2D.hpp"
#include "CS200/ImmediateRenderer2D.hpp"

#include "CS200/NDC.hpp"
#include "CS200/RenderingAPI.hpp"
#include "AudioManager.hpp"
#include "FPS.hpp"
#include "Font.hpp"
#include "GameState.hpp"
#include "GameStateManager.hpp"
#include "Input.hpp"
#include "Logger.hpp"
#include "TextureManager.hpp"
#include "Timer.hpp"
#include "Window.hpp"

#include <chrono>

// Pimpl implementation class
class Engine::Impl
{
public:
    Impl()
        :
#ifdef DEVELOPER_VERSION
          logger(CS230::Logger::Severity::Debug, true, std::chrono::system_clock::now())
#else
          logger(CS230::Logger::Severity::Event, false, std::chrono::system_clock::now())
#endif
          ,
          window{}, input{}
    {
    }

    CS230::Logger              logger;
    CS230::Window              window{};
    CS230::Input               input{};
    ImGuiHelper::Viewport      viewport{};
    util::FPS                  fps{};
    util::Timer                timer{};
    WindowEnvironment          environment{};
    CS230::GameStateManager    gameStateManager{};
    CS200::InstancedRenderer2D InstancedRenderer2D{};
    CS200::ImmediateRenderer2D ImmediateRenderer2D{};
    CS230::TextureManager      textureManager{};
};

Engine& Engine::Instance()
{
    static Engine instance;
    return instance;
}

CS230::Logger& Engine::GetLogger()
{
    return Instance().impl->logger;
}

CS230::Window& Engine::GetWindow()
{
    return Instance().impl->window;
}

CS230::Input& Engine::GetInput()
{
    return Instance().impl->input;
}

const WindowEnvironment& Engine::GetWindowEnvironment()
{
    return Instance().impl->environment;
}

CS230::GameStateManager& Engine::GetGameStateManager()
{
    return Instance().impl->gameStateManager;
}

CS230::TextureManager& Engine::GetTextureManager()
{
    return Instance().impl->textureManager;
}
CS200::IRenderer2D& Engine::GetRenderer2D(Dokev::RenderState selection)
{
    switch (selection)
    {
        case Dokev::RenderState::IMMEDIATE:
        return Instance().impl->ImmediateRenderer2D;
        case Dokev::RenderState::INSTANCED:
        return Instance().impl->InstancedRenderer2D;
    }
    Instance().impl->logger.LogError("Reached the end of switch statement without returning at GetRenderer2D in Engine.cpp line 87");
       return Instance().impl->InstancedRenderer2D;
}

void Engine::Start(std::string_view window_title)
{
    impl->logger.LogEvent("Engine Started");
#if defined(DEVELOPER_VERSION)
    impl->logger.LogEvent("Developer Build");
#endif
    impl->window.Start(window_title);
    auto& window = impl->window;

    const auto window_size = window.GetSize();
    impl->viewport         = { 0, 0, window_size.x, window_size.y };
    CS200::RenderingAPI::SetViewport(window_size);
    impl->environment.DisplaySize = { static_cast<double>(window_size.x), static_cast<double>(window_size.y) };
    ImGuiHelper::Initialize(window.GetSDLWindow(), window.GetGLContext());
    window.SetEventCallback(ImGuiHelper::FeedEvent);
    impl->ImmediateRenderer2D.Init();
    impl->InstancedRenderer2D.Init(); //MC
    CS230::AudioManager::Instance().Init();

    impl->timer.ResetTimeStamp();
}

void Engine::Stop()
{
    impl->ImmediateRenderer2D.Shutdown();
    impl->InstancedRenderer2D.Shutdown(); //MC

    impl->gameStateManager.Clear();
    CS230::AudioManager::Instance().Shutdown();
    ImGuiHelper::Shutdown();
    impl->logger.LogEvent("Engine Stopped");
}

void Engine::Update()
{
    updateEnvironment();
    impl->window.Update();
    impl->input.Update();
    auto& state_manager = impl->gameStateManager;
    state_manager.Update();
    const auto        viewport      = impl->viewport;
    const Math::ivec2 viewport_size = { viewport.width, viewport.height };
    CS200::RenderingAPI::SetViewport(viewport_size, { viewport.x, viewport.y });
    state_manager.Draw();
    impl->viewport = ImGuiHelper::Begin();
    state_manager.DrawImGui();
    ImGuiHelper::End();
    CS230::Font::IncrementFrame();
}

bool Engine::HasGameEnded()
{
    return impl->window.IsClosed() || impl->gameStateManager.HasGameEnded();\
}

Engine::Engine() : impl(new Impl())
{
}

Engine::~Engine()
{
    delete impl;
}

void Engine::updateEnvironment()
{
    auto& environment     = impl->environment;
    environment.DeltaTime = impl->timer.GetElapsedSeconds();
    impl->timer.ResetTimeStamp();
    environment.ElapsedTime += environment.DeltaTime;
    ++environment.FrameCount;
    impl->fps.Update(environment.DeltaTime);
    environment.FPS               = impl->fps;
    const auto viewport           = impl->viewport;
    impl->environment.DisplaySize = { static_cast<double>(viewport.width), static_cast<double>(viewport.height) };
}
