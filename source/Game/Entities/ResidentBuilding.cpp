/**
 * \file Resident.cpp
 * \brief Dervied class of Building, manage human tiredness
 * \author Dongyun Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "ResidentBuilding.hpp"
#include "Engine/Path.hpp"

Dokev::Texture2D ResidentBuilding::house2Texture;
Dokev::Texture2D ResidentBuilding::house2InteractionTexture;
Dokev::Texture2D ResidentBuilding::missingTexture;
bool             ResidentBuilding::texturesLoaded = false;

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
        house2Texture            = Dokev::LoadTexture(assets::locate_asset("Assets/images/building/house2.png").string());
        house2InteractionTexture = Dokev::LoadTexture(assets::locate_asset("Assets/images/building/house2_interation.png").string());
        missingTexture           = Dokev::LoadTexture(assets::locate_asset("Assets/images/textures/MissingTexture.png").string());
        texturesLoaded           = true;
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
            texture      = house2Texture;
            max_capacity = 2; // Each home can house 2 people
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
}

void ResidentBuilding::OnWorkerArrived(Human* human)
{
    (void)human;
    AddResident(1);
}

void ResidentBuilding::OnWorkerLeft(Human* human)
{
    (void)human;
    RemoveResident(1);
}

void ResidentBuilding::Update(float dt)
{
    UpdateConstruction(dt);

    if (!isBuilt)
    {
        return;
    }

    if (current_capacity > 0)
    {
        switch (buildingType)
        {
        case BuildingType::HOME:
            texture = house2InteractionTexture;
            break;
        default:
            break;
        }
    }
    else
    {
        switch (buildingType)
        {
        case BuildingType::HOME:
            texture = house2Texture;
            break;
        default:
            break;
        }
    }
}

void ResidentBuilding::Draw(int viewIndex, const GridMap* map, int x, int y)
{
    if (!map)
    {
        return;
    }

    CS200::RGBA tint = 0xFFFFFFFF;
    if (!isBuilt)
    {
        const auto alpha = static_cast<uint8_t>(128 + static_cast<int>(progress * 127.0f));
        tint             = ApplyAlpha(tint, alpha);
    }

    texture.Draw(x, y, tint);

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
