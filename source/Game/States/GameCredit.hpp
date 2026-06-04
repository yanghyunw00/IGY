/**
 * \file GameCredit.hpp
 * \brief Displays the credits with a Star Wars-style scrolling image
 * \author Youngwoo Lee(youngwoo.lee)
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include "CS200/NDC.hpp"
#include "CS200/RGBA.hpp"
#include "CS200/RenderingAPI.hpp"
#include "Dokev/Graphics/DKTexture.hpp"
#include "Dokev/State/DKGameState.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Font.hpp"
#include "Engine/Input.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Vec2.hpp"
#include "Engine/Window.hpp"
#include <memory>

class GameCredit : public Dokev::GameState
{
public:
    void                  Load() override;
    void                  Update(double dt) override;
    void                  Unload() override;
    virtual gsl::czstring GetName() const override;
    void                  Draw() const override;
    void                  DrawImGui() override;
    void                  DrawHintText(const std::string& text, Math::vec2 position, CS200::RGBA color) const;

private:
    Dokev::Texture2D             creditTexture;
    std::unique_ptr<CS230::Font> hintFont;
    double           scrollY      = 0.0;
    float            imageScale   = 0.8f;
    double           scaledHeight = 0.0;
    bool             loaded       = false;

    const std::string spaceBarText = "SpaceBar for Fast Forward ";
    const std::string escapeText   = " ESC or ENTER for EXIT";

    static constexpr double SCROLL_SPEED = 140.0;
};
