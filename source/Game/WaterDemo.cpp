/**
 * \file WaterDemo.cpp
 * \brief Water-heavy terrain demo
 */
#include "WaterDemo.hpp"

void WaterDemo::Load()
{
    SetMapVariant(MapVariant::WaterHeavy);
    DEMO::Load();
}
