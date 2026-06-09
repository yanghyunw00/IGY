/**
 * \file GameCredit.cpp
 * \brief Displays the credits with a Star Wars-style scrolling image
 * \author Youngwoo Lee(youngwoo.lee)
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "GameCredit.hpp"
#include "CS200/NDC.hpp"
#include "CS200/RenderingAPI.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Input.hpp"
#include "Engine/Window.hpp"
#include "MainMenu.hpp"

void GameCredit::Load()
{
    CS200::RenderingAPI::SetClearColor(CS200::WHITE);
    creditTexture = Dokev::LoadTexture("Assets/images/igy_credit.png");

    try
    {
        hintFont = std::make_unique<CS230::Font>("Assets/fonts/DungGeunMo.ttf", 40);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    if (creditTexture.IsValid())
    {
        auto  winSize  = Engine::GetWindow().GetSize();
        float imgWidth = static_cast<float>(creditTexture.GetWidth());
        imageScale     = static_cast<float>(winSize.x) / imgWidth;
        scaledHeight   = creditTexture.GetHeight() * static_cast<double>(imageScale);
        scrollY        = -4300;
        loaded         = true;
    }
}

void GameCredit::Update(double dt)
{
    const auto& input = Engine::GetInput();

    if (input.KeyJustPressed(CS230::Input::Keys::Enter) || input.KeyJustPressed(CS230::Input::Keys::Escape))
    {
        Engine::GetGameStateManager().PopState();
        Engine::GetGameStateManager().PushState<MainMenu>();
        return;
    }

    scrollY += SCROLL_SPEED * dt;

    if (input.KeyDown(CS230::Input::Keys::Space))
    {
        scrollY += SCROLL_SPEED * dt * 3.0;
    }

    if (scrollY > 0)
    {
        scrollY = -4300;
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
    if (!loaded)
        return;

    auto& renderer2d = Engine::GetRenderer2D();
    auto  winSize    = Engine::GetWindow().GetSize();
    renderer2d.BeginScene(CS200::build_ndc_matrix(winSize));

    float      posX = (winSize.x - creditTexture.GetWidth() * imageScale) / 2.0f;
    Math::vec2 pos{ static_cast<double>(posX), scrollY };
    creditTexture.DrawEx(pos, 0.0f, Math::vec2{ imageScale, imageScale }, CS200::WHITE);

    constexpr double rightMargin  = 20.0;
    constexpr double lineHeight   = 40.0;
    constexpr double bottomMargin = 20.0;
    double           leftEdge     = static_cast<double>(winSize.x) - rightMargin - 380.0;

    DrawHintText(escapeText, { leftEdge, bottomMargin + lineHeight }, Dokev::ColorPalette::ORANGE);
    DrawHintText(spaceBarText, { leftEdge, bottomMargin + lineHeight * 2.0 }, Dokev::ColorPalette::ORANGE);
    renderer2d.EndScene();
}

void GameCredit::DrawImGui()
{
}

void GameCredit::DrawHintText(const std::string& text, Math::vec2 bottomLeft, CS200::RGBA color) const
{
    if (!hintFont)
        return;

    auto text_texture = hintFont->PrintToTexture(text, color);
    if (!text_texture)
        return;

    const Math::vec2 scale{ 0.45, 0.45 };
    constexpr double xMargin = 150;
    constexpr double yMargin = 50;
    Math::vec2       drawPos{ bottomLeft.x + xMargin, bottomLeft.y - yMargin };
    const auto       transform = Math::TranslationMatrix(drawPos) * Math::ScaleMatrix(scale);
    text_texture->Draw(transform, color);
}
