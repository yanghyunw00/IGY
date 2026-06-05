/**
 * \file DKMapManager.hpp
 * \brief
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#pragma once
#include "Dokev/GameObject/DKComponent.hpp"
#include "Game/Entities/Road.hpp"
#include "Game/World/GridMap.hpp"
#include <memory>


class MapManager : public IComponent
{
public:
    MapManager();
    ~MapManager() = default;

    void              Initialize();
    static void       SetNextMapVariant(MapVariant variant);
    static MapVariant GetNextMapVariant();
    static void       SetPrebuiltMap(std::unique_ptr<GridMap> prebuilt);
    static void       SetPrebuiltRoad(std::unique_ptr<Road> prebuilt);

    GridMap* GetMap()
    {
        return map.get();
    }

    Road* GetRoad()
    {
        return road.get();
    }

    const GridMap* GetMap() const
    {
        return map.get();
    }

    const Road* GetRoad() const
    {
        return road.get();
    }

private:
    std::unique_ptr<GridMap> map;
    std::unique_ptr<Road>    road;

    static MapVariant                s_nextVariant;
    static std::unique_ptr<GridMap>  s_prebuiltMap;
    static std::unique_ptr<Road>     s_prebuiltRoad;
};
