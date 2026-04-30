#include "EventManager.hpp"
#include "Dokev/Core/DKUtility.hpp"

void EventManager::RandomEvent()
{
    int randomSeed = Dokev::GetRandomInt(0,2);

    if(randomSeed == static_cast<int>(EventList::DROUGHT))
    {
        drought.print();
    }
    else if(randomSeed == static_cast<int>(EventList::METEOR))
    {
        std::cout << "mmm" << std::endl;
    }
    else if(randomSeed == static_cast<int>(EventList::STORM))
    {
        std::cout << "SSS" << std::endl;
    }
}

void EventManager::EventCycle(double dt)
{
    duration += dt;

    if(duration > maxTime)
    {
        RandomEvent();
        duration = 0;
    }
}
