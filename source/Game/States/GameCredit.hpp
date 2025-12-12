/**
 * \file GameCredit.hpp
 * \brief Displays the main screen
 * \author Youngwoo Lee(youngwoo.lee)
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include "CS200/IRenderer2D.hpp"
#include "CS200/NDC.hpp"
#include "CS200/RGBA.hpp"
#include "CS200/RenderingAPI.hpp"
#include "Dokev/Core/DKColor.hpp"
#include "Dokev/Graphics/DKTexture.hpp"
#include "Dokev/State/DKGameState.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Font.hpp"
#include "Engine/Input.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Vec2.hpp"
#include "Engine/Window.hpp"

class GameCredit : public Dokev::GameState
{
public:
    void                  Load() override;
    void                  Update(double dt) override;
    void                  Unload() override;
    virtual gsl::czstring GetName() const override;
    void                  Draw() const override;
    void                  DrawImGui() override;
    void                  DrawText(const std::string& text, const Math::vec2& position, CS230::Font& font, CS200::RGBA color, Math::vec2 textscale) const;

private:
    std::unique_ptr<CS230::Font> Font_outlined;

    const std::string Credit         = "<Credit>";
    const std::string Libraries      = "- 3rd-Party Libraries -";
    const std::string Game_Copyright = "- Game Copyright -";

    const std::string GLEW          = "GLEW 2.2.0 (OpenGL Extension Wrangler Library)";
    const std::string GLEWLicense   = "Modified BSD License";
    const std::string GLEWCopyright = "Copyright (C) 2002-2007, Milan Ikits, Marcelo E. Magallon, Lev Povalahev";

    const std::string SDL2          = "SDL2 2.32.8 (Simple DirectMedia Layer)";
    const std::string SDL2License   = "zlib License";
    const std::string SDL2Copyright = "Copyright (C) 1997-2025 Sam Lantinga slouken(at)libsdl.org";

    const std::string ImGui          = "Dear ImGui (Immediate Mode GUI)";
    const std::string ImGuiLicense   = "MIT License";
    const std::string ImGuiCopyright = "Copyright (c) 2014-2025 Omar Cornut";

    const std::string Microsoft          = "Microsoft GSL 4.2.0 (Guidelines Support Library)";
    const std::string MicrosoftLicense   = "MIT License";
    const std::string MicrosoftCopyright = "Copyright (c) 2015 Microsoft Corporation";

    const std::string STB          = "STB Libraries (Single-file public domain libraries)";
    const std::string STBLicense   = "MIT License or Public Domain (dual license)";
    const std::string STBCopyright = "Copyright (c) 2017 Sean Barrett";

    const std::string DigipenCopyright = "Copyright (C) 2025 DigiPen (USA) Corporation";

    const std::string Back = "Back";

    ////////////////////
    //   Text Color   //
    ////////////////////
    CS200::RGBA TextColor_Title    = Dokev::ColorPalette::WHITE;
    CS200::RGBA TextColor_ThirdPL  = Dokev::ColorPalette::YELLOW;
    CS200::RGBA TextColor_GameCP   = Dokev::ColorPalette::YELLOW;
    CS200::RGBA TextColor_Back     = Dokev::ColorPalette::PINK;
    CS200::RGBA TextColor_contents = Dokev::ColorPalette::WHITE;

    ////////////////////
    //   Text Scale   //
    ////////////////////
    double TextScale_Credit_x = 0.7;
    double TextScale_Credit_y = 0.7;

    double TextScale_Title_x = 0.5;
    double TextScale_Title_y = 0.5;

    double TextScale_contents_x = 0.3;
    double TextScale_contents_y = 0.3;

    double TextScale_Back_x = 0.6;
    double TextScale_Back_y = 0.55;

    Math::vec2 TextScale_Credit   = { TextScale_Credit_x, TextScale_Credit_y };
    Math::vec2 TextScale_Title    = { TextScale_Title_x, TextScale_Title_y };
    Math::vec2 TextScale_contents = { TextScale_contents_x, TextScale_contents_y };
    Math::vec2 TextScale_Back     = { TextScale_Back_x, TextScale_Back_y };

    ////////////////////
    // Text Alignment //
    ////////////////////
    int Text_dist_x = 100;
    int Text_dist_y = 40;

    int Padding_x       = 200;
    int Padding_y_Top   = 100;
    int Padding_y_Top_2 = 50;

    int Padding_y_Bottom             = 100;
    int Padding_y_Bottom_to_Teamname = 50;

    int Padding_y_Top_Library           = Engine::GetWindow().GetWindowSize().y - Padding_y_Top;
    int Padding_y_Top_Game_Copyright    = Padding_y_Bottom + 100;
    int Padding_y_Bottom_Game_Copyright = Padding_y_Top_Game_Copyright - Text_dist_y;
    int Padding_y_Middle_Library        = (Engine::GetWindow().GetWindowSize().y - Padding_y_Top - Padding_y_Top_Game_Copyright) / 5;

    int contents_counter   = 1;
    int contents_padding_y = 20;

    Math::ivec2 window_size = Engine::GetWindow().GetSize();
    int         center_x    = (window_size.x) / 2;
    int         center_y    = (window_size.y) / 2;

    int Left_x = Padding_x; // 1


    Math::vec2 Text_Position_Center_Bottom         = { static_cast<double>(center_x), 0.0 + static_cast<double>(Padding_y_Bottom) };
    Math::vec2 Text_Position_Credit                = { static_cast<double>(center_x), static_cast<double>(window_size.y) - static_cast<double>(Padding_y_Top_2) };
    Math::vec2 Text_Position_Third_Party_Libraries = { static_cast<double>(center_x), static_cast<double>(window_size.y) - static_cast<double>(Padding_y_Top) };
    Math::vec2 Text_Position_Back                  = { static_cast<double>(center_x), static_cast<double>(Padding_y_Bottom - 50) };


    Math::vec2 Text_Position_GLEW           = { static_cast<double>(center_x), static_cast<double>(Padding_y_Top_Library + Text_dist_y - Padding_y_Middle_Library * 1) };
    Math::vec2 Text_Position_SDL2           = { static_cast<double>(center_x), static_cast<double>(Padding_y_Top_Library + Text_dist_y - Padding_y_Middle_Library * 2) };
    Math::vec2 Text_Position_ImGui          = { static_cast<double>(center_x), static_cast<double>(Padding_y_Top_Library + Text_dist_y - Padding_y_Middle_Library * 3) };
    Math::vec2 Text_Position_Microsoft      = { static_cast<double>(center_x), static_cast<double>(Padding_y_Top_Library + Text_dist_y - Padding_y_Middle_Library * 4) };
    Math::vec2 Text_Position_STB            = { static_cast<double>(center_x), static_cast<double>(Padding_y_Top_Library + Text_dist_y - Padding_y_Middle_Library * 5) };
    Math::vec2 Text_Position_Game_Copyright = { static_cast<double>(center_x), static_cast<double>(Padding_y_Top_Game_Copyright - 50) };
};