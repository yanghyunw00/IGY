/**
 * \file Building.hpp
 * \brief Base class for all building objcect
 * \author Dongyun Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once
 #include "Dokev/DKMath.hpp"
 #include "Dokev/DKTexture.hpp"

class Building
{
protected:
    vec2 gridPosition;
    int width, height;
    Dokev::Texture2D texture;
    bool isBuilt;
    unsigned int max_capacity;
    unsigned int current_capacity;
    int cost;

public:
    Building(vec2 pos, int w, int h);
    virtual ~Building();

    virtual void Update(float dt) = 0;
    virtual void Draw() = 0;

    int GetCost() const;
    vec2 GetGrid() const;
    bool GetIsBuilt() const;
    unsigned int GetMaxCapacity() const;
    unsigned int GetCurrentCapacity() const;
};