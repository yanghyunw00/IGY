/**
 * \file MainMenu.cpp
 * \brief Displays the main screen
 * \author Youngwoo Lee(youngwoo.lee)
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "MainMenu.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameStateManager.hpp"
#include "GamePlayState.hpp"
#include "Dokev/DKMapManager.hpp"

// #include "Engine/Windwow.hpp"


//  MainMenu::MainMenu()
// {

// }

void MainMenu::Load()
{
    CS200::RenderingAPI::SetClearColor(0x0d0e20FF);

    try
    {
        Font_simple = std::make_unique<CS230::Font>("Assets/fonts/Font_Simple.png");
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

     try
    {
        Font_outlined = std::make_unique<CS230::Font>("Assets/fonts/Font_Outlined.png");
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    selectedVariant = MapManager::GetNextMapVariant();
}

void MainMenu::Update(double dt)
{
    (void)dt;

    TextColor_PlayIGY   = Dokev::ColorPalette::WHITE;
    TextColor_MapSelect = Dokev::ColorPalette::WHITE;
    TextColor_Exit      = Dokev::ColorPalette::WHITE;

    switch (current)
    {
        case Main_Keys::GamePlayKey:
            TextColor_PlayIGY = Dokev::ColorPalette::HOTPINK;
            break;
        case Main_Keys::MapSelectKey:
            TextColor_MapSelect = Dokev::ColorPalette::HOTPINK;
            break;
        case Main_Keys::ExitKey:
            TextColor_Exit = Dokev::ColorPalette::HOTPINK;
            break;
        default: break;
    }
    const auto& input = Engine::GetInput();

    if (input.KeyJustPressed(CS230::Input::Keys::Down) || input.KeyJustPressed(CS230::Input::Keys::S))
    {
        int nextstate = static_cast<int>(current) + 1;

        if (nextstate >= Count)
        {
            nextstate = 0;
        }

        current = static_cast<Main_Keys>(nextstate);
    }

    if (input.KeyJustPressed(CS230::Input::Keys::Up))
    {
        int prestate = static_cast<int>(current) - 1;

        if (prestate < GamePlayKey)
        {
            prestate = static_cast<int>(Count) - 1;
        }

        current = static_cast<Main_Keys>(prestate);
    }

    if (current == Main_Keys::MapSelectKey)
    {
        if (input.KeyJustPressed(CS230::Input::Keys::Left) || input.KeyJustPressed(CS230::Input::Keys::A))
        {
            CycleMapVariant(-1);
        }

        if (input.KeyJustPressed(CS230::Input::Keys::Right) || input.KeyJustPressed(CS230::Input::Keys::D))
        {
            CycleMapVariant(1);
        }
    }

    if (input.KeyJustPressed(CS230::Input::Keys::Enter))
    {
        switch (current)
        {
            case Main_Keys::GamePlayKey:
                MapManager::SetNextMapVariant(selectedVariant);
                Engine::GetGameStateManager().PopState();
                Engine::GetGameStateManager().PushState<GamePlayState>();
                return;
                break;
            case Main_Keys::MapSelectKey:
                CycleMapVariant(1);
                return;
                break;
            case Main_Keys::ExitKey:
                {
                    SDL_Event e{};
                    e.type         = SDL_WINDOWEVENT;
                    e.window.event = SDL_WINDOWEVENT_CLOSE;
                    SDL_PushEvent(&e);
                    return;
                }
                break;

            default: break;
        }
    }
}

void MainMenu::Unload()
{
}

gsl::czstring MainMenu::GetName() const
{
    return "MainMenu";
}

void MainMenu::Draw() const
{
    CS200::RenderingAPI::Clear();
    auto& renderer2d = Engine::GetRenderer2D();

    renderer2d.BeginScene(CS200::build_ndc_matrix(Engine::GetWindow().GetSize()));

    ////////////////////////////////
    //           Title            //
    ////////////////////////////////

    DrawText(text_Title, Text_Position_Title, *Font_outlined, TextColor_Title, TextScale_Title);

    ////////////////////////////////
    //          PlayIGY           //
    ////////////////////////////////
    Math::vec2 pos{ static_cast<double>(PlayIGY_x), static_cast<double>(PlayIGY_y) };
    DrawText(text_PlayIGY, pos, *Font_simple, TextColor_PlayIGY, TextScale_PlayIGY);

    ////////////////////////////////
    //        Map Select          //
    ////////////////////////////////
    Math::vec2 mapPos{ static_cast<double>(MapSelect_x), static_cast<double>(MapSelect_y) };
    DrawText(GetMapLabel(), mapPos, *Font_simple, TextColor_MapSelect, TextScale_MapSelect);

    ////////////////////////////////
    //           Exit             //
    ////////////////////////////////
    Math::vec2 pos2{ static_cast<double>(Exit_x), static_cast<double>(Exit_y) };
    DrawText(text_Exit, pos2, *Font_simple, TextColor_Exit, TextScale_Exit);

    ////////////////////////////////
    //      Antartic_Monkeys      //
    ////////////////////////////////

    DrawText(text_Antartic_Monkeys, Text_Position_Center_Bottom, *Font_simple, TextColor_Antartic_Monkeys, TextScale_Antartic_Monkeys);
    Math::vec2 mapInstructionPos{ static_cast<double>(center_x), static_cast<double>(Padding_y_Bottom + 50) };
    DrawText(text_MapInstructions, mapInstructionPos, *Font_simple, TextColor_Antartic_Monkeys, TextScale_Antartic_Monkeys);

    renderer2d.EndScene();
}

void MainMenu::CycleMapVariant(int direction)
{
    constexpr int variantCount = 4;
    int           currentIndex = static_cast<int>(selectedVariant);
    currentIndex               = (currentIndex + direction + variantCount) % variantCount;
    selectedVariant            = static_cast<MapVariant>(currentIndex);
}

std::string MainMenu::GetMapLabel() const
{
    std::string label = text_MapSelect;
    label += ": ";
    label += GetMapVariantLabel(selectedVariant);
    return label;
}

void MainMenu::DrawImGui()
{
}

void MainMenu::DrawText(const std::string& text, const Math::vec2& position, CS230::Font& font, CS200::RGBA color, Math::vec2 textscale) const
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
