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

void Road::drawRoads(const GridMap& map, int currentview)
{
	if (roadPositions.empty())
		return;

	GridCell** viewArray = map.GetViewArray(currentview);
	int originX = ((TILE_WIDTH / 2) * GRID_SIZE_X) - TILE_WIDTH / 2;
	int originY = 0;

	// Draw roads in isometric order (back to front) just like the map
	for (int sum = 0; sum < GRID_SIZE_X + GRID_SIZE_Y - 1; ++sum)
	{
		for (int x = 0; x <= sum; ++x)
		{
			int y = sum - x;

			if (x >= 0 && x < GRID_SIZE_X && y >= 0 && y < GRID_SIZE_Y)
			{
				// Check if there's a road at this position
				bool hasRoadHere = false;
				for (const auto& roadPos : roadPositions)
				{
					if (static_cast<int>(roadPos.x) == x && static_cast<int>(roadPos.y) == y)
					{
						hasRoadHere = true;
						break;
					}
				}

				if (hasRoadHere)
				{
					// Draw the road
					float height = static_cast<float>(viewArray[y * GRID_SIZE_X + x]->getHeight());
					int screenX = originX + (x - y) * TILE_WIDTH_HALF;
					int screenY = originY + (x + y) * TILE_HEIGHT_HALF;
					int topY = static_cast<int>(height) - screenY;

					roadTexture.Draw(screenX, topY);
				}
			}
		}
	}
}

void Road::buildRoadPath(GridMap& map, int startX, int startY, int endX, int endY)
{
	// Use pathfinding
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
