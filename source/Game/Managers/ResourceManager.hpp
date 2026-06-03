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
    int totalMoney = 1000;
    int totalFood = 0;
    int foodCapacity = 0;

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

    bool TrySpendFood(int amount)
    {
        assert(amount >= 0 && "Food spend amount cannot be negative");

        if (totalFood < amount)
        {
            return false;
        }

        totalFood -= amount;
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

    void AddFood(int amount)
    {
        assert(amount >= 0 && "Food amount cannot be negative");
        totalFood += amount;
        if (totalFood > foodCapacity)
            totalFood = foodCapacity;
    }

    void SetFood(int amount)
    {
        assert(amount >= 0 && "Food amount cannot be negative");
        totalFood = amount;
        if (totalFood > foodCapacity)
            totalFood = foodCapacity;
    }

    int GetFood() const { return totalFood; }
    int GetFoodCapacity() const { return foodCapacity; }

    void AddFoodCapacity(int capacity)
    {
        assert(capacity >= 0 && "Food capacity cannot be negative");
        foodCapacity += capacity;
    }

    void RemoveFoodCapacity(int capacity)
    {
        assert(capacity >= 0 && "Food capacity cannot be negative");
        foodCapacity -= capacity;
        if (foodCapacity < 0)
            foodCapacity = 0;
        if (totalFood > foodCapacity)
            totalFood = foodCapacity;
    }
};
