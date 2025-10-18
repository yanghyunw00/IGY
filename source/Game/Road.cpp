/**
 * \file Road.cpp
 * \author Dongyun Lee
 * \author
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include <vector>
#include "Road.hpp"
#include "GridMap.hpp"
#include "Engine/Vec2.hpp"
#include "Dokev/DKColor.hpp"

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

void Road::drawRoads([[maybe_unused]]  GridMap& map,[[maybe_unused]]  int currentview)
{
	if (!roadPositions.empty())
	{
		for ([[maybe_unused]]  auto& roadPos : roadPositions)
		{
			Math::fvec2 transformedPos = map.TransformPathPointForView(roadPos, currentview);

			int gridX = static_cast<int>(transformedPos.x);
			int gridY = static_cast<int>(transformedPos.y);

			//Get height from the grid cell
			GridCell** viewArray = map.GetViewArray(currentview);
			float height = static_cast<float>(viewArray[gridY * GRID_SIZE_X + gridX]->getHeight());

			//Get origin
			int originX = map.GetIsoOriginX();
			int originY = map.GetIsoOriginY();

			//Manual isometric calculation
			int isoX = static_cast<int>(originX + (transformedPos.x - transformedPos.y) * TILE_WIDTH_HALF);
			int isoY = static_cast<int>(originY + (transformedPos.x + transformedPos.y) * TILE_HEIGHT_HALF - static_cast<int>(height));

			DrawTexture(roadTexture, isoX, isoY, Dokev::ColorPalette::WHITE);
		}
	}
}

void Road::buildRoadPath([[maybe_unused]]  GridMap& map, [[maybe_unused]] int startX,[[maybe_unused]]  int startY, [[maybe_unused]] int endX,[[maybe_unused]]  int endY)
{
	//Use pathfinding
	map.FindPath(startX, startY, endX, endY);

	std::vector<Math::fvec2> path = map.GetCurrentPath();

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
