#pragma once
#include "Dokev/Graphics/DKTexture.hpp"
#include "Dokev/Graphics/DKShapes.hpp"
#include "Dokev/Managers/DKGameObjectManager.hpp"
#include "Game/Managers/DKCityMetrics.hpp"
#include "Game/World/DisasterEvent.hpp"
#include <vector>

class GridMap;
class Human;

class Fire : public DisasterEvent
{
private:
    Dokev::GameObjectManager* gameObjectManager;
    GridMap*                  gridMap;

    static Dokev::Texture2D fireTexture;

public:
    static void UnloadTexture();

    static constexpr double maxDuration    = 10.0;
    static constexpr double damageInterval = 3.0;
    static constexpr int    fireDamage     = 3;
    float radius = 50.0f;

    double elapsed     = 0.0;
    double damageTimer = 0.0;
    int    cx          = 0;
    int    cy          = 0;
    std::vector<Building*> fireRange;
    std::vector<Human*>    dispatched;


public:
    Fire(Dokev::GameObjectManager* gom, GridMap* map, float r = 50.0f) : gameObjectManager(gom), gridMap(map), radius(r) {}

    void OnStart() override;
    void OnTick(double dt) override;
    void OnEnd() override;
    bool IsFinished() const override;
    void Draw(int index) const override;

    void        AddFirefighter(Human* h)  { dispatched.push_back(h); }
    Math::fvec2 GetCenter()         const { return { static_cast<float>(cx), static_cast<float>(cy) }; }

    const char* GetName() const override
    {
        return "Fire";
    }

    float GetProgress() const override
    {
        return static_cast<float>(elapsed / maxDuration);
    }
};
