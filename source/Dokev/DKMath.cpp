/**
 * \file DKMath.cpp
 * \brief implementation math functions
 * \author Dongyun Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "DKMath.hpp"
#include <cmath>

float Dokev::DKMath::Normalize(float value, float start, float end)
{
    return (value - start) / (end - start);
}

double Dokev::DKMath::Normalize(double value, double start, double end)
{
    return (value - start) / (end - start);
}

float Dokev::DKMath::Lerp(float start, float end, float amount)
{
    return start + amount * (end - start);
}


int Dokev::DKMath::fastFloor(float f)
{
    int ai = (int)f;
    return (f < ai) ? ai - 1 : ai;
}

float Dokev::DKMath::fade(float t)
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float Dokev::DKMath::Length(vec2 v)
{
    return sqrtf((v.x * v.x) + (v.y * v.y));
}

float Dokev::DKMath::LengthSquared(vec2 v)
{
    return (v.x * v.x) + (v.y * v.y);
}

float Dokev::DKMath::DotProduct(vec2 v1, vec2 v2)
{
    return (v1.x * v2.x + v1.y * v2.y);
}

float Dokev::DKMath::DotProduct(float x, float y, float z, float first, float second, float third)
{
    return first * x + second * y + third * z;
}

float Dokev::DKMath::CrossProduct(vec2 v1, vec2 v2)
{
    return (v1.x * v2.y - v1.y * v2.x);
}

float Dokev::DKMath::Distance(vec2 v1, vec2 v2)
{
    return sqrtf((v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y));
}

float Dokev::DKMath::DistanceSquared(vec2 v1, vec2 v2)
{
    return ((v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y));
}

float Dokev::DKMath::gradiant(int hash, float x, float y, float z)
{
    int h = hash & 15;

    float u, v;

    if (h < 8) // determine  the u value
    {
        u = x;
    }
    else
    {
        u = y;
    }

    if (h < 4) // v can be xyz depends on h value
    {
        v = y;
    }
    else if (h == 12 || h == 14)
    {
        v = x;
    }
    else
    {
        v = z;
    }

    // determine the sign
    if ((h & 1) != 0)
    {
        u = -u;
    }

    if ((h & 2) != 0)
    {
        v = -v;
    }

    return u + v;
}

float Dokev::DKMath::PerlinNoise(float x, float y, float z, int wrapX, int wrapY, int wrapZ, int seed)
{
      static const int perm[256] = {
          151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
          8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
          35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,
          134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
          55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,
          18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,
          250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
          189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,
          172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,
          228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,
          107,49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,
          138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
      };

      unsigned int mask_x = (wrapX - 1) & 255;
      unsigned int mask_y = (wrapY - 1) & 255;
      unsigned int mask_z = (wrapZ - 1) & 255;

      int cell_x = fastFloor(x);
      int cell_y = fastFloor(y);
      int cell_z = fastFloor(z);

      int grid_x0 = cell_x & mask_x;
      int grid_x1 = (cell_x + 1) & mask_x;
      int grid_y0 = cell_y & mask_y;
      int grid_y1 = (cell_y + 1) & mask_y;
      int grid_z0 = cell_z & mask_z;
      int grid_z1 = (cell_z + 1) & mask_z;

      float rel_x = x - cell_x;
      float rel_y = y - cell_y;
      float rel_z = z - cell_z;

      float smooth_x = fade(rel_x);
      float smooth_y = fade(rel_y);
      float smooth_z = fade(rel_z);

      int hash_x0 = perm[(grid_x0 + seed) & 255];
      int hash_x1 = perm[(grid_x1 + seed) & 255];

      int hash_xy00 = perm[(hash_x0 + grid_y0) & 255];
      int hash_xy01 = perm[(hash_x0 + grid_y1) & 255];
      int hash_xy10 = perm[(hash_x1 + grid_y0) & 255];
      int hash_xy11 = perm[(hash_x1 + grid_y1) & 255];

      float corner000 = gradiant(perm[(hash_xy00 + grid_z0) & 255], rel_x, rel_y, rel_z);
      float corner001 = gradiant(perm[(hash_xy00 + grid_z1) & 255], rel_x, rel_y, rel_z - 1);
      float corner010 = gradiant(perm[(hash_xy01 + grid_z0) & 255], rel_x, rel_y - 1, rel_z);
      float corner011 = gradiant(perm[(hash_xy01 + grid_z1) & 255], rel_x, rel_y - 1, rel_z - 1);
      float corner100 = gradiant(perm[(hash_xy10 + grid_z0) & 255], rel_x - 1, rel_y, rel_z);
      float corner101 = gradiant(perm[(hash_xy10 + grid_z1) & 255], rel_x - 1, rel_y, rel_z - 1);
      float corner110 = gradiant(perm[(hash_xy11 + grid_z0) & 255], rel_x - 1, rel_y - 1, rel_z);
      float corner111 = gradiant(perm[(hash_xy11 + grid_z1) & 255], rel_x - 1, rel_y - 1, rel_z - 1);

      float blend_z00 = Lerp(corner000, corner001, smooth_z);
      float blend_z01 = Lerp(corner010, corner011, smooth_z);
      float blend_z10 = Lerp(corner100, corner101, smooth_z);
      float blend_z11 = Lerp(corner110, corner111, smooth_z);

      float blend_y0 = Lerp(blend_z00, blend_z01, smooth_y);
      float blend_y1 = Lerp(blend_z10, blend_z11, smooth_y);

      return Lerp(blend_y0, blend_y1, smooth_x);
}
