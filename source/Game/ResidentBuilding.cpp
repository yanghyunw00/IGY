/**
 * \file Resident.cpp
 * \brief Dervied class of Building, manage human tiredness
 * \author Dongyun Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "ResidentBuilding.hpp"
#include "Engine/Path.hpp"

Dokev::Texture2D ResidentBuilding::homeTexture;
Dokev::Texture2D ResidentBuilding::missingTexture;
bool ResidentBuilding::texturesLoaded = false;

namespace
{
    CS200::RGBA ApplyAlpha(CS200::RGBA color, uint8_t alpha)
    {
        return (color & 0xFFFFFF00u) | alpha;
    }
}

void ResidentBuilding::LoadTextures()
{
    if (!texturesLoaded)
    {
        homeTexture = Dokev::LoadTexture(assets::locate_asset("Assets/images/textures/home.png").string());
        missingTexture = Dokev::LoadTexture(assets::locate_asset("Assets/images/textures/MissingTexture.png").string());
        texturesLoaded = true;
    }
}

ResidentBuilding::ResidentBuilding(Math::fvec2 pos, int w, int h, BuildingType bt) : Building(pos, w, h), satisfaction(1.0f), taxrate(10)
{
    LoadTextures(); 

    max_capacity     = 50;
    current_capacity = 0;
    cost             = 100;
    buildingType     = bt;

    switch (bt)
    {
        case BuildingType::HOME:
            texture      = homeTexture;
            max_capacity = 2;  // Each home can house 2 people
            cost         = 0;
            break;

        default:
            texture      = missingTexture;
            max_capacity = 0;
            cost         = 0;
            break;
    }
}

ResidentBuilding::~ResidentBuilding()
{
    // TODO
}

void ResidentBuilding::Update(float dt)
{
    UpdateConstruction(dt);
}

void ResidentBuilding::Draw(int viewIndex, const GridMap* map)
{
    if (!map)
    {
        return;
    }

    Math::ivec2 drawPos = CalculateScreenPosition(viewIndex, map);

    CS200::RGBA tint = 0xFFFFFFFF;
    if (!isBuilt)
    {
        const auto alpha = static_cast<uint8_t>(128 + static_cast<int>(progress * 127.0f));
        tint             = ApplyAlpha(tint, alpha);
    }

    texture.Draw(drawPos.x, drawPos.y, tint);

    if (!isBuilt)
    {
        DrawProgressBar(viewIndex, map);
    }
}

void ResidentBuilding::AddResident(int count)
{
    current_capacity += count;
    if (current_capacity > max_capacity)
    {
        current_capacity = max_capacity;
    }
}

void ResidentBuilding::RemoveResident(int count)
{
    if (count > static_cast<int>(current_capacity))
    {
        current_capacity = 0;
    }
    else
    {
        current_capacity -= count;
    }
}

int ResidentBuilding::GetTaxIncome() const
{
    return static_cast<int>(current_capacity * taxrate / 100);
}

float ResidentBuilding::GetSatisfaction() const
{
    return satisfaction;
}

bool ResidentBuilding::IsFull() const
{
    return current_capacity >= max_capacity;
}
