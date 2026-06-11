/**
 * \file Setting.hpp
 * \brief
 * \author
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
#include "Dokev/Input/DKMouseInput.hpp"
#include "Dokev/State/DKGameState.hpp"
#include "Dokev/World/DKCollision.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Font.hpp"
#include "Engine/Input.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Vec2.hpp"
#include "Engine/Window.hpp"

class Setting : public Dokev::GameState
{
public:
    void                  Load() override;
    void                  Update(double dt) override;
    void                  Unload() override;
    virtual gsl::czstring GetName() const override;
    void                  Draw() const override;
    void                  DrawImGui() override;
    void                  DrawText(const std::string& text, const Math::vec2& position, CS230::Font& font, CS200::RGBA color, Math::vec2 textscale) const;

    static bool IsUIHidden()        { return s_uiHidden; }
    static bool IsBuildMode()       { return s_buildMode; }
    static void SetFromGame(bool v) { s_fromGame = v; }
    static bool IsOpen()            { return s_isOpen; }

private:
    void DrawSlider(int cx, int centerY, int value, int maxVal) const;
    void DrawToggleBox(int cx, int itemCenterY, bool on) const;

    std::unique_ptr<CS230::Font> Font_simple;
    std::unique_ptr<CS230::Font> Font_outlined;

    //////////////////////
    //   Setting Keys   //
    //////////////////////
    enum class Setting_Keys
    {
        Volume,
        VSync,
        UIToggle,
        BuildMode,
        Back,
        Count
    };
    Setting_Keys current = Setting_Keys::Volume;

    //////////////////////
    //   Setting Text   //
    //////////////////////
    const std::string text_Volume   = "Volume";
    const std::string text_VSync    = "VSync";
    const std::string text_UIToggle = "UI Toggle";
    const std::string text_BuildMode= "Build Mode";
    const std::string text_Back     = "Back";

    ////////////////////
    //   Text Color   //
    ////////////////////
    CS200::RGBA TextColor_Volume     = Dokev::ColorPalette::BLACK;
    CS200::RGBA TextColor_volume_num = Dokev::ColorPalette::BLACK;
    CS200::RGBA TextColor_VSync      = Dokev::ColorPalette::BLACK;
    CS200::RGBA TextColor_UIToggle   = Dokev::ColorPalette::BLACK;
    CS200::RGBA TextColor_BuildMode  = Dokev::ColorPalette::BLACK;
    CS200::RGBA TextColor_Back       = Dokev::ColorPalette::BLACK;

    ////////////////////
    //   Text Scale   //
    ////////////////////
    double TextScale_Volume_x     = 1.7;
    double TextScale_Volume_y     = 1.7;
    double TextScale_volume_num_x = 1.0;
    double TextScale_volume_num_y = 1.0;
    double TextScale_Back_x       = 1.7;
    double TextScale_Back_y       = 1.7;

    Math::vec2 TextScale_Volume      = { TextScale_Volume_x, TextScale_Volume_y };
    Math::vec2 TextScale_volume_num  = { TextScale_volume_num_x, TextScale_volume_num_y };
    Math::vec2 TextScale_VSync       = { 1.7, 1.7 };
    Math::vec2 TextScale_UIToggle    = { 1.7, 1.7 };
    Math::vec2 TextScale_BuildMode   = { 1.7, 1.7 };
    Math::vec2 TextScale_Back        = { TextScale_Back_x, TextScale_Back_y };

    ////////////////////
    // Text Alignment //
    ////////////////////
    Math::ivec2 window_size = Engine::GetWindow().GetSize();
    int         center_x    = (window_size.x) / 2;
    int         center_y    = (window_size.y) / 2;
    int         Text_dist_x = 100;
    int         Text_dist_y = 75;

    int Padding_x        = 200;
    int Padding_y_Top    = 200;
    int Padding_y_Bottom = 100;
    int padding_y_Middle = (Engine::GetWindow().GetWindowSize().y - Padding_y_Top - Padding_y_Bottom) / static_cast<int>(Setting_Keys::Count);

    Math::vec2 Text_Position_Center_Bottom = { static_cast<double>(center_x), 0.0 + static_cast<double>(Padding_y_Bottom) };
    Math::vec2 Text_Position_Volume        = { static_cast<double>(center_x),
                                               static_cast<double>(window_size.y) - static_cast<double>(Padding_y_Top) - padding_y_Middle* static_cast<int>(Setting_Keys::Volume) };
    Math::vec2 Text_Position_volume_num    = { static_cast<double>(center_x), static_cast<double>(window_size.y) - static_cast<double>(Padding_y_Top) - padding_y_Middle + 150 };
    Math::vec2 Text_Position_Back = { static_cast<double>(center_x), static_cast<double>(window_size.y) - static_cast<double>(Padding_y_Top) - padding_y_Middle* static_cast<int>(Setting_Keys::Back) };


    // Positions for new items (fixed offsets from top, y-up coords)
    Math::vec2 Text_Position_VSync     = { static_cast<double>(center_x), static_cast<double>(window_size.y) - 310.0 };
    Math::vec2 Text_Position_UIToggle  = { static_cast<double>(center_x), static_cast<double>(window_size.y) - 395.0 };
    Math::vec2 Text_Position_BuildMode = { static_cast<double>(center_x), static_cast<double>(window_size.y) - 480.0 };

    /////////////////////
    //      Volume     //
    /////////////////////
    int  volume    = 1;
    int  volume_min = 0;
    int  volume_max = 5;
    bool vsyncOn   = true;

    static bool s_uiHidden;
    static bool s_buildMode;
    static bool s_fromGame;
    static bool s_isOpen;


    // Helper func
    void DrawOptions();
    void SetVolume(int scale);

    Dokev::Mouse mouse;
};