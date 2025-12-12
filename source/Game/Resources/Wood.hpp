#pragma once
#include "Resource.hpp"

class Wood : public Resource
{
public:
    Wood(int x, int y, int amount);

    std::string GetName() const override
    {
        return "Wood";
    }

    float GetMiningSpeed() const override
    {
        return 1.2f;
    }
};