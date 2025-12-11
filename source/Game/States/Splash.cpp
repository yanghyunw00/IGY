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
 #include "MainMenu.hpp"
 



Splash::Splash()
{
    
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
    if (loaded == true)
    {
        CS200::RenderingAPI::Clear();
        auto& renderer2d = Engine::GetRenderer2D();

        auto windowSize = Engine::GetWindow().GetSize();
        auto ndcMatrix  = CS200::build_ndc_matrix(windowSize);
        renderer2d.BeginScene(ndcMatrix);

        auto win  = Engine::GetWindow().GetSize();
        auto size = Splash_texture.GetSize();

        Math::vec2 Splash_Position = {static_cast<double>(windowSize.x - size.x)/2.0 , static_cast<double>(win.y  - size.y)};
        Splash_texture.Draw(Splash_Position, 0xFFFFFFFF);        

        renderer2d.EndScene();
    }
}
void Splash::DrawImGui()
{

}
