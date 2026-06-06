#include "EventManager.hpp"
#include "Dokev/Core/DKUtility.hpp"
#include "Game/Entities/Building.hpp"
#include "Game/Entities/Human.hpp"
#include "Game/World/Drought.hpp"
#include "Game/World/Earthquake.hpp"
#include "Game/World/Fire.hpp"
#include "Game/Common/BuildingType.hpp"

void EventManager::Initialize(Dokev::CityMetrics* metrics, Dokev::GameObjectManager* gom, GridMap* map)
{
    cityMetrics       = metrics;
    gameObjectManager = gom;
    gridMap           = map;
}

void EventManager::ForceEvent()
{
    for (auto& e : activeEvents) e->OnEnd();
    activeEvents.clear();
    duration      = 0;
    warningActive = false;
    warningFired  = false;
    RandomEvent();
    PrerollNextEvent();
}

void EventManager::ForceEventByType(EventList type)
{
    for (auto& e : activeEvents) e->OnEnd();
    activeEvents.clear();
    duration = 0;

    if (type == EventList::FIRE)
    {
        activeEvents.push_back(std::make_unique<Fire>(gameObjectManager, gridMap, fireRadius));
        activeEvents.back()->OnStart();
        DispatchFirefighters();
    }
    else if (type == EventList::DROUGHT)
    {
        activeEvents.push_back(std::make_unique<Drought>(cityMetrics, gameObjectManager));
        activeEvents.back()->OnStart();
    }
    else if (type == EventList::EARTHQUAKE)
    {
        activeEvents.push_back(std::make_unique<Earthquake>(cityMetrics, gameObjectManager, gridMap, earthquakeRadius));
        activeEvents.back()->OnStart();
        if (earthquakeFunction) earthquakeFunction();
    }
}

void EventManager::PrerollNextEvent()
{
    // Always pre-roll a real disaster so a natural event is guaranteed to occur
    // and the early-warning prediction matches what actually spawns.
    switch (Dokev::GetRandomInt(0, 2))
    {
        case static_cast<int>(EventList::DROUGHT):    nextEventType = EventList::DROUGHT;    break;
        case static_cast<int>(EventList::EARTHQUAKE): nextEventType = EventList::EARTHQUAKE; break;
        default:                                      nextEventType = EventList::FIRE;       break;
    }
}

void EventManager::RandomEvent()
{
    // Spawn the pre-rolled disaster so the natural event always matches the
    // early-warning prediction. Fall back to a fresh roll if none was prepared.
    if (nextEventType == EventList::NONE1)
        PrerollNextEvent();

    ForceEventByType(nextEventType);
}

void EventManager::EventCycle(double dt)
{
    for(auto& e : activeEvents) e->OnTick(dt);

    bool hasFire = false;
    for (const auto& e : activeEvents)
        if (dynamic_cast<const Fire*>(e.get())) { hasFire = true; break; }

    if (hasFire)
    {
        redispatchTimer += dt;
        if (redispatchTimer >= redispatchInterval)
        {
            DispatchFirefighters();
            redispatchTimer = 0.0;
        }
    }
    else
    {
        redispatchTimer = 0.0;
    }

    std::erase_if(activeEvents, [] (const auto& event)
    {
        if(event->IsFinished())
        {
            event->OnEnd();
            return true;
        }
        return false;
    });

    // While any disaster is active, freeze the next-disaster timer at 0.
    if (!activeEvents.empty())
    {
        duration = 0;
        return;
    }

    // Advance the next-disaster countdown only when no event is active.
    duration += dt;

    double effectiveMax = GetEffectiveMaxTime();

    // Observatory early warning: 10s before disaster
    if (observatoryCount >= 1 && !warningFired && activeEvents.empty())
    {
        if (duration > effectiveMax - 10.0)
        {
            warningFired  = true;
            warningActive = true;
            if (nextEventType == EventList::NONE1)
                PrerollNextEvent();
        }
    }

    if(duration > effectiveMax && activeEvents.empty())
    {
        warningActive = false;
        warningFired  = false;
        RandomEvent();
        duration = 0;
        PrerollNextEvent(); // pre-roll for the next cycle
    }
}

void EventManager::DispatchFirefighters()
{
    if (!gameObjectManager || !gridMap) return;

    Fire* fire = nullptr;
    for (auto& e : activeEvents)
        if (auto* f = dynamic_cast<Fire*>(e.get()))
            { fire = f; break; }
    if (!fire) return;

    Math::fvec2 fireCenter     = fire->GetCenter();
    const float coverageRadius = 30.0f;

    for (const auto& building : gameObjectManager->GetBuildings())
    {
        if (building->GetBuildingType() != BuildingType::WORKPLACE3) continue;
        if (!building->GetIsBuilt()) continue;

        auto  bpos = building->GetGrid();
        float dx   = bpos.x - fireCenter.x;
        float dy   = bpos.y - fireCenter.y;
        if (dx * dx + dy * dy > coverageRadius * coverageRadius) continue;

        // Dispatch one idle/resting human near this fire station
        for (auto& human : gameObjectManager->GetHumans())
        {
            if (human->IsFirefighting()) continue;
            HumanState hs = human->GetState();
            if (hs != HumanState::Idle && hs != HumanState::Resting && hs != HumanState::FindingDestination) continue;

            auto  hpos = human->GetGridPosition();
            float hdx  = hpos.x - bpos.x;
            float hdy  = hpos.y - bpos.y;
            if (hdx * hdx + hdy * hdy > 10.0f * 10.0f) continue;

            human->DispatchToFire(fireCenter, gridMap);
            fire->AddFirefighter(human.get());
            break; // one firefighter per station
        }
    }
}
