/*!
@file	Road.cpp
@author	Dongyun Lee
@date	2025/10/11
@brief	Build a road
*//*______________________________________________________________________*/

#include <vector>
#include "Road.h"
#include "GridMap.h"

void Road::addRoad(int x, int y)
{
	roadPositions.push_back({ static_cast<float>(x), static_cast<float>(y)});
}

void Road::removeRoad(int x, int y)
{
	for (auto it = roadPositions.begin(); it != roadPositions.end(); ++it)
	{
		if (it->x == x && it->y == y)
		{
			roadPositions.erase(it);
			break;
		}
	}
}

bool Road::hasRoad(int x, int y)
{
	for (auto it = roadPositions.begin(); it != roadPositions.end(); ++it)
	{
		if (it->x == x && it->y == y)
		{
			return true;
		}
	}

	return false;
}

void Road::drawRoads(GridMap& map, int currentview)
{
	if (!roadPositions.empty())
	{
		for (auto& roadPos : roadPositions)
		{
			Vector2 transformedPos = map.TransformPathPointForView(roadPos, currentview);

			int gridX = static_cast<int>(transformedPos.x);
			int gridY = static_cast<int>(transformedPos.y);

			// Get height from the grid cell
			GridCell** viewArray = map.GetViewArray(currentview);
			float height = viewArray[gridY * GRID_SIZE_X + gridX]->getHeight();

			// Get origin
			int originX = map.GetIsoOriginX();
			int originY = map.GetIsoOriginY();

			// Manual isometric calculation
			int isoX = originX + (transformedPos.x - transformedPos.y) * TILE_WIDTH_HALF;
			int isoY = originY + (transformedPos.x + transformedPos.y) * TILE_HEIGHT_HALF - static_cast<int>(height);

			DrawTexture(roadTexture, isoX, isoY, WHITE);
		}
	}
}

void Road::buildRoadPath(GridMap& map, int startX, int startY, int endX, int endY)
{
	// Use pathfinding
	map.FindPath(startX, startY, endX, endY);

	std::vector<Vector2> path = map.GetCurrentPath();

	for (const auto& pos : path)
	{
		addRoad(static_cast<int>(pos.x), static_cast<int>(pos.y));
	}

	map.ClearPath();
}

void Road::removeAllRoad()
{
	roadPositions.clear();
}
