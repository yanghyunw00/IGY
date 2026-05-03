#include "EventManager.hpp"
#include "Dokev/Core/DKUtility.hpp"
#include "Game/World/Drought.hpp"
#include "Game/World/Meteor.hpp"
#include "Game/World/Fire.hpp"

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
    else if(randomSeed == static_cast<int>(EventList::METEOR)) //Like earthquake
    {
        activeEvents.push_back(std::make_unique<Meteor>(cityMetrics, gameObjectManager, gridMap));
        activeEvents.back()->OnStart();
        if (meteorFunction) meteorFunction();
    }
    else if(randomSeed == static_cast<int>(EventList::FIRE))
    {
        activeEvents.push_back(std::make_unique<Fire>(gameObjectManager, gridMap));
        activeEvents.back()->OnStart();
    }
}

void EventManager::EventCycle(double dt)
{
    duration += dt;
    
    for(auto& e : activeEvents) e->OnTick(dt);

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
