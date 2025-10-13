/*!
@file	Road.h
@author	Dongyun Lee
@date	2025/10/11
@brief	Build a road
*//*______________________________________________________________________*/

#include <raylib.h>
#include <vector>

#include "GridMap.h"

#pragma once
class GridMap;

class Road
{

private:
	Texture2D roadTexture;
	std::vector<Vector2> roadPositions;

public:
	Road()
	{
		roadTexture = LoadTexture("resources/textures/road.png");
	};

	~Road()
	{
		UnloadTexture(roadTexture);
	};

	void addRoad(int x, int y);
	void removeRoad(int x, int y);
	void removeAllRoad();
	bool hasRoad(int x, int y);
	void drawRoads(GridMap& map, int currentview);
	void buildRoadPath(GridMap& map, int startX, int startY, int endX, int endY);
};
