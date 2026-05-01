/**
 * \file MainMenu.cpp
 * \brief Displays the main screen
 * \author Youngwoo Lee(youngwoo.lee)
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "GameCredit.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameStateManager.hpp"
#include "GamePlayState.hpp"
#include "MainMenu.hpp"
#include "Setting.hpp"

void GameCredit::Load()
{
    CS200::RenderingAPI::SetClearColor(0x0d0e20FF);
    try
    {
        Font_outlined = std::make_unique<CS230::Font>("Assets/fonts/DungGeunMo.ttf", 40);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void GameCredit::Update(double dt)
{
    (void)dt;
    const auto& input = Engine::GetInput();

    if (input.KeyJustPressed(CS230::Input::Keys::Enter))
    {
        Engine::GetGameStateManager().PopState();
        Engine::GetGameStateManager().PushState<MainMenu>();
    }
}

void GameCredit::Unload()
{
}

gsl::czstring GameCredit::GetName() const
{
    return "GameCredit";
}

void GameCredit::Draw() const
{
    CS200::RenderingAPI::Clear();
    auto& renderer2d = Engine::GetRenderer2D();

    renderer2d.BeginScene(CS200::build_ndc_matrix(Engine::GetWindow().GetSize()));
    ////////////////////////////////
    //           Credit           //
    ////////////////////////////////

    DrawText(Credit, Text_Position_Credit, *Font_outlined, TextColor_Title, TextScale_Credit);

    ////////////////////////////////
    //         Libraries          //
    ////////////////////////////////

    DrawText(Libraries, Text_Position_Third_Party_Libraries, *Font_outlined, TextColor_ThirdPL, TextScale_Title);

    ////////////////////////////////
    //            GLEW            //
    ////////////////////////////////

    DrawText(GLEW, Text_Position_GLEW, *Font_outlined, TextColor_contents, TextScale_Title);
    Math::vec2 GlewContentPosition1 = { Text_Position_GLEW.x - 0, Text_Position_GLEW.y - (contents_padding_y * 2) + 10 };
    Math::vec2 GlewContentPosition2 = { Text_Position_GLEW.x - 0, Text_Position_GLEW.y - (contents_padding_y * 3) + 10 };

    DrawText(GLEWLicense, GlewContentPosition1, *Font_outlined, TextColor_contents, TextScale_contents);
    DrawText(GLEWCopyright, GlewContentPosition2, *Font_outlined, TextColor_contents, TextScale_contents);

    ////////////////////////////////
    //            SDL2            //
    ////////////////////////////////

    DrawText(SDL2, Text_Position_SDL2, *Font_outlined, TextColor_contents, TextScale_Title);
    Math::vec2 SDL2ContentPosition1 = { Text_Position_SDL2.x - 0, Text_Position_SDL2.y - (contents_padding_y * 2) + 10 };
    Math::vec2 SDL2ContentPosition2 = { Text_Position_SDL2.x - 0, Text_Position_SDL2.y - (contents_padding_y * 3) + 10 };

    DrawText(SDL2License, SDL2ContentPosition1, *Font_outlined, TextColor_contents, TextScale_contents);
    DrawText(SDL2Copyright, SDL2ContentPosition2, *Font_outlined, TextColor_contents, TextScale_contents);

    ////////////////////////////////
    //            ImGui           //
    ////////////////////////////////

    DrawText(ImGui, Text_Position_ImGui, *Font_outlined, TextColor_contents, TextScale_Title);
    Math::vec2 ImGuiContentPosition1 = { Text_Position_ImGui.x - 0, Text_Position_ImGui.y - (contents_padding_y * 2) + 10 };
    Math::vec2 ImGuiContentPosition2 = { Text_Position_ImGui.x - 0, Text_Position_ImGui.y - (contents_padding_y * 3) + 10 };

    DrawText(ImGuiLicense, ImGuiContentPosition1, *Font_outlined, TextColor_contents, TextScale_contents);
    DrawText(ImGuiCopyright, ImGuiContentPosition2, *Font_outlined, TextColor_contents, TextScale_contents);

    ////////////////////////////////
    //          Microsoft         //
    ////////////////////////////////

    DrawText(Microsoft, Text_Position_Microsoft, *Font_outlined, TextColor_contents, TextScale_Title);
    Math::vec2 MicrosoftContentPosition1 = { Text_Position_Microsoft.x - 0, Text_Position_Microsoft.y - (contents_padding_y * 2) + 10 };
    Math::vec2 MicrosoftContentPosition2 = { Text_Position_Microsoft.x - 0, Text_Position_Microsoft.y - (contents_padding_y * 3) + 10 };

    DrawText(MicrosoftLicense, MicrosoftContentPosition1, *Font_outlined, TextColor_contents, TextScale_contents);
    DrawText(MicrosoftCopyright, MicrosoftContentPosition2, *Font_outlined, TextColor_contents, TextScale_contents);

    ////////////////////////////////
    //             STB            //
    ////////////////////////////////

    DrawText(STB, Text_Position_STB, *Font_outlined, TextColor_contents, TextScale_Title);
    Math::vec2 STBContentPosition1 = { Text_Position_STB.x - 0, Text_Position_STB.y - (contents_padding_y * 2) + 10 };
    Math::vec2 STBContentPosition2 = { Text_Position_STB.x - 0, Text_Position_STB.y - (contents_padding_y * 3) + 10 };

    DrawText(STBLicense, STBContentPosition1, *Font_outlined, TextColor_contents, TextScale_contents);
    DrawText(STBCopyright, STBContentPosition2, *Font_outlined, TextColor_contents, TextScale_contents);

    ////////////////////////////////
    //       Game Copyright       //
    ////////////////////////////////

    DrawText(Game_Copyright, Text_Position_Game_Copyright, *Font_outlined, TextColor_GameCP, TextScale_Title);
    Math::vec2 Game_CopyrightContentPosition1 = { Text_Position_Game_Copyright.x - 0, Text_Position_Game_Copyright.y - (contents_padding_y * 2) + 10 };

    DrawText(DigipenCopyright, Game_CopyrightContentPosition1, *Font_outlined, TextColor_contents, TextScale_contents);

    ////////////////////////////////
    //            Back            //
    ////////////////////////////////

    DrawText(Back, Text_Position_Back, *Font_outlined, TextColor_Back, TextScale_Title);

    renderer2d.EndScene();
}

void GameCredit::DrawImGui()
{
}

void GameCredit::DrawText(const std::string& text, const Math::vec2& position, CS230::Font& font, CS200::RGBA color, Math::vec2 textscale) const
{
    auto text_texture = font.PrintToTexture(text, color);

    if (!text_texture)
    {
        return;
    }
    else
    {
        // Texture - GetSize
        Math::ivec2 size = text_texture->GetSize(); // {width, height}

        double     padding = 0.5;
        Math::vec2 halfSize{ static_cast<double>(size.x) * padding * textscale.x, static_cast<double>(size.y) * padding * textscale.y };

        // Center Pos - HalfSize
        Math::vec2 drawPos = position - halfSize;

        // Final Transform
        const auto position_transform = Math::TranslationMatrix(drawPos) * Math::ScaleMatrix(textscale);

        text_texture->Draw(position_transform, color);
    }
}