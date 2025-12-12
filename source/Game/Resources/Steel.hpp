#pragma once
#include "Resource.hpp"

class Steel : public Resource
{
public:
    Steel(int x, int y, int amount);

    std::string GetName() const override
    {
        return "Steel";
    }

    float GetMiningSpeed() const override
    {
        return 0.6f;
    }
};