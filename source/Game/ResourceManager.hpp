#pragma once
#include "Dokev/DKComponent.hpp"
#include "Game/Resource.hpp"

class ResourceManager : public IComponent
{
public:
    int totalWood = 5;
    int totalStone = 5;
    int totalSteel = 5;

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

    int GetTotalResources() const
    {
        return totalWood + totalStone + totalSteel;
    }
};