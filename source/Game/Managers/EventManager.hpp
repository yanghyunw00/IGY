#pragma once
#include <functional>
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
    EARTHQUAKE,
    NONE1
};

class EventManager
{
private:
    double duration = 0;
    double maxTime  = 45.0;

    Dokev::CityMetrics*       cityMetrics       = nullptr;
    Dokev::GameObjectManager* gameObjectManager = nullptr;
    GridMap*                  gridMap           = nullptr;

    std::vector<std::unique_ptr<DisasterEvent>> activeEvents;
    std::function<void()>                       earthquakeFunction;
    bool   debugDrawRadius    = false;
    float  fireRadius         = 20.0f;
    float  earthquakeRadius   = 13.0f;
    double redispatchTimer    = 0.0;
    static constexpr double redispatchInterval = 2.0;

public:
    void Initialize(Dokev::CityMetrics* metrics, Dokev::GameObjectManager* gom, GridMap* map);
    void SetEarthquakeCallback(std::function<void()> callback) { earthquakeFunction = std::move(callback); }
    void SetDebugDrawRadius(bool b) { debugDrawRadius = b; }
    void SetFireRadius(float r)       { fireRadius = r; }
    void SetEarthquakeRadius(float r) { earthquakeRadius = r; }
    float GetFireRadius()       const { return fireRadius; }
    float GetEarthquakeRadius() const { return earthquakeRadius; }

    void RandomEvent();
    void EventCycle(double dt);
    void ForceEvent();
    void ForceEventByType(EventList type);
    void DispatchFirefighters();

    void Draw(int index) const
    {
        for (const auto& e : activeEvents)
        {
            e->SetDebugDraw(debugDrawRadius);
            e->Draw(index);
        }
    }

    const std::vector<std::unique_ptr<DisasterEvent>>& GetActiveEvents() const { return activeEvents; }
    double GetDuration()  const { return duration; }
    double GetMaxTime()   const { return maxTime; }
    int    GetEventCount() const { return static_cast<int>(activeEvents.size()); }
};
