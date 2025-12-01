/**
 * \file GamePlayState.hpp
 * \author Hyunwoo Yang
 * \author
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#pragma once
#include <array>
#include <memory>

#include "CS200/RGBA.hpp"

#include "Dokev/DKTexture.hpp"
#include "Dokev/DKCamera.hpp"
#include "Dokev/DKMouseInput.hpp"
#include "Dokev/DKAnimationSprite.hpp"
#include "Dokev/DKParticleEmitter.hpp"
#include "Dokev/DKParticles.hpp"
#include "Dokev/DKGameObjectManager.hpp"
#include "Dokev/DKGameWorldEditor.hpp"
#include "Dokev/DKMapManager.hpp"
#include "Dokev/DKUIManager.hpp"
#include "Dokev/DKTutorialManager.hpp"
#include "Dokev/DKRenderState.hpp"
#include "Dokev/DKGameState.hpp"
#include "Dokev/DKMouseSelection.hpp"

#include "Engine/Vec2.hpp"
#include "Engine/GameState.hpp"
#include "Engine/Matrix.hpp"

#include "Game/ResourceManager.hpp"
#include "Game/BuildingCostManager.hpp"

class GamePlayState : public Dokev::GameState
{
public:
    void          Load() override;
    void          Update(double dt) override;
    void          Unload() override;
    void          Draw() const override;
    void          DrawImGui();
    gsl::czstring GetName() const override;
    void          SaveGame(const std::string& filename);
    void          LoadGame(const std::string& filename);

private:
    Dokev::Texture2D testTexture;
    Dokev::Texture2D noiseTexture;
    Dokev::Texture2D noiseTexture2;

    //Iso grid
    int index = 0;

    Dokev::Camera camera;
    Dokev::Mouse mouse;

    // Smoke animation demo
    Dokev::AnimatedSprite smokeSprite;

    // Particle emitter demo
    Dokev::ParticleEmitter<Dokev::Smoke> smokeEmitter;

    // Immigration system
    float immigrationTimer = 0.0f;
    float immigrationInterval = 5.0f;
    Dokev::RenderState selected_renderer = Dokev::RenderState::INSTANCED;

    // World editor
    Dokev::GameWorldEditor worldEditor;
    bool saveGameRequested = false;
    bool loadGameRequested = false;

    // UI Manager
    Dokev::UIManager uiManager;

    // Tutorial Manager
    Dokev::TutorialManager tutorialManager;

    // Building cost manager
    BuildingCostManager buildingCostManager;

    //Mouse Selection
    Dokev::MouseSelection mouseSelection;
};