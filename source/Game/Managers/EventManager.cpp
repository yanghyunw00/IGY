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
    duration = 0;
    RandomEvent();
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

void EventManager::RandomEvent()
{
    int randomSeed = Dokev::GetRandomInt(0,4);

    // if(randomSeed == static_cast<int>(EventList::FIRE))
    // {
    //     activeEvents.push_back(std::make_unique<Fire>(gameObjectManager, gridMap));
    //     activeEvents.back()->OnStart();
    // }

    if(randomSeed == static_cast<int>(EventList::DROUGHT))
    {
        activeEvents.push_back(std::make_unique<Drought>(cityMetrics, gameObjectManager));
        activeEvents.back()->OnStart();
    }
    else if(randomSeed == static_cast<int>(EventList::EARTHQUAKE))
    {
        activeEvents.push_back(std::make_unique<Earthquake>(cityMetrics, gameObjectManager, gridMap));
        activeEvents.back()->OnStart();
        if (earthquakeFunction) earthquakeFunction();
    }
    else if(randomSeed == static_cast<int>(EventList::FIRE))
    {
        activeEvents.push_back(std::make_unique<Fire>(gameObjectManager, gridMap, fireRadius));
        activeEvents.back()->OnStart();
        DispatchFirefighters();
    }
}

void EventManager::EventCycle(double dt)
{
    duration += dt;

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

    if(duration > maxTime && activeEvents.empty())
    {
        RandomEvent();
        duration = 0;
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
