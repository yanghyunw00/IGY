/**
 * \file Splash.hpp
 * \brief 
 * \author Youngwoo Lee(youngwoo.lee)
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#pragma once
 #include "Dokev/DKTexture.hpp"
 #include "Engine/Texture.hpp"
 #include "Dokev/DKColor.hpp"
 #include "Dokev/DKGameState.hpp"
 #include "Engine/Vec2.hpp"
 #include "Engine/Matrix.hpp"
 #include "Engine/Engine.hpp"
 #include "CS200/RGBA.hpp"
 #include "CS200/RenderingAPI.hpp"
 #include "CS200/IRenderer2D.hpp"
 #include "CS200/NDC.hpp"
 #include "Engine/Input.hpp"
 #include "Engine/Window.hpp"
 #include "Engine/Timer.hpp"

class Splash : public Dokev::GameState
{
public : 
                  Splash();
    void          Load() override;
    void          Update(double dt) override;
    void          Unload() override;
    virtual  gsl::czstring  GetName() const override;
    void          Draw() const override;
    void          DrawImGui() override;
    //void          DrawText(const std::string& text, const Math::vec2& position, CS230::Font& font, CS200::RGBA color, Math::vec2 textscale) const;

private : 

double timercounter = 0.0;
double timerLimit = 3.0;
Dokev::Texture2D Splash_texture;/*= Dokev::LoadTexture("Assets/images/DigiPen.png");*/
//Dokev::Texture2D Splash_texture("Assets/images/Digipen.png");
//auto transform = Math::TranslationMatrix({100, 100});
bool loaded = false;



};

