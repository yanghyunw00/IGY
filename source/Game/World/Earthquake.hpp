#pragma once
#include "Dokev/Graphics/DKAnimationSprite.hpp"
#include "Dokev/Graphics/DKShapes.hpp"
#include "Dokev/Managers/DKGameObjectManager.hpp"
#include "Game/Managers/DKCityMetrics.hpp"
#include "Game/World/DisasterEvent.hpp"

class GridMap;

class Earthquake : public DisasterEvent
{
private:
    Dokev::CityMetrics*       cityMetrics;
    Dokev::GameObjectManager* gameObjectManager;
    GridMap*                  gridMap;

    Dokev::AnimatedSprite sprite;
    int drawX = 0;
    int drawY = 0;
    int cx    = 0;
    int cy    = 0;

    static constexpr float  radius      = 10.0f;
    static constexpr double maxDuration = 6.0;
    double elapsed = 0.0;

public:
    Earthquake(Dokev::CityMetrics* metrics, Dokev::GameObjectManager* gom, GridMap* map)
        : cityMetrics(metrics), gameObjectManager(gom), gridMap(map) {}

    void OnStart() override;
    void OnTick(double dt) override { elapsed += dt; sprite.Update(elapsed); }
    void OnEnd() override;
    void Draw() const override;

    bool IsFinished() const override { return elapsed >= maxDuration; }
    const char* GetName() const override { return "Earthquake"; }
    float GetProgress() const override { return static_cast<float>(elapsed / maxDuration); }
};
