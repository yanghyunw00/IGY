/**
 * \file MainMenu.cpp
 * \brief Displays the main screen
 * \author Youngwoo Lee(youngwoo.lee)
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "MainMenu.hpp"
#include "Dokev/Managers/DKMapManager.hpp"
#include "Engine/AudioManager.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Path.hpp"
#include "GameCredit.hpp"
#include "GamePlayState.hpp"
#include "OpenGL/GL.hpp"
#include "Setting.hpp"


// #include "Engine/Windwow.hpp"


//  MainMenu::MainMenu()
// {

// }

void MainMenu::Load()
{
    CS200::RenderingAPI::SetClearColor(0x0d0e20FF);

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
        Font_outlined = std::make_unique<CS230::Font>("Assets/fonts/DungGeunMo.ttf", 100);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    selectedVariant = MapManager::GetNextMapVariant();
    CS230::AudioManager::Instance().PlayMusic("Assets/sounds/title.mp3", true);

    // Background map
    bgMap.SetMapVariant(selectedVariant);
    bgMap.ReGenerateMap();
    bgCamera.SetZoom(0.3f);

    auto winSize = Engine::GetWindow().GetSize();

    // Center background camera on map center
    {
        const int  midX       = GRID_SIZE_X_HALF;
        const int  midY       = GRID_SIZE_Y_HALF;
        GridCell** view       = bgMap.GetViewArray(0);
        double     cellHeight = view[midY * GRID_SIZE_X + midX]->getHeight();

        int   screenX = bgMap.GetIsoOriginX() + (midX - midY) * TILE_WIDTH_HALF;
        int   screenY = bgMap.GetIsoOriginY() + (midX + midY) * TILE_HEIGHT_HALF;
        int   topY    = static_cast<int>(cellHeight) - screenY;

        float cx = static_cast<float>(screenX + TILE_WIDTH_HALF);
        float cy = static_cast<float>(topY    + TILE_HEIGHT_HALF);

        bgCamera.SetPosition({
            1.0f - cx * 2.0f / static_cast<float>(winSize.x),
            1.0f - cy * 2.0f / static_cast<float>(winSize.y)
        });
    }

    bgFBO.Initialize(winSize.x, winSize.y);
    blurPipeline.Initialize(winSize.x, winSize.y);

    auto blurShaderH = OpenGL::CreateShader(
        assets::locate_asset("Assets/shaders/simple.vert"),
        assets::locate_asset("Assets/shaders/gaussian-blur.frag"));
    blurPipeline.AddEffect(PostProcessingEffect("blur-h", PostProcessingEffect::Enable::True, std::move(blurShaderH),
        [w = winSize.x, h = winSize.y](const OpenGL::CompiledShader& s) {
            GL::Uniform2f(s.UniformLocations.at("uResolution"), static_cast<float>(w), static_cast<float>(h));
            GL::Uniform1i(s.UniformLocations.at("uHorizontal"), 1);
        }));

    auto blurShaderV = OpenGL::CreateShader(
        assets::locate_asset("Assets/shaders/simple.vert"),
        assets::locate_asset("Assets/shaders/gaussian-blur.frag"));
    blurPipeline.AddEffect(PostProcessingEffect("blur-v", PostProcessingEffect::Enable::True, std::move(blurShaderV),
        [w = winSize.x, h = winSize.y](const OpenGL::CompiledShader& s) {
            GL::Uniform2f(s.UniformLocations.at("uResolution"), static_cast<float>(w), static_cast<float>(h));
            GL::Uniform1i(s.UniformLocations.at("uHorizontal"), 0);
        }));

    bgBlitShader = OpenGL::CreateShader(
        assets::locate_asset("Assets/shaders/simple.vert"),
        assets::locate_asset("Assets/shaders/simple-texture.frag"));

    struct BgVert { float x, y, u, v; };
    const BgVert bgVerts[] = {{ -1.f, -1.f, 0.f, 0.f }, { 3.f, -1.f, 2.f, 0.f }, { -1.f, 3.f, 0.f, 2.f }};
    GL::GenBuffers(1, &bgBlitVBO);
    GL::BindBuffer(GL_ARRAY_BUFFER, bgBlitVBO);
    GL::BufferData(GL_ARRAY_BUFFER, sizeof(bgVerts), bgVerts, GL_STATIC_DRAW);
    GL::GenVertexArrays(1, &bgBlitVAO);
    GL::BindVertexArray(bgBlitVAO);
    GL::BindBuffer(GL_ARRAY_BUFFER, bgBlitVBO);
    GL::EnableVertexAttribArray(0);
    GL::VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(BgVert), nullptr);
    GL::EnableVertexAttribArray(1);
    GL::VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(BgVert), reinterpret_cast<void*>(2 * sizeof(float)));
    GL::BindVertexArray(0);
    GL::BindBuffer(GL_ARRAY_BUFFER, 0);
}

void MainMenu::Update(double dt)
{
    bgDayCycle.Update(static_cast<float>(dt * 8.0));

    TextColor_PlayIGY   = Dokev::ColorPalette::WHITE;
    TextColor_MapSelect = Dokev::ColorPalette::WHITE;
    TextColor_Exit      = Dokev::ColorPalette::WHITE;

    switch (current)
    {
        case Main_Keys::GamePlayKey:
            TextColor_PlayIGY   = Dokev::ColorPalette::HOTPINK;
            TextColor_MapSelect = Dokev::ColorPalette::WHITE;
            TextColor_Setting   = Dokev::ColorPalette::WHITE;
            TextColor_Credit    = Dokev::ColorPalette::WHITE;
            TextColor_Exit      = Dokev::ColorPalette::WHITE;
            break;

        case Main_Keys::MapSelectKey:
            TextColor_PlayIGY   = Dokev::ColorPalette::WHITE;
            TextColor_MapSelect = Dokev::ColorPalette::HOTPINK;
            TextColor_Setting   = Dokev::ColorPalette::WHITE;
            TextColor_Credit    = Dokev::ColorPalette::WHITE;
            TextColor_Exit      = Dokev::ColorPalette::WHITE;
            break;

        case Main_Keys::SettingKey:
            TextColor_PlayIGY   = Dokev::ColorPalette::WHITE;
            TextColor_MapSelect = Dokev::ColorPalette::WHITE;
            TextColor_Setting   = Dokev::ColorPalette::HOTPINK;
            TextColor_Credit    = Dokev::ColorPalette::WHITE;
            TextColor_Exit      = Dokev::ColorPalette::WHITE;
            break;

        case Main_Keys::CreditKey:
            TextColor_PlayIGY   = Dokev::ColorPalette::WHITE;
            TextColor_MapSelect = Dokev::ColorPalette::WHITE;
            TextColor_Setting   = Dokev::ColorPalette::WHITE;
            TextColor_Credit    = Dokev::ColorPalette::HOTPINK;
            TextColor_Exit      = Dokev::ColorPalette::WHITE;
            break;

        case Main_Keys::ExitKey:
            TextColor_PlayIGY   = Dokev::ColorPalette::WHITE;
            TextColor_MapSelect = Dokev::ColorPalette::WHITE;
            TextColor_Setting   = Dokev::ColorPalette::WHITE;
            TextColor_Credit    = Dokev::ColorPalette::WHITE;
            TextColor_Exit      = Dokev::ColorPalette::HOTPINK;
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
            case Main_Keys::MapSelectKey: CycleMapVariant(1); break;
            case Main_Keys::SettingKey:
                Engine::GetGameStateManager().PopState();
                Engine::GetGameStateManager().PushState<Setting>();
                return;
                break;
            case Main_Keys::CreditKey:
                Engine::GetGameStateManager().PopState();
                Engine::GetGameStateManager().PushState<GameCredit>();
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
    blurPipeline.Shutdown();
    bgFBO.Shutdown();
    OpenGL::DestroyShader(bgBlitShader);
    if (bgBlitVAO != 0) { GL::DeleteVertexArrays(1, &bgBlitVAO); bgBlitVAO = 0; }
    if (bgBlitVBO != 0) { GL::DeleteBuffers(1, &bgBlitVBO); bgBlitVBO = 0; }
}

gsl::czstring MainMenu::GetName() const
{
    return "MainMenu";
}

void MainMenu::Draw() const
{
    auto winSize = Engine::GetWindow().GetSize();

    // Render map to offscreen FBO
    bgFBO.BindForRendering();
    GL::ClearColor(0.53f, 0.81f, 0.92f, 1.0f);
    GL::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GL::Viewport(0, 0, winSize.x, winSize.y);

    auto& bgRenderer = Engine::GetRenderer2D(Dokev::RenderState::INSTANCED);
    bgRenderer.BeginScene(bgCamera.GetViewMatrix() * CS200::build_ndc_matrix(winSize));
    bgMap.IsometricDrawMap(0, Dokev::RenderState::INSTANCED, nullptr, nullptr);
    bgRenderer.EndScene();

    GL::BindFramebuffer(GL_FRAMEBUFFER, 0);

    // Blur passes then blit to screen
    GLuint blurredTex = blurPipeline.Apply(bgFBO.GetTexture());

    GL::BindFramebuffer(GL_FRAMEBUFFER, 0);
    GL::Viewport(0, 0, winSize.x, winSize.y);
    GL::Disable(GL_BLEND);
    GL::UseProgram(bgBlitShader.Shader);
    GL::ActiveTexture(GL_TEXTURE0);
    GL::BindTexture(GL_TEXTURE_2D, blurredTex);
    GL::Uniform1i(bgBlitShader.UniformLocations.at("uColorTexture"), 0);
    GL::BindVertexArray(bgBlitVAO);
    GL::DrawArrays(GL_TRIANGLES, 0, 3);
    GL::BindVertexArray(0);
    GL::BindTexture(GL_TEXTURE_2D, 0);
    GL::UseProgram(0);
    GL::Enable(GL_BLEND);

    // Draw UI text on top
    auto& renderer2d = Engine::GetRenderer2D();

    renderer2d.BeginScene(CS200::build_ndc_matrix(winSize));

    // Day cycle overlay (semi-transparent night/dawn/dusk tint)
    CS200::RGBA overlayColor = bgDayCycle.GetOverlayColor();
    if (overlayColor != CS200::CLEAR)
        Dokev::DrawRectangle(0, 0, winSize.x, winSize.y, overlayColor);

    ////////////////////////////////
    //           Title            //
    ////////////////////////////////

    DrawText(text_Title, Text_Position_Title, *Font_outlined, TextColor_Title, TextScale_Title);

    ////////////////////////////////
    //          PlayIGY           //
    ////////////////////////////////

    DrawText(text_PlayIGY, Text_Position_GamePlayKey, *Font_simple, TextColor_PlayIGY, TextScale_PlayIGY);


    ////////////////////////////////
    //        Map Select          //
    ////////////////////////////////
    DrawText(GetMapLabel(), Text_Position_MapSelectKey, *Font_simple, TextColor_MapSelect, TextScale_MapSelect);

    ////////////////////////////////
    //          Setting           //
    ////////////////////////////////

    DrawText(text_Setting, Text_Position_SettingKey, *Font_simple, TextColor_Setting, TextScale_Setting);

    ////////////////////////////////
    //           Credit           //
    ////////////////////////////////

    DrawText(text_Credit, Text_Position_CreditKey, *Font_simple, TextColor_Credit, TextScale_Credit);


    ////////////////////////////////
    //           Exit             //
    ////////////////////////////////

    DrawText(text_Exit, Text_Position_ExitKey, *Font_simple, TextColor_Exit, TextScale_Exit);

    ////////////////////////////////
    //    Futsal manager 2025     //
    ////////////////////////////////

    DrawText(text_Futsal_Manager_2025, Text_Position_Left_Bottom, *Font_simple, TextColor_Futsal_Manager_2025, TextScale_Futsal_Manager_2025);
    DrawText(text_MapInstructions, Math::vec2{ Text_Position_Left_Bottom.x, static_cast<double>(Padding_y_Bottom + 50) }, *Font_simple, TextColor_Futsal_Manager_2025, TextScale_Futsal_Manager_2025);

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
