/**
 * \file GamePlayState.hpp
 * \author Hyunwoo Yang
 * \author
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "CS200/RGBA.hpp"
#include "Engine/GameState.hpp"
#include "Engine/Matrix.hpp"
#include "Dokev/DKTexture.hpp"
#include "Dokev/DKCamera.hpp"
#include "Dokev/DKMouseInput.hpp"
#include "Dokev/DKAnimationSprite.hpp"
#include "Dokev/DKParticleEmitter.hpp"
#include "Engine/Vec2.hpp"
#include "GridMap.hpp"
//#include "Human.hpp"
#include "Road.hpp"
#include <array>
#include <memory>
#include "GameObjectManager.hpp"
#include "MovementSystem.hpp"
#include "RenderSystem.hpp"
#include "AISystem.hpp"

class GamePlayState : public CS230::GameState
{
public:
    void          Load() override;
    void          Update() override;
    void          Unload() override;
    void          Draw() const override;
    void          DrawImGui() override;
    gsl::czstring GetName() const override;

private:
    Dokev::Texture2D testTexture;
    Dokev::Texture2D noiseTexture;
    Dokev::Texture2D noiseTexture2;

    GridMap map;

    //Iso grid
    int index = 1;

    Dokev::Camera camera;
    Dokev::Mouse mouse;

    // Smoke animation demo
    Dokev::AnimatedSprite smokeSprite;

    // Particle emitter demo
    std::unique_ptr<Dokev::ParticleEmitter> smokeEmitter;

    // Human pathfinding demo
    //std::vector<Human> human;

    // Road system
    std::unique_ptr<Road> road;

    // Performance tracking
    bool vsyncEnabled = true;

    // Immigration system
    float immigrationTimer = 0.0f;
    mutable float immigrationInterval = 5.0f; 


    std::unique_ptr<GameObjectManager> m_goManager;
    std::unique_ptr<MovementSystem> m_movementSystem;
    std::unique_ptr<RenderSystem> m_renderSystem;
    std::unique_ptr<AISystem> m_aiSystem;

private:
    void                        updateInput();
    void                        CreateImmigrantGameObject(GridMap* map);
    Math::fvec2                 FindClosestWalkableTile(GridMap* map, Math::fvec2 startPos) const;

};