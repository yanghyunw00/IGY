/**
 * \file Setting.cpp
 * \brief
 * \author
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "Setting.hpp"
#include "Dokev/Graphics/DKShapes.hpp"
#include "Engine/AudioManager.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Window.hpp"
#include "GamePlayState.hpp"
#include "MainMenu.hpp"
#include <SDL.h>

bool Setting::s_uiHidden  = false;
bool Setting::s_buildMode = false;
bool Setting::s_fromGame  = false;
bool Setting::s_isOpen    = false;

void Setting::Load()
{
    CS200::RenderingAPI::SetClearColor(CS200::WHITE);
    volume  = CS230::AudioManager::Instance().GetVolumeLevel();
    current = Setting_Keys::Volume;
    s_isOpen = true;
    mouse.Update(); // consume any held-button state from before entering Setting

    try
    {
        Font_simple = std::make_unique<CS230::Font>("Assets/fonts/DungGeunMo.ttf", 40);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    try
    {
        Font_outlined = std::make_unique<CS230::Font>("Assets/fonts/DungGeunMo.ttf", 40);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void Setting::Update(double dt)
{
    (void)dt;
    mouse.Update();

    // Highlight selected item
    TextColor_Volume    = (current == Setting_Keys::Volume)    ? Dokev::ColorPalette::HOTPINK : Dokev::ColorPalette::BLACK;
    TextColor_VSync     = (current == Setting_Keys::VSync)     ? Dokev::ColorPalette::HOTPINK : Dokev::ColorPalette::BLACK;
    TextColor_UIToggle  = (current == Setting_Keys::UIToggle)  ? Dokev::ColorPalette::HOTPINK : Dokev::ColorPalette::BLACK;
    TextColor_BuildMode = (current == Setting_Keys::BuildMode) ? Dokev::ColorPalette::HOTPINK : Dokev::ColorPalette::BLACK;
    TextColor_Back      = (current == Setting_Keys::Back)      ? Dokev::ColorPalette::HOTPINK : Dokev::ColorPalette::BLACK;

    const auto& input = Engine::GetInput();

    if (input.KeyJustPressed(CS230::Input::Keys::Escape))
    {
        Engine::GetGameStateManager().PopState();
        if (!s_fromGame)
            Engine::GetGameStateManager().PushState<MainMenu>();
        s_fromGame = false;
        return;
    }

    if (input.KeyJustPressed(CS230::Input::Keys::Down) || input.KeyJustPressed(CS230::Input::Keys::S))
    {
        int nextstate = static_cast<int>(current) + 1;
        if (nextstate >= static_cast<int>(Setting_Keys::Count))
            nextstate = 0;
        current = static_cast<Setting_Keys>(nextstate);
    }

    if (input.KeyJustPressed(CS230::Input::Keys::Up) || input.KeyJustPressed(CS230::Input::Keys::W))
    {
        int prestate = static_cast<int>(current) - 1;
        if (prestate < 0)
            prestate = static_cast<int>(Setting_Keys::Count) - 1;
        current = static_cast<Setting_Keys>(prestate);
    }

    if (input.KeyJustPressed(CS230::Input::Keys::A) || input.KeyJustPressed(CS230::Input::Keys::Left))
    {
        if (current == Setting_Keys::Volume)
        {
            int v = volume - 1;
            SetVolume(v < volume_min ? volume_max : v);
            return;
        }
        else if (current == Setting_Keys::VSync)     { vsyncOn   = false; SDL_GL_SetSwapInterval(0); return; }
        else if (current == Setting_Keys::UIToggle)  { s_uiHidden  = false; return; }
        else if (current == Setting_Keys::BuildMode) { s_buildMode = false; return; }
    }

    if (input.KeyJustPressed(CS230::Input::Keys::D) || input.KeyJustPressed(CS230::Input::Keys::Right))
    {
        if (current == Setting_Keys::Volume)
        {
            int v = volume + 1;
            SetVolume(v > volume_max ? volume_min : v);
            return;
        }
        else if (current == Setting_Keys::VSync)     { vsyncOn   = true; SDL_GL_SetSwapInterval(1); return; }
        else if (current == Setting_Keys::UIToggle)  { s_uiHidden  = true; return; }
        else if (current == Setting_Keys::BuildMode) { s_buildMode = true; return; }
    }

    // Mouse hover
    {
        auto        winSz = Engine::GetWindow().GetSize();
        Math::fvec2 mp    = mouse.GetPositionFloat();
        Math::fvec2 renderMp{ mp.x, static_cast<float>(winSz.y) - mp.y };

        constexpr float hitW = 320.0f;
        constexpr float hitH = 65.0f;

        auto hoverCheck = [&](Math::vec2 pos, Setting_Keys key)
        {
            Math::fvec2 rectPos{ static_cast<float>(pos.x) - hitW * 0.5f, static_cast<float>(pos.y) - hitH * 0.5f };
            if (Dokev::PointInRect(renderMp, rectPos, { hitW, hitH }))
                current = key;
        };
        hoverCheck(Text_Position_Volume,    Setting_Keys::Volume);
        hoverCheck(Text_Position_VSync,     Setting_Keys::VSync);
        hoverCheck(Text_Position_UIToggle,  Setting_Keys::UIToggle);
        hoverCheck(Text_Position_BuildMode, Setting_Keys::BuildMode);
        hoverCheck(Text_Position_Center_Bottom, Setting_Keys::Back);
    }

    bool confirmed = input.KeyJustPressed(CS230::Input::Keys::Enter) || mouse.MouseJustPressed(Dokev::Mouse::Button::Left);

    if (confirmed)
    {
        switch (current)
        {
            case Setting_Keys::Back:
                Engine::GetGameStateManager().PopState();
                if (!s_fromGame)
                    Engine::GetGameStateManager().PushState<MainMenu>();
                s_fromGame = false;
                return;
            case Setting_Keys::Volume:
                {
                    int v = volume + 1;
                    SetVolume(v > volume_max ? volume_min : v);
                    return;
                }
            case Setting_Keys::VSync:
                vsyncOn = !vsyncOn;
                SDL_GL_SetSwapInterval(vsyncOn ? 1 : 0);
                return;
            case Setting_Keys::UIToggle:
                s_uiHidden = !s_uiHidden;
                return;
            case Setting_Keys::BuildMode:
                s_buildMode = !s_buildMode;
                return;
            default: break;
        }
    }
}

void Setting::Unload()
{
    s_isOpen = false;
}

gsl::czstring Setting::GetName() const
{
    return "Setting";
}

void Setting::Draw() const
{
    CS200::RenderingAPI::Clear();
    auto& renderer2d = Engine::GetRenderer2D();

    renderer2d.BeginScene(CS200::build_ndc_matrix(Engine::GetWindow().GetSize()));

    ////////////////////////////////
    //           Volume           //
    ////////////////////////////////

    DrawText(text_Volume, Text_Position_Volume, *Font_outlined, TextColor_Volume, TextScale_Volume);

    // Volume slider (below Volume label)
    DrawSlider(center_x, static_cast<int>(Text_Position_Volume.y) - 52, volume, volume_max);

    // VSync
    DrawText(text_VSync, Text_Position_VSync, *Font_outlined, TextColor_VSync, TextScale_VSync);
    DrawToggleBox(center_x, static_cast<int>(Text_Position_VSync.y), vsyncOn);

    // UI Toggle
    DrawText(text_UIToggle, Text_Position_UIToggle, *Font_outlined, TextColor_UIToggle, TextScale_UIToggle);
    DrawToggleBox(center_x, static_cast<int>(Text_Position_UIToggle.y), s_uiHidden);

    // Build Mode
    DrawText(text_BuildMode, Text_Position_BuildMode, *Font_outlined, TextColor_BuildMode, TextScale_BuildMode);
    DrawToggleBox(center_x, static_cast<int>(Text_Position_BuildMode.y), s_buildMode);

    ////////////////////////////////
    //           Back             //
    ////////////////////////////////

    DrawText(text_Back, Text_Position_Center_Bottom, *Font_outlined, TextColor_Back, TextScale_Back);

    renderer2d.EndScene();
}

void Setting::DrawSlider(int cx, int centerY, int value, int maxVal) const
{
    const int   sliderW = 300;
    const int   sliderH = 16;
    const int   x       = cx - sliderW / 2;
    const int   y       = centerY - sliderH / 2;  // bottom of rect in y-up
    const float fill    = (maxVal > 0) ? static_cast<float>(value) / static_cast<float>(maxVal) : 0.0f;

    // Track
    Dokev::DrawRectangle(x, y, sliderW, sliderH, 0xDDDDDDFF);
    // Fill
    int fillW = static_cast<int>(sliderW * fill);
    if (fillW > 0)
        Dokev::DrawRectangle(x, y, fillW, sliderH, 0x4488FFFF);
    // Border
    Dokev::DrawRectangleLinesEx(x, y, sliderW, sliderH, 2.0f, 0x555555FF);
    // Knob
    int knobX = x + fillW - 5;
    Dokev::DrawRectangle(knobX, y - 4, 10, sliderH + 8, 0x1155CCFF);
    // Tick marks
    for (int i = 0; i <= maxVal; ++i)
    {
        int tx = x + static_cast<int>(sliderW * i / static_cast<float>(maxVal)) - 1;
        Dokev::DrawRectangle(tx, y - 2, 2, sliderH + 4, 0x888888FF);
    }
}

void Setting::DrawToggleBox(int cx, int itemCenterY, bool on) const
{
    const int boxW = 60;
    const int boxH = 28;
    const int bx   = cx + 190;
    const int by   = itemCenterY - boxH / 2;

    CS200::RGBA bg  = on ? 0x33AA33FF : 0x888888AA;
    CS200::RGBA brd = on ? 0x22882299 : 0x55555599;
    Dokev::DrawRectangle(bx, by, boxW, boxH, bg);
    Dokev::DrawRectangleLinesEx(bx, by, boxW, boxH, 1.5f, brd);

    if (Font_simple)
    {
        const char* label = on ? "ON" : "OFF";
        if (auto t = Font_simple->PrintToTexture(label, 0xFFFFFFFF))
        {
            float ts = 0.55f;
            float tx = static_cast<float>(bx + boxW / 2) - t->GetSize().x * ts * 0.5f;
            float ty = static_cast<float>(by + boxH / 2) - t->GetSize().y * ts * 0.5f;
            t->Draw(Math::TranslationMatrix(Math::vec2{ tx, ty }) * Math::ScaleMatrix(Math::vec2{ ts, ts }), 0xFFFFFFFF);
        }
    }
}

void Setting::SetVolume(int scale)
{
    volume = scale;
    CS230::AudioManager::Instance().SetVolumeLevel(volume, volume_max);
}

void Setting::DrawText(const std::string& text, const Math::vec2& position, CS230::Font& font, CS200::RGBA color, Math::vec2 textscale) const
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

void Setting::DrawImGui()
{
}