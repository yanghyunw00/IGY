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

#include "CS200/OffscreenFramebuffer.hpp"
#include "CS200/PostProcessingPipeline.hpp"
#include "CS200/RGBA.hpp"
#include "OpenGL/Shader.hpp"


#include "Dokev/Graphics/DKAnimationSprite.hpp"
#include "Dokev/Graphics/DKCamera.hpp"
#include "Dokev/Graphics/DKParticleEmitter.hpp"
#include "Dokev/Graphics/DKParticles.hpp"
#include "Dokev/Graphics/DKRenderState.hpp"
#include "Dokev/Graphics/DKTexture.hpp"
#include "Dokev/Input/DKMouseInput.hpp"
#include "Dokev/Managers/DKGameObjectManager.hpp"
#include "Dokev/Managers/DKMapManager.hpp"
#include "Dokev/Managers/DKUIManager.hpp"
#include "Dokev/State/DKGameState.hpp"
#include "Game/Editor/DKGameWorldEditor.hpp"
#include "Game/Input/DKMouseSelection.hpp"
#include "Game/UI/DKTutorialManager.hpp"


#include "Engine/GameState.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Vec2.hpp"


#include "../Managers/AchievementManager.hpp"
#include "../Managers/BuildingCostManager.hpp"
#include "../Managers/ResourceManager.hpp"
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


    // Iso grid
    int index = 0;

    Dokev::Camera camera;
    Dokev::Mouse  mouse;

    // Smoke animation demo
    // Dokev::AnimatedSprite smokeSprite;

    // Particle emitter demo
    // Dokev::ParticleEmitter<Dokev::Smoke> smokeEmitter;

    // Immigration system
    float              immigrationTimer    = 0.0f;
    float              immigrationInterval = 5.0f;
    Dokev::RenderState selected_renderer   = Dokev::RenderState::INSTANCED;

    // World editor
    Dokev::GameWorldEditor worldEditor;
    bool                   saveGameRequested = false;
    bool                   loadGameRequested = false;

    // UI Manager
    Dokev::UIManager uiManager;

    // Tutorial Manager
    Dokev::TutorialManager tutorialManager;

    // Building cost manager
    BuildingCostManager buildingCostManager;

    // Mouse Selection
    Dokev::MouseSelection mouseSelection;

    // Player UI
    UserInterfaceEditor playerUI;

    Math::ivec2 lastPlaceRoadPos    = { -1, -1 };
    int         roadBuildSoundCount = 0;


    //Camera Shaking
    bool       isShaking     = false;
    float      shakeAmount   = 0.001f;
    float      shakeDuration = 50.0f;

    // Achievement system
    AchievementManager achievementManager;
};