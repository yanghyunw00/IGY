#pragma once
#include "Dokev/Managers/DKGameObjectManager.hpp"
#include "Game/Managers/DKCityMetrics.hpp"
#include "Game/World/DisasterEvent.hpp"

class Drought : public DisasterEvent
{
private:
    Dokev::CityMetrics*       cityMetrics;
    Dokev::GameObjectManager* gameObjectManager;

    static constexpr float  productionPenalty = 0.3f;
    static constexpr double maxDuration       = 30.0;

    double elapsed = 0.0;

public:
    Drought(Dokev::CityMetrics* metrics, Dokev::GameObjectManager* gom) : cityMetrics(metrics), gameObjectManager(gom)
    {
    }

    void OnStart() override;
    void OnTick(double dt) override;
    void OnEnd() override;
    bool IsFinished() const override;
    void Draw(int index) const override;

    const char* GetName() const override
    {
        return "Drought";
    }

    float GetProgress() const override
    {
        return static_cast<float>(elapsed / maxDuration);
    }
};
