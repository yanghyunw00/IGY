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
#include "Dokev/Managers/DKTimeManager.hpp"

class WorkBuilding : public Building
{
protected:
    float                  productionRate;
    int                    resourceProduce;
    Dokev::Timer           productionTimer;
    class ResourceManager* resourceManager;

    // Static texture cache to prevent memory leaks
    static Dokev::Texture2D loggingCampTexture;
    static Dokev::Texture2D quarryTexture;
    static Dokev::Texture2D ironMineTexture;
    static Dokev::Texture2D granaryTexture;
    static Dokev::Texture2D missingTexture;
    //added most recently
    static Dokev::Texture2D loggingCampInterationTexture;
    static Dokev::Texture2D quarryInterationTexture;
    static Dokev::Texture2D ironMineInterationTexture;
    static Dokev::Texture2D granaryInterationTexture;

    static bool             texturesLoaded;

    static void LoadTextures();

public:
    WorkBuilding(Math::fvec2 pos, int w, int h, BuildingType bt);
    ~WorkBuilding() override;

    void SetResourceManager(ResourceManager* rm);

    void Update(float dt) override;
    void Draw(int viewIndex, const GridMap* map, int x, int y) override;

    std::string TypeName() override
    {
        return "WorkBuilding";
    }

    // Building interaction callbacks
    void OnConstructed() override;
    void OnWorkerArrived(Human* human) override;
    void OnWorkerLeft(Human* human) override;

    // Workplace functions
    void  AddWorker(int count);
    void  RemoveWorker(int count);
    int   GetProduction() const;
    float GetProductionRate() const;
    bool  IsFull() const;
    bool  HasWorker() const;
};
