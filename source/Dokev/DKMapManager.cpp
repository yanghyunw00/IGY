/**
 * \file DKMapManager.cpp
 * \brief 
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "DKMapManager.hpp"

MapManager::MapManager()
{
    road = std::make_unique<Road>();
}

void MapManager::Initialize()
{
    map.ResetPerlinTexture();
    map.ResetIsoGridMapArray();
    map.InitializePathFinding();
}
