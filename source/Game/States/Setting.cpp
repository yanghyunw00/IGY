#include "Setting.hpp"
#include "Engine/AudioManager.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Window.hpp"
#include "GamePlayState.hpp"
#include "MainMenu.hpp"

void Setting::Load()
{
    CS200::RenderingAPI::SetClearColor(CS200::WHITE);
    volume = CS230::AudioManager::Instance().GetVolumeLevel();

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
    TextColor_Volume = (current == Setting_Keys::Volume) ? Dokev::ColorPalette::HOTPINK : Dokev::ColorPalette::BLACK;
    TextColor_Back   = (current == Setting_Keys::Back) ? Dokev::ColorPalette::HOTPINK : Dokev::ColorPalette::BLACK;

    const auto& input = Engine::GetInput();

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
    }

    if (input.KeyJustPressed(CS230::Input::Keys::D) || input.KeyJustPressed(CS230::Input::Keys::Right))
    {
        if (current == Setting_Keys::Volume)
        {
            int v = volume + 1;
            SetVolume(v > volume_max ? volume_min : v);
            return;
        }
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
        hoverCheck(Text_Position_Volume, Setting_Keys::Volume);
        hoverCheck(Text_Position_Center_Bottom, Setting_Keys::Back);
    }

    bool confirmed = input.KeyJustPressed(CS230::Input::Keys::Enter) || mouse.MouseJustPressed(Dokev::Mouse::Button::Left);

    if (confirmed)
    {
        switch (current)
        {
            case Setting_Keys::Back:
                Engine::GetGameStateManager().PopState();
                Engine::GetGameStateManager().PushState<MainMenu>();
                return;
            case Setting_Keys::Volume:
                {
                    int v = volume + 1;
                    SetVolume(v > volume_max ? volume_min : v);
                    return;
                }
            default: break;
        }
    }
}

void Setting::Unload()
{
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

    ////////////////////////////////
    //         volume_num         //
    ////////////////////////////////

    DrawText(std::to_string(volume), Text_Position_volume_num, *Font_simple, TextColor_volume_num, TextScale_volume_num);

    ////////////////////////////////
    //           Back             //
    ////////////////////////////////

    DrawText(text_Back, Text_Position_Center_Bottom, *Font_outlined, TextColor_Back, TextScale_Back);

    renderer2d.EndScene();
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