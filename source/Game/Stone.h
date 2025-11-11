#pragma once
#include "Resource.h"

class Stone : public Resource
{
public:
    Stone(int x, int y, int amount);
    std::string GetName() const override { return "Stone"; }
    float GetMiningSpeed() const override { return 0.8f; }
};