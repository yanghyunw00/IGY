#pragma once
#include "../Resources/Resource.hpp"
#include "Dokev/GameObject/DKComponent.hpp"
#include <cassert>


class ResourceManager : public IComponent
{
public:
    int totalWood  = 5;
    int totalStone = 5;
    int totalSteel = 5;

    void AddResource(ResourceType type, int amount)
    {
        assert(amount >= 0 && "Resource amount cannot be negative");

        switch (type)
        {
            case ResourceType::Wood: totalWood += amount; break;
            case ResourceType::Stone: totalStone += amount; break;
            case ResourceType::Steel: totalSteel += amount; break;
        }

        assert(totalWood >= 0 && "Wood cannot be negative");
        assert(totalStone >= 0 && "Stone cannot be negative");
        assert(totalSteel >= 0 && "Steel cannot be negative");
    }

    void SetResources(int wood, int stone, int steel)
    {
        totalWood  = wood;
        totalStone = stone;
        totalSteel = steel;
    }

    int GetTotalResources() const
    {
        return totalWood + totalStone + totalSteel;
    }
};