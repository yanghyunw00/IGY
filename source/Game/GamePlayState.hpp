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
#include "Human.hpp"
#include "Road.hpp"
#include <array>
#include <memory>

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
    std::unique_ptr<Human> human;

    // Road system
    std::unique_ptr<Road> road;

private:
    void                       updateInput();

};