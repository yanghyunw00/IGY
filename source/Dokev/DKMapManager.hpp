/**
 * \file DKMapManager.hpp
 * \brief 
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#pragma once
#include "Dokev/DKComponent.hpp"
#include "Game/GridMap.hpp"
#include "Game/Road.hpp"
#include <memory>
#include "Game/Resource.h"
#include <vector>

class MapManager : public IComponent
{
public:
    MapManager();
    ~MapManager() = default;

    void Initialize();

    GridMap* GetMap() { return &map; }
    Road* GetRoad() { return road.get(); }

    const GridMap* GetMap() const { return &map; }
    const Road* GetRoad() const { return road.get(); }

    const std::vector<std::unique_ptr<Resource>>& GetResources() const { return m_resources; }

private:
    void SpawnResources(); 

    GridMap map;
    std::unique_ptr<Road> road;
    
    std::vector<std::unique_ptr<Resource>> m_resources;
};