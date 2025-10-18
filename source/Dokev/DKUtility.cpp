#include "DKUtility.hpp"
#include "Dokev/DKMath.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Logger.hpp"
#include <cmath>
#include <iostream>
#include <random>

int Dokev::GetRandomValue(int min, int max)
{
    if (min < 0 || max == 0 || max > 999999)
    {
        Engine::GetLogger().LogError("Invalid Input: min < 0, max == 0, max > 999999");
        return 0; // For now, just terminate program.
    }

    std::random_device                 rd;
    std::mt19937                       gen(rd());
    std::uniform_int_distribution<int> dis(min, max);

    int result = dis(gen);

    return result;
}