/**
 * \file WorkBuilding.cpp
 * \brief Derived class of Building, manages workplace jobs and production
 * \author Dongyun Lee
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "WorkBuilding.hpp"
#include "../Managers/ResourceManager.hpp"
#include "../Managers/WorkManager.hpp"
#include "CS200/RGBA.hpp"
#include "Engine/Path.hpp"
#include <algorithm>


Dokev::Texture2D WorkBuilding::loggingCampTexture;
Dokev::Texture2D WorkBuilding::quarryTexture;
Dokev::Texture2D WorkBuilding::ironMineTexture;
Dokev::Texture2D WorkBuilding::granaryTexture;
Dokev::Texture2D WorkBuilding::farmTexture;
Dokev::Texture2D WorkBuilding::cafeTexture;
Dokev::Texture2D WorkBuilding::restaurant2Texture;
Dokev::Texture2D WorkBuilding::workplace3Texture;
Dokev::Texture2D WorkBuilding::disasterObservatoryTexture;
Dokev::Texture2D WorkBuilding::casinoTexture;
Dokev::Texture2D WorkBuilding::eduBuildingTexture;

Dokev::Texture2D WorkBuilding::loggingCampInterationTexture;
Dokev::Texture2D WorkBuilding::quarryInterationTexture;
Dokev::Texture2D WorkBuilding::ironMineInterationTexture;
Dokev::Texture2D WorkBuilding::granaryInterationTexture;
Dokev::Texture2D WorkBuilding::farmInteractionTexture;
Dokev::Texture2D WorkBuilding::cafeInteractionTexture;
Dokev::Texture2D WorkBuilding::restaurant2InteractionTexture;

Dokev::Texture2D WorkBuilding::missingTexture;
bool             WorkBuilding::texturesLoaded = false;

Dokev::AnimatedSprite WorkBuilding::woodWorkshopAnim;
Dokev::AnimatedSprite WorkBuilding::rockWorkshopAnim;
Dokev::AnimatedSprite WorkBuilding::steelWorkshopAnim;

namespace
{
    CS200::RGBA ApplyAlpha(CS200::RGBA color, uint8_t alpha)
    {
        return (color & 0xFFFFFF00u) | alpha;
    }
}

void WorkBuilding::LoadTextures()
{
    if (!texturesLoaded)
    {
        loggingCampTexture = Dokev::LoadTexture(assets::locate_asset("Assets/resources/LoggingCamp.png").string());
        quarryTexture      = Dokev::LoadTexture(assets::locate_asset("Assets/resources/Quarry.png").string());
        ironMineTexture    = Dokev::LoadTexture(assets::locate_asset("Assets/resources/IronMine.png").string());
        granaryTexture     = Dokev::LoadTexture(assets::locate_asset("Assets/resources/Granary_2x2.png").string());
        farmTexture        = Dokev::LoadTexture(assets::locate_asset("Assets/resources/Farm.png").string());

        loggingCampInterationTexture = Dokev::LoadTexture(assets::locate_asset("Assets/resources/LoggingCamp_interaction.png").string());
        quarryInterationTexture = Dokev::LoadTexture(assets::locate_asset("Assets/resources/Quarry_interaction.png").string());
        ironMineInterationTexture = Dokev::LoadTexture(assets::locate_asset("Assets/resources/IronMine_interaction.png").string());
        granaryInterationTexture = Dokev::LoadTexture(assets::locate_asset("Assets/resources/Granary_2x2_interaction.png").string());
        farmInteractionTexture   = Dokev::LoadTexture(assets::locate_asset("Assets/resources/Farm.png").string());
        cafeTexture              = Dokev::LoadTexture(assets::locate_asset("Assets/resources/Cafe.png").string());
        restaurant2Texture       = Dokev::LoadTexture(assets::locate_asset("Assets/resources/restaurant2.png").string());
        cafeInteractionTexture       = Dokev::LoadTexture(assets::locate_asset("Assets/resources/Cafe_interaction.png").string());
        restaurant2InteractionTexture = Dokev::LoadTexture(assets::locate_asset("Assets/resources/restaurant2_interaction.png").string());

        workplace3Texture   = Dokev::LoadTexture(assets::locate_asset("Assets/images/building/workplace3.png").string());
        disasterObservatoryTexture   = Dokev::LoadTexture(assets::locate_asset("Assets/images/building/disaster_observatory.png").string());
        casinoTexture       = Dokev::LoadTexture(assets::locate_asset("Assets/images/building/casino2.png").string());
        eduBuildingTexture  = Dokev::LoadTexture(assets::locate_asset("Assets/images/building/edu_building.png").string());

        missingTexture = Dokev::LoadTexture(assets::locate_asset("Assets/images/textures/MissingTexture.png").string());

        woodWorkshopAnim  = Dokev::AnimatedSprite(assets::locate_asset("Assets/resources/LoggingCamp_Human_Interaction_Line.spt"));
        rockWorkshopAnim  = Dokev::AnimatedSprite(assets::locate_asset("Assets/resources/Quarry_Human_Interaction_Line.spt"));
        steelWorkshopAnim = Dokev::AnimatedSprite(assets::locate_asset("Assets/resources/IronWork_Human_Interaction_Line.spt"));

        texturesLoaded = true;
    }
}

void WorkBuilding::UnloadTextures()
{
    loggingCampTexture            = Dokev::Texture2D{};
    quarryTexture                 = Dokev::Texture2D{};
    ironMineTexture               = Dokev::Texture2D{};
    granaryTexture                = Dokev::Texture2D{};
    farmTexture                   = Dokev::Texture2D{};
    cafeTexture                   = Dokev::Texture2D{};
    restaurant2Texture            = Dokev::Texture2D{};
    workplace3Texture             = Dokev::Texture2D{};
    disasterObservatoryTexture             = Dokev::Texture2D{};
    casinoTexture                 = Dokev::Texture2D{};
    eduBuildingTexture            = Dokev::Texture2D{};
    missingTexture                = Dokev::Texture2D{};
    loggingCampInterationTexture  = Dokev::Texture2D{};
    quarryInterationTexture       = Dokev::Texture2D{};
    ironMineInterationTexture     = Dokev::Texture2D{};
    granaryInterationTexture      = Dokev::Texture2D{};
    farmInteractionTexture        = Dokev::Texture2D{};
    cafeInteractionTexture        = Dokev::Texture2D{};
    restaurant2InteractionTexture = Dokev::Texture2D{};
    woodWorkshopAnim              = Dokev::AnimatedSprite{};
    rockWorkshopAnim              = Dokev::AnimatedSprite{};
    steelWorkshopAnim             = Dokev::AnimatedSprite{};
    texturesLoaded                = false;
}

void WorkBuilding::UpdateAnimations(double dt)
{
    woodWorkshopAnim.Update(dt);
    rockWorkshopAnim.Update(dt);
    steelWorkshopAnim.Update(dt);
}

Dokev::AnimatedSprite* WorkBuilding::GetWorkerAnimation() const
{
    switch (buildingType)
    {
        case BuildingType::WOOD_WORKSHOP: return &woodWorkshopAnim;
        case BuildingType::ROCK_WORKSHOP: return &rockWorkshopAnim;
        case BuildingType::STEEL_WORKSHOP: return &steelWorkshopAnim;
        default: return nullptr;
    }
}

WorkBuilding::WorkBuilding(Math::fvec2 pos, int w, int h, int hp, BuildingType bt) : Building(pos, w, h,hp), productionRate(1.0f), resourceProduce(10), productionTimer(1.0f), resourceManager(nullptr)
{
    LoadTextures();

    max_capacity     = 2;
    current_capacity = 0;
    cost             = 150;

    buildingType = bt;

    switch (bt)
    {
        case BuildingType::WOOD_WORKSHOP:
            texture          = loggingCampTexture;
            max_capacity     = 2;
            cost             = 0;
            maintenanceCost  = 10;
            break;

        case BuildingType::ROCK_WORKSHOP:
            texture          = quarryTexture;
            max_capacity     = 2;
            cost             = 0;
            maintenanceCost  = 10;
            break;

        case BuildingType::STEEL_WORKSHOP:
            texture          = ironMineTexture;
            max_capacity     = 2;
            cost             = 0;
            maintenanceCost  = 15;
            break;

        case BuildingType::GRANARY:
            texture         = granaryTexture;
            max_capacity    = 2;
            cost            = 0;
            maintenanceCost = 5;
            break;

        case BuildingType::FARM:
            texture         = farmTexture;
            max_capacity    = 2;
            cost            = 0;
            maintenanceCost = 10;
            break;

        case BuildingType::CAFE:
            texture         = cafeTexture;
            max_capacity    = 2;
            cost            = 0;
            maintenanceCost = 10;
            break;

        case BuildingType::RESTAURANT2:
            texture         = restaurant2Texture;
            max_capacity    = 2;
            cost            = 0;
            maintenanceCost = 12;
            break;

        case BuildingType::WORKPLACE3:
            texture         = workplace3Texture;
            max_capacity    = 3;
            cost            = 0;
            maintenanceCost = 12;
            break;

        case BuildingType::DISASTER_OBSERVATORY:
            texture         = disasterObservatoryTexture;
            max_capacity    = 4;
            cost            = 0;
            maintenanceCost = 15;
            break;

        case BuildingType::CASINO:
            texture         = casinoTexture;
            max_capacity    = 3;
            cost            = 0;
            maintenanceCost = 18;
            break;

        case BuildingType::PENTHOUSE:
            texture         = eduBuildingTexture;
            max_capacity    = 3;
            cost            = 0;
            maintenanceCost = 15;
            break;

        default:
            texture      = missingTexture;
            max_capacity = 0;
            cost         = 0;
            break;
    }
}

WorkBuilding::~WorkBuilding()
{
    WorkManager::GetInstance().RemoveAllJobsForBuilding(this);
    if (buildingType == BuildingType::GRANARY && resourceManager != nullptr)
        resourceManager->RemoveFoodCapacity(200);
}

void WorkBuilding::OnConstructed()
{
    if (buildingType == BuildingType::WORKPLACE3)
        return;

    for (unsigned int i = 0; i < max_capacity; ++i)
        WorkManager::GetInstance().AddJob(WorkType::Gathering, gridPosition, this);

    if (buildingType == BuildingType::GRANARY && resourceManager != nullptr)
        resourceManager->AddFoodCapacity(200);
}

void WorkBuilding::OnWorkerArrived(Human* human)
{
    (void)human;
    AddWorker(1);
}

void WorkBuilding::OnWorkerLeft(Human* human)
{
    (void)human;
    RemoveWorker(1);
    WorkManager::GetInstance().AddJob(WorkType::Gathering, gridPosition, this);
}

void WorkBuilding::Update(float dt)
{
    UpdateConstruction(dt);
    UpdateLightTimer(dt);

    if (!isBuilt)
    {
        return;
    }

    if (resourceManager != nullptr && HasWorker() && productionTimer.Update(dt))
    {
        int amountToProduce = static_cast<int>(current_capacity * productionRate);

        switch (buildingType)
        {
            case BuildingType::WOOD_WORKSHOP: resourceManager->AddResource(ResourceType::Wood, amountToProduce); break;
            case BuildingType::ROCK_WORKSHOP: resourceManager->AddResource(ResourceType::Stone, amountToProduce); break;
            case BuildingType::STEEL_WORKSHOP: resourceManager->AddResource(ResourceType::Steel, amountToProduce); break;
            case BuildingType::GRANARY: break;
            case BuildingType::FARM:
                if (resourceManager->GetFoodCapacity() > 0)
                    resourceManager->AddFood(amountToProduce);
                break;
            default: break;
        }
    }

    if (HasWorker())
    {
        switch (buildingType)
        {
        case BuildingType::WOOD_WORKSHOP:
            texture = loggingCampInterationTexture;
            break;
        case BuildingType::ROCK_WORKSHOP:
            texture = quarryInterationTexture;
            break;
        case BuildingType::STEEL_WORKSHOP:
            texture = ironMineInterationTexture;
            break;
        case BuildingType::GRANARY:
            texture = granaryInterationTexture;
            break;
        case BuildingType::FARM:
            texture = farmInteractionTexture;
            break;
        case BuildingType::CAFE:
            texture = cafeInteractionTexture;
            break;
        case BuildingType::RESTAURANT2:
            texture = restaurant2InteractionTexture;
            break;
        default:
            break;
        }
    }
    else
    {
        switch (buildingType)
        {
        case BuildingType::WOOD_WORKSHOP:
            texture = loggingCampTexture;
            break;
        case BuildingType::ROCK_WORKSHOP:
            texture = quarryTexture;
            break;
        case BuildingType::STEEL_WORKSHOP:
            texture = ironMineTexture;
            break;
        case BuildingType::GRANARY:
            texture = granaryTexture;
            break;
        case BuildingType::FARM:
            texture = farmTexture;
            break;
        case BuildingType::CAFE:
            texture = cafeTexture;
            break;
        case BuildingType::RESTAURANT2:
            texture = restaurant2Texture;
            break;
        default:
            break;
        }
    }
}

void WorkBuilding::Draw(int viewIndex, const GridMap* map, [[maybe_unused]] int x, [[maybe_unused]] int y)
{
    if (!map)
    {
        return;
    }

    CS200::RGBA tint = CS200::WHITE;

    if (!isBuilt)
    {
        const auto alpha = static_cast<uint8_t>(128 + static_cast<int>(progress * 127.0f));
        tint             = ApplyAlpha(tint, alpha);
    }

    Math::ivec2 drawPos = CalculateScreenPosition(viewIndex, map);

    Dokev::AnimatedSprite* workerAnim = GetWorkerAnimation();
    if (isBuilt && HasWorker() && workerAnim != nullptr && workerAnim->GetSpriteSheet().IsValid())
    {
        workerAnim->DrawAtPosition(drawPos.x, drawPos.y);
    }
    else
    {
        texture.Draw(drawPos.x, drawPos.y, tint);
    }

    if (!isBuilt)
    {
        DrawProgressBar(viewIndex, map);
    }
}

void WorkBuilding::AddWorker(int count)
{
    current_capacity += count;
    if (current_capacity > max_capacity)
    {
        current_capacity = max_capacity;
    }
}

void WorkBuilding::RemoveWorker(int count)
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

int WorkBuilding::GetProduction() const
{
    return static_cast<int>(current_capacity * resourceProduce * productionRate);
}

float WorkBuilding::GetProductionRate() const
{
    return productionRate;
}

void WorkBuilding::SetProductionRate(float rate)
{
    productionRate = rate;
}

bool WorkBuilding::IsFull() const
{
    return current_capacity >= max_capacity;
}

bool WorkBuilding::HasWorker() const
{
    return current_capacity > 0;
}

void WorkBuilding::SetResourceManager(ResourceManager* rm)
{
    resourceManager = rm;
}
