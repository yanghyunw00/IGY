/**
 * \file ResourceManager.hpp
 * \brief
 * \author
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include "Dokev/GameObject/DKComponent.hpp"
#include <cassert>
#include <map>
#include <string>


class ResourceManager : public IComponent
{
public:
    int totalMoney = 1000;
    int totalFood = 0;
    int foodCapacity = 0;

    struct DayStats
    {
        std::map<std::string, int> income;
        std::map<std::string, int> upkeep;

        int TotalIncome() const  { int t = 0; for (const auto& [k,v] : income) t += v; return t; }
        int TotalUpkeep() const  { int t = 0; for (const auto& [k,v] : upkeep) t += v; return t; }
    };

    struct FoodDayStats
    {
        std::map<std::string, int> production;
        std::map<std::string, int> consumption;

        int TotalProduction()  const { int t = 0; for (const auto& [k,v] : production)  t += v; return t; }
        int TotalConsumption() const { int t = 0; for (const auto& [k,v] : consumption) t += v; return t; }
    };

    DayStats     lastDay;
    FoodDayStats lastFoodDay;

    void AddNamedIncome(const std::string& name, int amount)
    {
        assert(amount >= 0);
        totalMoney += amount;
        currentDay.income[name] += amount;
    }

    void AddNamedUpkeep(const std::string& name, int amount)
    {
        currentDay.upkeep[name] += amount;
    }

    void AddNamedFoodProduction(const std::string& name, int amount)
    {
        assert(amount >= 0);
        currentFoodDay.production[name] += amount;
    }

    void AddNamedFoodConsumption(const std::string& name, int amount)
    {
        assert(amount >= 0);
        currentFoodDay.consumption[name] += amount;
    }

    void RecordDayEnd()
    {
        lastDay     = currentDay;
        lastFoodDay = currentFoodDay;
        currentDay     = {};
        currentFoodDay = {};
    }

    const DayStats&     GetCurrentDayStats()     const { return currentDay; }
    const FoodDayStats& GetCurrentFoodDayStats() const { return currentFoodDay; }

    void AddMoney(int amount)
    {
        assert(amount >= 0 && "Money amount cannot be negative");
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

private:
    DayStats     currentDay;
    FoodDayStats currentFoodDay;
};
