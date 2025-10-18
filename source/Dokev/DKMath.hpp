/**
 * \file DKMath.hpp
 * \brief math manipulation functions
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

/*
Normalize(float value, float start, float end): Normalize float value to range.​

Normalize(double value, double start, double end): Normalize double value to range.​

Lerp(float start, float end, float amount): Linearly interpolate float between start and end by amount.

Lerp(double start, double end, double amount): Linearly interpolate double between start and end by amount.

vec2Length(vec2 v): Calculate length of 2D vector.

vec2LengthSqr(vec2 v): Calculate squared length of 2D vector.

vec2DotProduct(vec2 v1, vec2 v2): Calculate dot product of two 2D vectors.

vec2CrossProduct(vec2 v1, vec2 v2): Calculate scalar cross product of two 2D vectors.

vec2Distance(vec2 v1, vec2 v2): Calculate distance between two 2D points.

vec2DistanceSqr(vec2 v1, vec2 v2): Calculate squared distance between two 2D points.
*/
#pragma once

#include "Engine/Matrix.hpp"
#include "Engine/Vec2.hpp"
#include <iostream>


#define PI 3.14159265358979323f
#define EPSILON 0.000001f

using vec2 = Math::fvec2;

namespace Dokev
{
    struct DKMath
    {
        static float  Normalize(float value, float start, float end);
        static double Normalize(double value, double start, double end);

        static float Lerp(float start, float end, float amount);
        static int   fastFloor(float f);
        static float fade(float t);

        static float Length(vec2 v);
        static float LengthSquared(vec2 v);
        static float DotProduct(vec2 v1, vec2 v2);
        static float DotProduct(float x, float y, float z, float first, float second, float third);
        static float CrossProduct(vec2 v1, vec2 v2);
        static float Distance(vec2 v1, vec2 v2);
        static float DistanceSquared(vec2 v1, vec2 v2);

        static float gradiant(int hash, float x, float y, float z);
        static float PerlinNoise(float x, float y, float z, int wrapX = 256, int wrapY = 256, int wrapZ = 256, int seed = 0);
    };
}