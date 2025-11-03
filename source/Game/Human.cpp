/**
 * \file Human.cpp
 * \author Hyunwoo Yang
 * \author
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "Human.hpp"
#include "GridMap.hpp" 
#include "Dokev/DKColor.hpp"
#include <iostream>

Human::Human(Math::fvec2 startGridPos)
	: currentGridPos(startGridPos)
	, targetGridPos(startGridPos)
	, currentPathIndex(0)
	, isMoving(false)
	, color(Dokev::ColorPalette::BLUE)
{
	startPosition = currentGridPos;
	actualPosition = currentGridPos;
	moveTimer = 0.0f;
}

void Human::FollowPath(const std::vector<Math::fvec2 >& newPath)
{
	if(newPath.size() <= 1)
	{
		isMoving = false;
		path.clear();
		return;
	}

	path = newPath;
	currentPathIndex = 0;


	currentGridPos = path[0];
	actualPosition = path[0];
	startPosition = path[0];

	currentPathIndex = 1;
	targetGridPos = path[currentPathIndex];
	isMoving = true;
	moveTimer = 0.0f;


}

void Human::Update(float deltaTime, GridMap* map)
{
	if(!isMoving) return;

	moveTimer += deltaTime;
	float t = moveTimer / moveDelay;

	if(t > 1.0f)
	{
		t = 1.0f;
	}

	actualPosition.x = startPosition.x + (targetGridPos.x - startPosition.x) * t;
	actualPosition.y = startPosition.y + (targetGridPos.y - startPosition.y) * t;

	if(t >= 1.0f) // 목표 지점 도착
	{
		// 위치를 목표 지점에 정확히 맞춤ccccccccccccc
		actualPosition = targetGridPos;
		currentGridPos = targetGridPos;

		///debugging mode
		if(debugMode && map)
		{
			int gridX = (int)currentGridPos.x;
			int gridY = (int)currentGridPos.y;

			if(gridX >= 0 && gridX < GRID_SIZE_X && gridY >= 0 && gridY < GRID_SIZE_Y)
			{
				GridCell** viewArray = map->GetViewArray(1);
				GridCell* currentCell = viewArray[gridY * GRID_SIZE_X + gridX];

				std::string terrainName;
				double height = currentCell->getHeight();

				if(height < 75.0)      terrainName = "Water";
				else if(height < 150.0) terrainName = "Plains";
				else if(height < 205.0) terrainName = "Mountain";
				else                    terrainName = "Peak";

				std::cout << "Arrived Tile: (" << gridX << ", " << gridY << ") - "
					<< terrainName << " [Height: " << (int)height << "]" << std::endl;
			}
		}

		currentPathIndex++;

		// 끝에 도달했는지 
		if(currentPathIndex >= path.size())
		{
			isMoving = false;
			path.clear();
			if(debugMode)
			{
				std::cout << "=== Destination ===" << std::endl;
			}
		}
		else // 아직 덜 갔으면
		{
			startPosition = currentGridPos;
			targetGridPos = path[currentPathIndex];
			moveTimer = 0.0f;

			if(debugMode)
			{
				std::cout << "  -> Next Tile: (" << (int)targetGridPos.x << ", " << (int)targetGridPos.y << ")" << std::endl;
			}
		}
	}
	else
	{
		actualPosition.x = startPosition.x + (targetGridPos.x - startPosition.x) * t;
		actualPosition.y = startPosition.y + (targetGridPos.y - startPosition.y) * t;
	}
}

void Human::Draw(int viewIndex, const GridMap* map) const
{
	float gridX_float = actualPosition.x;
	float gridY_float = actualPosition.y;

	int gridX_int = static_cast<int>(gridX_float);
	int gridY_int = static_cast<int>(gridY_float);

	if(gridX_int < 0 || gridX_int >= GRID_SIZE_X || gridY_int < 0 || gridY_int >= GRID_SIZE_Y)
		return;

	// Get the view array
	GridCell** viewArray = map->GetViewArray(viewIndex);

	// Get height from the non-rotated grid position
	float height = static_cast<float>(viewArray[gridY_int * GRID_SIZE_X + gridX_int]->getHeight());

	// Match the map's drawing calculation
	int originX = ((TILE_WIDTH / 2) * GRID_SIZE_X) - TILE_WIDTH / 2;
	int originY = 0;

	int screenX = originX + static_cast<int>((gridX_float - gridY_float) * TILE_WIDTH_HALF);
	int screenY = originY + static_cast<int>((gridX_float + gridY_float) * TILE_HEIGHT_HALF);
	int topY = static_cast<int>(height) - screenY;

	int drawX = screenX + TILE_WIDTH_HALF;
	int drawY = topY;

	Dokev::DrawCircle(drawX, drawY, 20, Dokev::ColorPalette::RED);
	Dokev::DrawCircleLines(drawX, drawY, 20, Dokev::ColorPalette::YELLOW);
}

