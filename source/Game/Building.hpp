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
 #include "Dokev/DKGameObject.hpp"
 #include "GridMap.hpp"

 enum class BuildingType
 {
    // WORK
    Farm = 1,
    Office,


    // RESIDENT
    SmallHouse = 100
    
 };

class Building : public Dokev::GameObject
{
protected:
    Math::fvec2 gridPosition;
    int width, height;
    Dokev::Texture2D texture;
    bool isBuilt;
    unsigned int max_capacity;
    unsigned int current_capacity;
    int cost;
    float progress;
    BuildingType buildingType;

public:
    Building(Math::fvec2 pos, int w, int h);
    virtual ~Building();

    virtual void Update(float dt) = 0;
    virtual void Draw(int viewIndex, const GridMap* map) = 0;

    int GetCost() const;
    Math::fvec2 GetGrid() const;
    bool GetIsBuilt() const;
    unsigned int GetMaxCapacity() const;
    unsigned int GetCurrentCapacity() const;
    float GetProgress() const;
    BuildingType GetBuildingType() const;

    std::string TypeName() override {return "Building";}
    GameObjectType Type() override {return GameObjectType::Building;}
};