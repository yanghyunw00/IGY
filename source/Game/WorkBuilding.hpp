/**
 * \file WorkBuilding.hpp
 * \brief Derived class of Building, manages workplace jobs and production
 * \author Dongyun Lee
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#pragma once

#include "Building.hpp"
#include "Dokev/DKTimeManager.hpp"

class WorkBuilding : public Building
{
protected:
    float productionRate;
    int   resourceProduce;
    Dokev::Timer productionTimer;
    class ResourceManager* resourceManager;

    // Static texture cache to prevent memory leaks
    static Dokev::Texture2D workshopTexture;
    static Dokev::Texture2D missingTexture;
    static bool texturesLoaded;

    static void LoadTextures();

public:
    WorkBuilding(Math::fvec2 pos, int w, int h, BuildingType bt);
    ~WorkBuilding() override;

    void SetResourceManager(ResourceManager* rm);

    void Update([[maybe_unused]] float dt) override;
    void Draw(int viewIndex, const GridMap* map) override;

    std::string TypeName() override {return "WorkBuilding";}

    // Workplace functions
    void  AddWorker(int count);
    void  RemoveWorker(int count);
    int   GetProduction() const;
    float GetProductionRate() const;
    bool  IsFull() const;
    bool  HasWorker() const;
};
