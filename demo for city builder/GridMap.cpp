#include "GridMap.h"

#include <raylib.h>
#include <raymath.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

void GridMap::rotatePoint90(float x,
	float y,
	float tx,
	float ty,
	int angleDeg,
	float& oX,
	float& oY)
{
	float dx = x - tx;
	float dy = y - ty;

	int rot = ((angleDeg % 360) + 360) % 360;

	switch(rot) {
	case 0:
		oX = tx + dx;
		oY = ty + dy;
		break;
	case 90:
		oX = tx - dy;
		oY = ty + dx;
		break;
	case 180:
		oX = tx - dx;
		oY = ty - dy;
		break;
	case 270:
		oX = tx + dy;
		oY = ty - dx;
		break;
	case 360:
		oX = tx + dx;
		oY = ty + dy;
		break;
	}
}

void GridMap::IsometricExportOriginalMap()
{
	int offsetupper = static_cast<int>(IsoGridMapArray[0].getHeight());
	RenderTexture2D target = LoadRenderTexture(
		TILE_WIDTH * GRID_SIZE_X, TILE_HEIGHT * GRID_SIZE_Y + offsetupper + 225);

	BeginTextureMode(target);

	int originX = ((TILE_WIDTH / 2) * GRID_SIZE_X) - TILE_WIDTH / 2;
	int originY = offsetupper;

	for(int sum = 0; sum < GRID_SIZE_X + GRID_SIZE_Y - 1; ++sum) {
		for(int x = 0; x <= sum; ++x) {
			int y = sum - x;

			if(x >= 0 && x < GRID_SIZE_X && y >= 0 && y < GRID_SIZE_Y) {
				int screenX = originX + (x - y) * TILE_WIDTH_HALF;
				int screenY = originY + (x + y) * TILE_HEIGHT_HALF;

				int count = y * GRID_SIZE_X + x;
				double height = IsoGridMapArray[count].getHeight();

				int current_texture = 3;
				if(height < 205)
					current_texture = 2;
				if(height < 150)
					current_texture = 1;
				if(height < 75)
					current_texture = 0;

				DrawTexture(
					IsoGridTexture[current_texture], screenX, screenY - height, WHITE);
				DrawTexture(Max_Height, screenX, screenY - height, WHITE);
			}
		}
	}

	EndTextureMode();
	Image image = LoadImageFromTexture(target.texture);
	ImageFlipVertical(&image);

	ExportImage(image, "original.png");
}

void GridMap::IsometricExportMap(int view)
{
	GridCell** selectedView = nullptr;
	Texture2D* outputTexture = nullptr;
	const char* filename = nullptr;

	switch(view) {
	case 1:
		selectedView = view1;
		outputTexture = &IsoMap1Texture;
		break;
	case 2:
		selectedView = view2;
		outputTexture = &IsoMap2Texture;
		break;
	case 3:
		selectedView = view3;
		outputTexture = &IsoMap3Texture;
		break;
	case 4:
		selectedView = view4;
		outputTexture = &IsoMap4Texture;
		break;
	default:
		return;
	}

	int offsetupper = static_cast<int>(selectedView[0]->getHeight());
	RenderTexture2D target = LoadRenderTexture(
		TILE_WIDTH * GRID_SIZE_X, TILE_HEIGHT * GRID_SIZE_Y + offsetupper + 225);

	BeginTextureMode(target);

	int originX = ((TILE_WIDTH / 2) * GRID_SIZE_X) - TILE_WIDTH / 2;
	int originY = offsetupper;

	for(int sum = 0; sum < GRID_SIZE_X + GRID_SIZE_Y - 1; ++sum) {
		for(int x = 0; x <= sum; ++x) {
			int y = sum - x;

			if(x >= 0 && x < GRID_SIZE_X && y >= 0 && y < GRID_SIZE_Y) {
				int screenX = originX + (x - y) * TILE_WIDTH_HALF;
				int screenY = originY + (x + y) * TILE_HEIGHT_HALF;

				int count = y * GRID_SIZE_X + x;
				double height = selectedView[count]->getHeight();

				int current_texture = 3;
				if(height < 205)
					current_texture = 2;
				if(height < 150)
					current_texture = 1;
				if(height < 75)
					current_texture = 0;

				DrawTexture(
					IsoGridTexture[current_texture], screenX, screenY - height, WHITE);
				DrawTexture(Max_Height, screenX, screenY - height, WHITE);
			}
		}
	}

	EndTextureMode();
	Image image = LoadImageFromTexture(target.texture);
	ImageFlipVertical(&image);
	if(outputTexture->id != 0) {
		UnloadTexture(*outputTexture);
	}
	*outputTexture = LoadTextureFromImage(image);
}

void GridMap::ResetIsoGridMapArray()
{
	IsoGridMapArray.resize(GRID_SIZE_X * GRID_SIZE_Y);

	int x = 0;
	int y = 0;
	int row = 1;
	int last_row = 0;
	int count = 0;

	for(int sum = 0; sum < GRID_SIZE_X + GRID_SIZE_Y - 1; ++sum) {
		for(int x = 0; x <= sum; ++x) {
			int y = sum - x;

			if(x < GRID_SIZE_X && y < GRID_SIZE_Y) {
				int count = y * GRID_SIZE_X + x;

				unsigned char Height = GetImageColor(PerlinNoiseImage, x, y).r;

				IsoGridMapArray[count] = GridCell { (double)Height, x, y };
			}
		}
	}
}

void GridMap::InitializeIsoView()
{
	float new_x = 0;
	float new_y = 0;

	for(int sum = 0; sum < GRID_SIZE_X + GRID_SIZE_Y - 1; ++sum) {
		for(int x = 0; x <= sum; ++x) {
			int y = sum - x;

			if(x >= 0 && x < GRID_SIZE_X && y >= 0 && y < GRID_SIZE_Y) {
				int baseIndex = y * GRID_SIZE_X + x;

				view1[baseIndex] = &IsoGridMapArray[baseIndex];

				rotatePoint90(
					x, y, GRID_SIZE_X_HALF, GRID_SIZE_Y_HALF, 90, new_x, new_y);
				int rotIndex90 = static_cast<int>(new_y) * GRID_SIZE_X + static_cast<int>(new_x);
				if(rotIndex90 >= 0 && rotIndex90 < GRID_SIZE_X * GRID_SIZE_Y)
					view2[rotIndex90] = &IsoGridMapArray[baseIndex];

				rotatePoint90(
					x, y, GRID_SIZE_X_HALF, GRID_SIZE_Y_HALF, 180, new_x, new_y);
				int rotIndex180 = static_cast<int>(new_y) * GRID_SIZE_X + static_cast<int>(new_x);
				if(rotIndex180 >= 0 && rotIndex180 < GRID_SIZE_X * GRID_SIZE_Y)
					view3[rotIndex180] = &IsoGridMapArray[baseIndex];

				rotatePoint90(
					x, y, GRID_SIZE_X_HALF, GRID_SIZE_Y_HALF, 270, new_x, new_y);
				int rotIndex270 = static_cast<int>(new_y) * GRID_SIZE_X + static_cast<int>(new_x);
				if(rotIndex270 >= 0 && rotIndex270 < GRID_SIZE_X * GRID_SIZE_Y)
					view4[rotIndex270] = &IsoGridMapArray[baseIndex];
			}
		}
	}
}

void GridMap::IsometricDrawMap(int index)
{
	currentView = (index % 4) + 1;

	switch(index % 4) {
	case 0:
		if(IsoMap1Texture.id != 0) {
			DrawTexture(IsoMap1Texture, 0, 0, WHITE);
		}
		break;
	case 1:
		if(IsoMap2Texture.id != 0) {
			DrawTexture(IsoMap2Texture, 0, 0, WHITE);
		}
		break;
	case 2:
		if(IsoMap3Texture.id != 0) {
			DrawTexture(IsoMap3Texture, 0, 0, WHITE);
		}
		break;
	case 3:
		if(IsoMap4Texture.id != 0) {
			DrawTexture(IsoMap4Texture, 0, 0, WHITE);
		}
		break;
	}

	DrawCurrentPathIsometric(RED, currentView);
}

void GridMap::ResetPerlinTexture()
{
	int seedX = GetRandomValue(0, 10000);
	int seedY = GetRandomValue(0, 10000);
	PerlinNoiseImage = GenImagePerlinNoise(GRID_SIZE_X, GRID_SIZE_Y, seedX, seedY, 1.0f);
	PerlinNoiseTexture = LoadTextureFromImage(PerlinNoiseImage);

	ExportImage(PerlinNoiseImage, "PerlinNoise.png");
}

const Texture2D*
GridMap::GetPerlinTexture() const
{
	return &PerlinNoiseTexture;
}

//------------------------------------Path finding-------------------------------------//

void GridMap::InitializePathFinding()
{
	if(pathFinder) {
		delete pathFinder;
		pathFinder = nullptr;
	}

	pathFinder = new PathFinding(GRID_SIZE_X, GRID_SIZE_Y, 50.0f);

	std::vector<double> heights;
	std::vector<float> costs;

	heights.reserve(IsoGridMapArray.size());
	costs.reserve(IsoGridMapArray.size());

	for(const auto& cell : IsoGridMapArray) {
		heights.push_back(cell.getHeight());
		costs.push_back(cell.getMovementCost());
	}

	pathFinder->SetHeightMap(heights);
	pathFinder->SetCostMap(costs);

	int walkableCount = 0;
	for(int y = 0; y < GRID_SIZE_Y; y++) {
		for(int x = 0; x < GRID_SIZE_X; x++) {
			const auto& currentCell = IsoGridMapArray[y * GRID_SIZE_X + x];
			bool walkable = currentCell.isWalkable();
			pathFinder->SetWalkable(x, y, walkable);
			if(walkable)
				walkableCount++;
		}
	}
}

void GridMap::HandlePathFindingInput()
{
	ClearPath();
}

void GridMap::FindPath(Vector2 start, Vector2 end)
{
	if(!pathFinder) {
		std::cout << "No pathfinder" << std::endl;
		return;
	}
	currentPath = pathFinder->FindPath(start, end);
	showPath = true;

}

void GridMap::FindPath(int startX, int startY, int endX, int endY)
{
	FindPath({ static_cast<float>(startX), static_cast<float>(startY) },
		{ static_cast<float>(endX), static_cast<float>(endY) });
}

void GridMap::ClearPath()
{
	currentPath.clear();
	showPath = false;
	std::cout << "Path cleared" << std::endl;
}

GridCell** GridMap::GetViewArray(int viewIndex)
{
	switch(viewIndex)
	{
	case 1:
		return view1;
	case 2:
		return view2;
	case 3:
		return view3;
	case 4:
		return view4;
	default:
		return view1;
	}
}

void GridMap::DrawCurrentPathIsometric(Color color, int viewIndex)
{
	if(!pathFinder || currentPath.empty() || !showPath) {
		return;
	}

	GridCell** selectedView = GetViewArray(viewIndex);

	int originX = ((TILE_WIDTH / 2) * GRID_SIZE_X) - TILE_WIDTH / 2;
	int originY = static_cast<int>(selectedView[0]->getHeight());

	std::vector<Vector2> transformedPath;
	transformedPath.reserve(currentPath.size());

	for(const auto& point : currentPath)
	{
		Vector2 transformedPoint = TransformPathPointForView(point, viewIndex);
		transformedPath.push_back(transformedPoint);
	}

	const int offset = 0;

	for(size_t i = 0; i < transformedPath.size(); i++)
	{
		int x = static_cast<int>(transformedPath[i].x);
		int y = static_cast<int>(transformedPath[i].y);

		if(x < 0 || x >= GRID_SIZE_X || y < 0 || y >= GRID_SIZE_Y) continue;

		float currentHeight = selectedView[y * GRID_SIZE_X + x]->getHeight();
		Vector2 worPos = WorldToIsometric(x,y, currentHeight);
		DrawCircle(worPos.x, worPos.y, 5, color);

		if(i < transformedPath.size() - 1)
		{
			int nextX = static_cast<int>(transformedPath[i + 1].x);
			int nextY = static_cast<int>(transformedPath[i + 1].y);

			if(nextX < 0 || nextX >= GRID_SIZE_X || nextY < 0 || nextY >= GRID_SIZE_Y)
				continue;

			float nextHeight = selectedView[nextY * GRID_SIZE_X + nextX]->getHeight();

			Vector2 nextPos = WorldToIsometric(x, y, nextHeight);

			DrawLineEx({ (float)worPos.x, (float)worPos.y },{ (float)nextPos.x, (float)nextPos.y }, 2.0f, color);
		}
	}
}

void GridMap::SetWalkable(int x, int y, bool walkable)
{
	if(pathFinder) {
		pathFinder->SetWalkable(x, y, walkable);
	}
}

bool GridMap::IsWalkable(int x, int y)
{
	if(pathFinder) {
		return pathFinder->IsPositionWalkable(x, y);
	}
	return false;
}

Vector2 GridMap::ScreenToGridIsometric(Vector2 screenPos)
{
	float localX = screenPos.x - (float)isoOriginX;
	float localY = screenPos.y - (float)isoOriginY;

	float approxX = (localX / (float)TILE_WIDTH_HALF + localY / (float)TILE_HEIGHT_HALF) * 0.5f;
	float approxY = (localY / (float)TILE_HEIGHT_HALF - localX / (float)TILE_WIDTH_HALF) * 0.5f;

	int cx = static_cast<int>(std::floor(approxX));
	int cy = static_cast<int>(std::floor(approxY));

	for(int dy = -1; dy <= 1; ++dy) {
		for(int dx = -1; dx <= 1; ++dx) {
			int tx = cx + dx;
			int ty = cy + dy;

			if(tx < 0 || tx >= GRID_SIZE_X || ty < 0 || ty >= GRID_SIZE_Y)
				continue;

			double tileHeight = IsoGridMapArray[ty * GRID_SIZE_X + tx].getHeight();
			int tileScreenX = isoOriginX + (tx - ty) * TILE_WIDTH_HALF;
			int tileScreenY = isoOriginY + (tx + ty) * TILE_HEIGHT_HALF - static_cast<int>(tileHeight);

			float relX = screenPos.x - (float)tileScreenX;
			float relY = screenPos.y - (float)tileScreenY;

			float nx = fabs(relX) / (float)TILE_WIDTH_HALF;
			float ny = fabs(relY) / (float)TILE_HEIGHT_HALF;

			if(nx + ny <= 1.0f) {
				return { static_cast<float>(tx), static_cast<float>(ty) };
			}
		}
	}

	if(cx >= 0 && cx < GRID_SIZE_X && cy >= 0 && cy < GRID_SIZE_Y)
		return { static_cast<float>(cx), static_cast<float>(cy) };

	return { -1.0f, -1.0f };
}

Vector2 GridMap::GridToScreenIsometric(Vector2 gridPos)
{
	float isoX = (gridPos.x - gridPos.y) * TILE_WIDTH_HALF;
	float isoY = (gridPos.x + gridPos.y) * TILE_HEIGHT_HALF;
	return { isoX, isoY };
}

Vector2 GridMap::WorldToIsometric(float gridX, float gridY, float height) const
{
	int originX = GetIsoOriginX();
	int originY = GetIsoOriginY();

	int isoX = originX + (gridX - gridY) * TILE_WIDTH_HALF;
	int isoY = originY + (gridX + gridY) * TILE_HEIGHT_HALF - static_cast<int>(height);

	float centerX = static_cast<int>(isoX + TILE_WIDTH_HALF);
	float centerY = static_cast<int>(isoY + TILE_HEIGHT_HALF);

	return { centerX, centerY };
}

Vector2 GridMap::TransformPathPointForView(Vector2 point, int viewIndex)
{
	float x = point.x;
	float y = point.y;
	float newX, newY;

	switch(viewIndex) {
	case 1:
		return point;
	case 2:
		rotatePoint90(x, y, GRID_SIZE_X_HALF, GRID_SIZE_Y_HALF, 90, newX, newY);
		return { newX, newY };

	case 3:
		rotatePoint90(x, y, GRID_SIZE_X_HALF, GRID_SIZE_Y_HALF, 180, newX, newY);
		return { newX, newY };

	case 4:
		rotatePoint90(x, y, GRID_SIZE_X_HALF, GRID_SIZE_Y_HALF, 270, newX, newY);
		return { newX, newY };

	default:
		return point;
	}
}
