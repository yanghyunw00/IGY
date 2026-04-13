/**
 * \file ResourceManager.hpp
 * \brief
 * \author
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include "../Resources/Resource.hpp"
#include "Dokev/GameObject/DKComponent.hpp"
#include <cassert>


class ResourceManager : public IComponent
{
public:
    int totalMoney = 15;

    void AddResource(ResourceType type, int amount)
    {
        (void)type;
        assert(amount >= 0 && "Resource amount cannot be negative");
        totalMoney += amount;
        assert(totalMoney >= 0 && "Money cannot be negative");
    }

    bool TrySpend(int amount)
    {
        assert(amount >= 0 && "Spend amount cannot be negative");

        if (totalMoney < amount)
        {
            return false;
        }

        totalMoney -= amount;
        return true;
    }

    void SetMoney(int money)
    {
        assert(money >= 0 && "Money cannot be negative");
        totalMoney = money;
    }

    void SetResources(int money)
    {
        SetMoney(money);
    }

    void SetResources(int wood, int stone, int steel)
    {
        SetMoney(wood + stone + steel);
    }

    int GetTotalResources() const
    {
        return totalMoney;
    }
};
