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
#include "Game/Managers/EventManager.hpp"


#include "Engine/GameState.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Vec2.hpp"


#include "../Managers/AchievementManager.hpp"
#include "../Managers/BuildingCostManager.hpp"
#include "../Managers/DayCycleManager.hpp"
#include "../Managers/WeatherCycleManager.hpp"
#include "../Managers/ResourceManager.hpp"
#include "Game/UI/DKUserInterfaceEditor.hpp"
#include "Game/UI/GameConsole.hpp"


class Human;

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
    void   UpdateTimeControls();
    double GetSimulationDeltaTime(double dt) const;
    void   SetSimulationSpeedPreset(std::size_t presetIndex);

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


    // Achievement system
    AchievementManager achievementManager;

    // Day/night cycle
    DayCycleManager dayCycle;
    int             previousDay = 1;

    // Weather cycle
    WeatherCycleManager    weatherCycle;
    OpenGL::CompiledShader rainShader;
    OpenGL::CompiledShader pointLightShader;
    GLuint                 rainVAO = 0;
    GLuint                 rainVBO = 0;
    float                  rainTime = 0.f;

    // Cloud system
    struct CloudPatch { float x, y, w, h, alpha; };
    std::vector<CloudPatch> clouds;
    float cloudVisOverride = -1.f;  // -1 = weather-driven

    bool        simulationPaused    = false;
    std::size_t simulationSpeedPresetIndex = 1;

    // Selected NPC (clicked by player)
    Human* selectedNPC = nullptr;

    //Event Manager
    EventManager eventManager;

    // Disaster Observatory evacuation
    bool  evacuateButtonVisible  = false;
    float evacuateButtonTimer    = 0.0f;
    bool  evacuationOrdered      = false;
    bool  evacuateButtonHovered  = false;
    bool  evacuateButtonClicked  = false;
    float evacuateClickFlash     = 0.0f;
    static constexpr float kEvacuateButtonDuration = 10.0f;

    // In-game console (no ImGui)
    GameConsole console;
};
