/**
 * \file MainMenu.hpp
 * \brief Displays the main screen
 * \author Youngwoo Lee(youngwoo.lee)
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include "../Entities/Road.hpp"
#include "../World/GridMap.hpp"
#include "Dokev/Managers/DKGameObjectManager.hpp"
#include "CS200/IRenderer2D.hpp"
#include "CS200/NDC.hpp"
#include "CS200/OffscreenFramebuffer.hpp"
#include "CS200/PostProcessingPipeline.hpp"
#include "CS200/RGBA.hpp"
#include "CS200/RenderingAPI.hpp"
#include "Dokev/Core/DKColor.hpp"
#include "Dokev/Graphics/DKCamera.hpp"
#include "Dokev/Graphics/DKShapes.hpp"
#include "Dokev/Graphics/DKTexture.hpp"
#include "Dokev/State/DKGameState.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Font.hpp"
#include "Engine/Input.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Vec2.hpp"
#include "Engine/Window.hpp"
#include "Game/Managers/DayCycleManager.hpp"
#include "OpenGL/Shader.hpp"
#include <GL/glew.h>
#include <memory>
#include <string>

class MainMenu : public Dokev::GameState
{
public:
    void                  Load() override;
    void                  Update(double dt) override;
    void                  Unload() override;
    virtual gsl::czstring GetName() const override;
    void                  Draw() const override;
    void                  DrawImGui() override;
    void                  DrawText(const std::string& text, const Math::vec2& position, CS230::Font& font, CS200::RGBA color, Math::vec2 textscale) const;
    void                  CycleMapVariant(int direction);
    void                  SetupBackground();
    std::string           GetMapLabel() const;

private:
    std::unique_ptr<CS230::Font> Font_simple;
    std::unique_ptr<CS230::Font> Font_outlined;

    // std::shared_ptr<CS230::Texture> Title;
    // std::shared_ptr<CS230::Texture> PlayIGY;
    // std::shared_ptr<CS230::Texture> Exit;
    // std::shared_ptr<CS230::Texture> Futsal_Manager_2025;

    const std::string text_Title            = "I G Y";
    const std::string text_PlayIGY          = "PlayIGY";
    const std::string text_MapSelect        = "Map";
    const std::string text_Setting          = "Setting";
    const std::string text_Credit           = "Credit";
    const std::string text_Exit             = "Exit";
    const std::string text_Futsal_Manager_2025 = "Futsal manager 2025";
    const std::string text_MapInstructions  = "Use Left / Right to change map";

    enum Main_Keys
    {

        GamePlayKey,
        MapSelectKey,
        SettingKey,
        CreditKey,
        ExitKey,
        Count
    };

    ////////////////////
    //   Text Color   //
    ////////////////////
    CS200::RGBA TextColor_Title            = Dokev::ColorPalette::WHITE;
    CS200::RGBA TextColor_PlayIGY          = Dokev::ColorPalette::WHITE;
    CS200::RGBA TextColor_MapSelect        = Dokev::ColorPalette::WHITE;
    CS200::RGBA TextColor_Setting          = Dokev::ColorPalette::WHITE;
    CS200::RGBA TextColor_Credit           = Dokev::ColorPalette::WHITE;
    CS200::RGBA TextColor_Exit             = Dokev::ColorPalette::WHITE;
    CS200::RGBA TextColor_Futsal_Manager_2025 = Dokev::ColorPalette::WHITE;

    ////////////////////
    //   Text Scale   //
    ////////////////////
    double TextScale_Title_x = 1.0;
    double TextScale_Title_y = 1.08;

    double TextScale_PlayIGY_x   = 1.0;
    double TextScale_PlayIGY_y   = 1.0;
    double TextScale_MapSelect_x = 0.95;
    double TextScale_MapSelect_y = 0.95;
    double TextScale_Setting_x   = 1.0;
    double TextScale_Setting_y   = 1.0;
    double TextScale_Credit_x    = 1.0;
    double TextScale_Credit_y    = 1.0;
    double TextScale_Exit_x      = 1.0;
    double TextScale_Exit_y      = 0.95;

    double TextScale_Futsal_Manager_2025_x = 0.4;
    double TextScale_Futsal_Manager_2025_y = 0.4;

    Math::vec2 TextScale_Title            = { TextScale_Title_x, TextScale_Title_y };
    Math::vec2 TextScale_PlayIGY          = { TextScale_PlayIGY_x, TextScale_PlayIGY_y };
    Math::vec2 TextScale_MapSelect        = { TextScale_MapSelect_x, TextScale_MapSelect_y };
    Math::vec2 TextScale_Setting          = { TextScale_Setting_x, TextScale_Setting_y };
    Math::vec2 TextScale_Credit           = { TextScale_Credit_x, TextScale_Credit_y };
    Math::vec2 TextScale_Exit             = { TextScale_Exit_x, TextScale_Exit_y };
    Math::vec2 TextScale_Futsal_Manager_2025 = { TextScale_Futsal_Manager_2025_x, TextScale_Futsal_Manager_2025_y };

    ////////////////////
    // Text Alignment //
    ////////////////////
    int Text_dist_x = 100;
    int Text_dist_y = 75;

    int Padding_x                    = 200;
    int Padding_y_Top                = 200;
    int Padding_y_Title_to_Menu      = 100;
    int Padding_y_Bottom             = 100;
    int Padding_y_Middle             = (Engine::GetWindow().GetWindowSize().y - Padding_y_Top - Padding_y_Bottom) / static_cast<int>(Main_Keys::Count);
    int Padding_y_Bottom_to_Teamname = 50;


    Math::ivec2 window_size = Engine::GetWindow().GetSize();
    int         center_x    = (window_size.x) / 2;
    int         center_y    = (window_size.y) / 2;

    int PlayIGY_x   = (window_size.x) / 2; // 1
    int PlayIGY_y   = (window_size.y) / 2 + Text_dist_y;
    int MapSelect_x = (window_size.x) / 2; // 2
    int MapSelect_y = (window_size.y) / 2;
    int Exit_x      = (window_size.x) / 2; // 3
    int Exit_y      = (window_size.y) / 2 - Text_dist_y;

    Math::vec2 Text_Position_Center_Bottom = { static_cast<double>(center_x), 0.0 + static_cast<double>(Padding_y_Bottom) };
    Math::vec2 Text_Position_Left_Bottom   = { static_cast<double>(window_size.x) - Padding_x, 0.0 + static_cast<double>(Padding_y_Bottom) };

    Math::vec2 Text_Position_Title = { static_cast<double>(center_x), static_cast<double>(window_size.y) - static_cast<double>(Padding_y_Top) };

    Math::vec2 Text_Position_GamePlayKey  = { static_cast<double>(center_x), static_cast<double>(window_size.y) - static_cast<double>(Padding_y_Top) - Padding_y_Title_to_Menu -
                                                                                 Padding_y_Middle* static_cast<int>(Main_Keys::GamePlayKey) };
    Math::vec2 Text_Position_MapSelectKey = { static_cast<double>(center_x), static_cast<double>(window_size.y) - static_cast<double>(Padding_y_Top) - Padding_y_Title_to_Menu -
                                                                                 Padding_y_Middle* static_cast<int>(Main_Keys::MapSelectKey) };
    Math::vec2 Text_Position_SettingKey   = { static_cast<double>(center_x), static_cast<double>(window_size.y) - static_cast<double>(Padding_y_Top) - Padding_y_Title_to_Menu -
                                                                                 Padding_y_Middle* static_cast<int>(Main_Keys::SettingKey) };
    Math::vec2 Text_Position_CreditKey    = { static_cast<double>(center_x), static_cast<double>(window_size.y) - static_cast<double>(Padding_y_Top) - Padding_y_Title_to_Menu -
                                                                                 Padding_y_Middle* static_cast<int>(Main_Keys::CreditKey) };
    Math::vec2 Text_Position_ExitKey      = { static_cast<double>(center_x),
                                              static_cast<double>(window_size.y) - static_cast<double>(Padding_y_Top) - Padding_y_Title_to_Menu - Padding_y_Middle* static_cast<int>(Main_Keys::ExitKey) };
    Math::vec2 Text_Position_Teamname     = { static_cast<double>(center_x), static_cast<double>(Padding_y_Bottom - Padding_y_Bottom_to_Teamname) };

    Main_Keys  current         = Main_Keys::GamePlayKey;
    MapVariant selectedVariant = MapVariant::Balanced;

    // Play transition
    bool   transitionActive   = false;
    double transitionTimer    = 0.0;
    static constexpr double transitionDuration = 1.5;

    // Animated background map
    std::unique_ptr<GridMap>                  bgMap;
    std::unique_ptr<Road>                     bgRoad;
    std::unique_ptr<Dokev::GameObjectManager> bgGameObjMgr;
    DayCycleManager            bgDayCycle;
    mutable OffscreenFramebuffer bgFBO;
    mutable PostProcessingPipeline blurPipeline;
    Dokev::Camera              bgCamera;
    OpenGL::CompiledShader     bgBlitShader;
    GLuint                     bgBlitVAO = 0;
    GLuint                     bgBlitVBO = 0;
};
