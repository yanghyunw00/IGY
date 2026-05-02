#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include "Game/World/DisasterEvent.hpp"
#include "Game/Managers/DKCityMetrics.hpp"
#include "Dokev/Managers/DKGameObjectManager.hpp"

class GridMap;

enum class EventList
{
    FIRE,
    DROUGHT,
    METEOR,
    NONE1
};

class EventManager
{
private:
    double duration = 0;
    double maxTime  = 30.0;

    Dokev::CityMetrics*       cityMetrics       = nullptr;
    Dokev::GameObjectManager* gameObjectManager = nullptr;
    GridMap*                  gridMap           = nullptr;

    std::vector<std::unique_ptr<DisasterEvent>> activeEvents;

public:
    void Initialize(Dokev::CityMetrics* metrics, Dokev::GameObjectManager* gom, GridMap* map);

    void RandomEvent();
    void EventCycle(double dt);
    void ForceEvent();

    void Draw() const { for (const auto& e : activeEvents) e->Draw(); }

    const std::vector<std::unique_ptr<DisasterEvent>>& GetActiveEvents() const { return activeEvents; }
    double GetDuration()  const { return duration; }
    double GetMaxTime()   const { return maxTime; }
    int    GetEventCount() const { return static_cast<int>(activeEvents.size()); }
};
