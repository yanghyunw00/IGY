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
Dokev::Texture2D ResidentBuilding::streetLightTexture;
Dokev::Texture2D ResidentBuilding::parkTexture;
Dokev::Texture2D ResidentBuilding::forestTexture;
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
        streetLightTexture       = Dokev::LoadTexture(assets::locate_asset("Assets/resources/street_light.png").string());
        parkTexture              = Dokev::LoadTexture(assets::locate_asset("Assets/images/building/park.png").string());
        forestTexture            = Dokev::LoadTexture(assets::locate_asset("Assets/images/building/forest.png").string());
        texturesLoaded           = true;
    }
}

void ResidentBuilding::UnloadTextures()
{
    house2Texture            = Dokev::Texture2D{};
    missingTexture           = Dokev::Texture2D{};
    house2InteractionTexture = Dokev::Texture2D{};
    streetLightTexture       = Dokev::Texture2D{};
    parkTexture              = Dokev::Texture2D{};
    forestTexture            = Dokev::Texture2D{};
    texturesLoaded           = false;
}

ResidentBuilding::ResidentBuilding(Math::fvec2 pos, int w, int h, int hp_, BuildingType bt) : Building(pos, w, h, hp_), satisfaction(1.0f), taxrate(10)
{
    LoadTextures();

    max_capacity     = 2;
    current_capacity = 0;
    cost             = 100;
    buildingType     = bt;

    switch (bt)
    {
        case BuildingType::HOME:
            texture         = house2Texture;
            max_capacity    = 2;
            cost            = 0;
            maintenanceCost = 5;
            break;

        case BuildingType::STREET_LIGHT:
            texture      = streetLightTexture;
            max_capacity = 0;
            cost         = 0;
            break;

        case BuildingType::PARK:
            texture         = parkTexture;
            max_capacity    = 0;
            cost            = 0;
            maintenanceCost = 3;
            break;

        case BuildingType::FOREST:
            texture         = forestTexture;
            max_capacity    = 0;
            cost            = 0;
            maintenanceCost = 2;
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
    UpdateLightTimer(dt);

    if (!isBuilt)
    {
        return;
    }

    if (current_capacity > 0)
    {
        switch (buildingType)
        {
            case BuildingType::HOME: texture = house2InteractionTexture; break;
            default: break;
        }
    }
    else
    {
        switch (buildingType)
        {
            case BuildingType::HOME: texture = house2Texture; break;
            default: break;
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

    if (buildingType == BuildingType::PARK)
    {
        Math::ivec2 drawPos = CalculateScreenPosition(viewIndex, map);
        texture.Draw(drawPos.x, drawPos.y, tint);
    }
    else if(buildingType == BuildingType::FOREST)
    {
        drawY = y - TILE_HEIGHT;
        texture.Draw(drawPos.x, drawPos.y, tint);
    }
    else
    {
        int drawX = (buildingType == BuildingType::STREET_LIGHT) ? x + TILE_WIDTH_HALF / 2 : x;
        int drawY = (buildingType == BuildingType::STREET_LIGHT) ? y - TILE_HEIGHT_HALF : y;
        texture.Draw(drawX, drawY, tint);
    }

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
