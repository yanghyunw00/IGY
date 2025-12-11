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
#include "CS200/OffscreenFramebuffer.hpp"
#include "CS200/PostProcessingPipeline.hpp"
#include "OpenGL/Shader.hpp"

#include "Dokev/Graphics/DKTexture.hpp"
#include "Dokev/Graphics/DKCamera.hpp"
#include "Dokev/Input/DKMouseInput.hpp"
#include "Dokev/Graphics/DKAnimationSprite.hpp"
#include "Dokev/Graphics/DKParticleEmitter.hpp"
#include "Dokev/Graphics/DKParticles.hpp"
#include "Dokev/Managers/DKGameObjectManager.hpp"
#include "Game/Editor/DKGameWorldEditor.hpp"
#include "Dokev/Managers/DKMapManager.hpp"
#include "Dokev/Managers/DKUIManager.hpp"
#include "Game/UI/DKTutorialManager.hpp"
#include "Dokev/Graphics/DKRenderState.hpp"
#include "Dokev/State/DKGameState.hpp"
#include "Game/Input/DKMouseSelection.hpp"

#include "Engine/Vec2.hpp"
#include "Engine/GameState.hpp"
#include "Engine/Matrix.hpp"

#include "../Managers/ResourceManager.hpp"
#include "../Managers/BuildingCostManager.hpp"
#include "Game/UI/DKUserInterfaceEditor.hpp"

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
    //Dokev::AnimatedSprite smokeSprite;

    // Particle emitter demo
    //Dokev::ParticleEmitter<Dokev::Smoke> smokeEmitter;

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

    // Player UI
    UserInterfaceEditor playerUI;

    Math::ivec2 lastPlaceRoadPos = {-1,-1};
};