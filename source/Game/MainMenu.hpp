#pragma once
 #include "Dokev/DKTexture.hpp"
 #include "Engine/Texture.hpp"
 #include "Engine/Font.hpp"
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

class MainMenu : public Dokev::GameState
{
public : 
    void          Load() override;
    void          Update(double dt) override;
    void          Unload() override;
    virtual  gsl::czstring  GetName() const override;
    void          Draw() const override;
    void          DrawImGui() override;
    void          DrawText(const std::string& text, const Math::vec2& position, CS230::Font& font, CS200::RGBA color, Math::vec2 textscale) const;

private : 
    std::unique_ptr<CS230::Font> Font_simple;
    std::unique_ptr<CS230::Font> Font_outlined;

    // std::shared_ptr<CS230::Texture> Title;
    // std::shared_ptr<CS230::Texture> PlayIGY;
    // std::shared_ptr<CS230::Texture> Exit;
    // std::shared_ptr<CS230::Texture> Antartic_Monkeys;

    const std::string text_Title                = "I G Y";
    const std::string text_PlayIGY              = "PlayIGY";
    const std::string text_Exit                 = "Exit";
    const std::string text_Antartic_Monkeys     = "Antartic_Monkeys";

    enum Main_Keys {

        GamePlayKey,
        ExitKey,
        Count
    };

    ////////////////////
    //   Text Color   //
    ////////////////////
    CS200::RGBA TextColor_Title                = Dokev::ColorPalette::WHITE;
    CS200::RGBA TextColor_PlayIGY              = Dokev::ColorPalette::WHITE;
    CS200::RGBA TextColor_Exit                 = Dokev::ColorPalette::WHITE;
    CS200::RGBA TextColor_Antartic_Monkeys     = Dokev::ColorPalette::WHITE;

    ////////////////////
    //   Text Scale   //
    ////////////////////
    double TextScale_Title_x                   = 2.5;
    double TextScale_Title_y                   = 2.7;

    double TextScale_PlayIGY_x                 = 1.0;
    double TextScale_PlayIGY_y                 = 1.0;
    double TextScale_Exit_x                    = 1.0;
    double TextScale_Exit_y                    = 0.95;

    double TextScale_Antartic_Monkeys_x        = 0.4;
    double TextScale_Antartic_Monkeys_y        = 0.4;

    Math::vec2 TextScale_Title                 = {TextScale_Title_x,TextScale_Title_y};
    Math::vec2 TextScale_PlayIGY               = {TextScale_PlayIGY_x,TextScale_PlayIGY_y};
    Math::vec2 TextScale_Exit                  = {TextScale_Exit_x,TextScale_Exit_y};
    Math::vec2 TextScale_Antartic_Monkeys      = {TextScale_Antartic_Monkeys_x, TextScale_Antartic_Monkeys_y};

    ////////////////////
    // Text Alignment //
    ////////////////////
    int Text_dist_x            = 100; 
    int Text_dist_y            = 75;

    int Padding_x              = 200;
    int Padding_y_Top          = 200;
    int Padding_y_Bottom       = 100;

    Math::ivec2 window_size = Engine::GetWindow().GetSize();
    int center_x               = (window_size.x) / 2;
    int center_y               = (window_size.y) / 2;

    int PlayIGY_x              = (window_size.x) / 2; // 1
    int PlayIGY_y              = (window_size.y) / 2;
    int Exit_x                 = (window_size.x) / 2; // 2
    int Exit_y                 = (window_size.y) / 2 - Text_dist_y;

    Math::vec2 Text_Position_Center_Bottom = {static_cast<double>(center_x), 0.0 + static_cast<double>(Padding_y_Bottom)};
    Math::vec2 Text_Position_Title = {static_cast<double>(center_x), static_cast<double>(window_size.y) - static_cast<double>(Padding_y_Top)};
    
    Main_Keys current = Main_Keys::GamePlayKey;

};