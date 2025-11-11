#pragma once
#include "Dokev/DKComponent.hpp"
#include "Game/Resource.h"

class ResourceManager : public IComponent
{
public:
    int totalWood = 0;
    int totalStone = 0;
    int totalSteel = 0;

    void AddResource(ResourceType type, int amount)
    {
        switch (type)
        {
            case ResourceType::Wood:  totalWood += amount; break;
            case ResourceType::Stone: totalStone += amount; break;
            case ResourceType::Steel: totalSteel += amount; break;
        }
    }

    void SetResources(int wood, int stone, int steel)
    {
        totalWood = wood;
        totalStone = stone;
        totalSteel = steel;
    }
};