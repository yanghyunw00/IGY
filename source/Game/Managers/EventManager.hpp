#pragma once
#include <iostream>
#include "Game/Entities/Drought.hpp"

enum class EventList
{
    DROUGHT,
    METEOR,
    STORM
};

class EventManager
{
private:
 double duration;
 double maxTime;
 Drought drought;

public:

    EventManager() : duration(0), maxTime(5) {};

    //drought, meteor, storm
    void RandomEvent();
    void EventCycle(double dt);

};
