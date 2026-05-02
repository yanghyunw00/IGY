#pragma once
#include "Dokev/Graphics/DKTexture.hpp"
#include "Dokev/Managers/DKGameObjectManager.hpp"
#include "Game/Managers/DKCityMetrics.hpp"
#include "Game/World/DisasterEvent.hpp"

class GridMap;

class Fire : public DisasterEvent
{
private:
    Dokev::GameObjectManager* gameObjectManager;
    GridMap*                  gridMap;

    static Dokev::Texture2D fireTexture;

    static constexpr double maxDuration    = 10.0;
    static constexpr double damageInterval = 3.0; 
    static constexpr float  radius         = 50.0f;
    static constexpr int    fireDamage     = 3;

    double elapsed     = 0.0;
    double damageTimer = 0.0;
    std::vector<Building*> fireRange;


public:
    Fire(Dokev::GameObjectManager* gom, GridMap* map) : gameObjectManager(gom), gridMap(map) {}

    void OnStart() override;
    void OnTick(double dt) override;
    void OnEnd() override;
    bool IsFinished() const override;
    void Draw() const override;

    const char* GetName() const override
    {
        return "Fire";
    }

    float GetProgress() const override
    {
        return static_cast<float>(elapsed / maxDuration);
    }
};
