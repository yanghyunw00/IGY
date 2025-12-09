// Load - > TimerReset
// update - > GetElapsedTime = 2.0
// Unload
/**
 * \file Splash.cpp
 * \brief 
 * \author Youngwoo Lee(youngwoo.lee)
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

 #include "Splash.hpp"
 #include "GamePlayState.hpp"
 #include "Engine/Engine.hpp"
 #include "Engine/GameStateManager.hpp"
 #include "Engine/Window.hpp"
 #include "Game/Mainmenu.hpp"
 



Splash::Splash()
{
    //Splash_texture = Dokev::LoadTexture("Assets/images/Digipen.png");
}

void Splash::Load() 
{
    if (!loaded)
    {
        Splash_texture = Dokev::LoadTexture("Assets/images/DigiPen.png");
        //std::cout << "LOADED: " << Splash_texture.IsValid() << std::endl;
        loaded = true;
    }
    
}
void Splash::Update(double dt) 
{


    timercounter += dt;

    if(timercounter >= timerLimit)
    {
        Engine::GetGameStateManager().PopState();
        Engine::GetGameStateManager().PushState<MainMenu>();
    }
    //Draw();
}
void Splash::Unload()
{

}
gsl::czstring Splash::GetName() const 
{
    return "Splash";
}
void Splash::Draw() const
{
    CS200::RenderingAPI::Clear();
    auto& renderer2d = Engine::GetRenderer2D();

    auto windowSize = Engine::GetWindow().GetSize();
    auto ndcMatrix  = CS200::build_ndc_matrix(windowSize);
    renderer2d.BeginScene(ndcMatrix);

    auto win  = Engine::GetWindow().GetSize();
    auto size = Splash_texture.GetSize();

    Splash_texture.Draw(static_cast<Math::vec2>( (200,200)/*((windowSize.x - size.x) , (win.y  / 2.0 + size.y))*/ ), 0xFFFFFFFF);
    Engine::GetLogger().LogDebug("Win_X :" + std::to_string(win.x));
    Engine::GetLogger().LogDebug("Win_X :" + std::to_string(windowSize.x));

    Engine::GetLogger().LogDebug("Win_Y :" + std::to_string(win.y));
    Engine::GetLogger().LogDebug("TEX_X :" + std::to_string(size.x));
    Engine::GetLogger().LogDebug("TEX_Y :" + std::to_string(size.y));            

    //std::cout << "WIN: " << win.x << "x" << win.y << ", TEX: " << size.x << "x" << size.y << "\n";
    renderer2d.EndScene();
}
void Splash::DrawImGui()
{

}
// void Splash::DrawText(const std::string& text, const Math::vec2& position, CS230::Font& font, CS200::RGBA color, Math::vec2 textscale) const
// {

// }