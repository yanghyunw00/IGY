/**
 * \file DKUtility.hpp
 * \brief Random number generation and probability utilities
 * \author Dongyun Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include "DKMath.hpp"

namespace Dokev
{
    int  GetRandomValue(int min, int max);
    bool ChanceRoll(float probability);
    vec2 GetRandomGridPosition(int gridWidth, int gridHeight);

    int   GetRandomInt(int min, int max);
    float GetRandomFloat(float min, float max);

}