/**
 * \file Road.hpp
 * \author Dongyun Lee
 * \author
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include <vector>

#include "GridMap.hpp"
#include "Dokev/DKTexture.hpp"
#include "Engine/Vec2.hpp"

#pragma once

using namespace Dokev;

class GridMap;

class Road
{

private:
	Texture2D roadTexture;
	std::vector<Math::fvec2> roadPositions;

public:
	Road()
	{
		roadTexture = LoadTexture("resources/textures/road.png");
	};

	~Road()
	{
	};

	void addRoad(int x, int y);
	void removeRoad(int x, int y);
	void removeAllRoad();
	bool hasRoad(int x, int y);
	void drawRoads(GridMap& map, int currentview);
	void buildRoadPath(GridMap& map, int startX, int startY, int endX, int endY);
};
