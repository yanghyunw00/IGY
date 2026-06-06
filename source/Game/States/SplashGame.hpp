/**
 * \file SplashGame.hpp
 * \author Dongyun Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#pragma once
#include "Dokev/Graphics/DKTexture.hpp"
#include "Dokev/State/DKGameState.hpp"

class SplashGame : public Dokev::GameState
{
public:
    SplashGame();
    void                  Load() override;
    void                  Update(double dt) override;
    void                  Unload() override;
    virtual gsl::czstring GetName() const override;
    void                  Draw() const override;
    void                  DrawImGui() override;

private:
    double           timercounter = 0.0;
    double           timerLimit   = 3.0;
    Dokev::Texture2D logoTexture;
    bool             loaded = false;
};
