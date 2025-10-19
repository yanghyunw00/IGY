/**
 * \file DKUtility.cpp
 * \brief Random number generation and probability utilities
 * \author Dongyun Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "DKUtility.hpp"
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

bool        Dokev::ChanceRoll(float probability)
{

    std::random_device                 rd;
    std::mt19937                       gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);

    return dis(gen) < probability;
}

vec2 Dokev::GetRandomGridPosition(int gridWidth, int gridHeight)
{
    float x = static_cast<float>(GetRandomInt(0, gridWidth-1));
    float y = static_cast<float>(GetRandomInt(0, gridHeight-1));

    return vec2{x, y};
}

int   Dokev::GetRandomInt(int min, int max)
{
    return GetRandomValue(min, max);
}

float Dokev::GetRandomFloat(float min, float max)
{
    std::random_device                 rd;
    std::mt19937                       gen(rd());
    std::uniform_real_distribution<float> dis(min, max);

    return dis(gen);
}