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
    double maxTime  = 35.0;

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

    // Observatory system
    int        observatoryCount  = 0;
    bool       warningFired      = false;
    bool       warningActive     = false;
    EventList  nextEventType     = EventList::NONE1;

public:
    void Initialize(Dokev::CityMetrics* metrics, Dokev::GameObjectManager* gom, GridMap* map);
    void SetEarthquakeCallback(std::function<void()> callback) { earthquakeFunction = std::move(callback); }
    void SetDebugDrawRadius(bool b) { debugDrawRadius = b; }
    void SetFireRadius(float r)       { fireRadius = r; }
    void SetEarthquakeRadius(float r) { earthquakeRadius = r; }
    float GetFireRadius()       const { return fireRadius; }
    float GetEarthquakeRadius() const { return earthquakeRadius; }

    void PrerollNextEvent();
    void RandomEvent();
    void EventCycle(double dt);
    void ForceEvent();
    void ForceEventByType(EventList type);
    void DispatchFirefighters();

    void SetObservatoryCount(int count) { observatoryCount = count; }
    bool      IsWarningActive()   const { return warningActive; }
    EventList GetNextEventType()  const { return nextEventType; }
    double    GetEffectiveMaxTime() const
    {
        if (observatoryCount >= 5)
            return 60.0 + (observatoryCount - 5);
        return maxTime;
    }

    void Draw(int index) const
    {
        const bool showDisasterLocation = debugDrawRadius || observatoryCount >= 5;
        for (const auto& e : activeEvents)
        {
            e->SetDebugDraw(showDisasterLocation);
            e->Draw(index);
        }
    }

    const std::vector<std::unique_ptr<DisasterEvent>>& GetActiveEvents() const { return activeEvents; }
    double GetDuration()  const { return duration; }
    double GetMaxTime()   const { return maxTime; }
    int    GetEventCount() const { return static_cast<int>(activeEvents.size()); }

};
