/*
Here lies the code responsible for the generation fo the map and the textures.
it uses Perlin Noise to create a natural heightmap.
we then look at the heightmap and mark heights up to a certain point to be water.
and heights above a certain point to be mountiantops covered in snow.
Somewhere in the middle lies fertile land.
*/

#pragma once
#include <raylib.h>
#include <vector>
#include "GridCell.h"
#include "MouseInput.h"
#include "Human.h"
#include "PathFinding.h"


#define GRID_SIZE_Y 35
#define GRID_SIZE_X 35

#define GRID_SIZE_Y_HALF (17)
#define GRID_SIZE_X_HALF (17)

#define TILE_WIDTH 64
#define TILE_HEIGHT 32
#define TILE_WIDTH_HALF 32
#define TILE_HEIGHT_HALF 16


class GridMap
{
public:
	GridMap() : pathFinder(nullptr), showPath(false), currentView(1)
	{

		//resize the grid because the grid might change in later itereations of the code. current there is no need for this
		PlainsTexture = LoadTexture("resources/textures/IsoPlains.png");
		WaterTexture = LoadTexture("resources/textures/IsoWater.png");
		MountainTexture = LoadTexture("resources/textures/IsoMountains.png");
		SnowTexture = LoadTexture("resources/textures/IsoPeak.png");
		SelectTexture = LoadTexture("resources/textures/IsoSelect.png");
		Max_Height = LoadTexture("resources/textures/Max_Height.png");
		IsoGridTexture[0] = WaterTexture;
		IsoGridTexture[1] = PlainsTexture;
		IsoGridTexture[2] = MountainTexture;
		IsoGridTexture[3] = SnowTexture;


		ResetPerlinTexture();

		ResetIsoGridMapArray();

		if (!IsoGridMapArray.empty())
		{
			isoOriginX = ((TILE_WIDTH / 2) * GRID_SIZE_X) - TILE_WIDTH / 2;
			isoOriginY = static_cast<int>(IsoGridMapArray[0].getHeight());
			std::cout << "isoOrigin set: X=" << isoOriginX << ", Y=" << isoOriginY << std::endl;
		}

		IsometricExportOriginalMap();

		InitializeIsoView();

		IsometricExportMap(1);
		IsometricExportMap(2);
		IsometricExportMap(3);
		IsometricExportMap(4);

		InitializePathFinding();

	};

	~GridMap()
	{
		if (pathFinder)
		{
			delete pathFinder;
			pathFinder = nullptr;
		}

		UnloadImage(PerlinNoiseImage);
		UnloadTexture(PerlinNoiseTexture);
	};

	void IsometricExportOriginalMap();
	void IsometricExportMap(int view);

	void ResetIsoGridMapArray();
	void InitializeIsoView();

	void IsometricDrawMap(int index);


	void ResetPerlinTexture();
	const Texture2D* GetPerlinTexture() const;


	void InitializePathFinding();
	void HandlePathFindingInput();
	void FindPath(Vector2 start, Vector2 end);
	void FindPath(int startX, int startY, int endX, int endY);
	void ClearPath();
	void DrawCurrentPathIsometric(Color color, int viewIndex);
	void SetWalkable(int x, int y, bool walkable);
	bool IsWalkable(int x, int y);


	Vector2 ScreenToGridIsometric(Vector2 screenPos);
	Vector2 GridToScreenIsometric(Vector2 gridPos);
	Vector2 WorldToIsometric(float gridX, float gridY, float height) const;

	//For Human ai
	std::vector<Vector2> GetCurrentPath() const { return currentPath; }
	GridCell** GetViewArray(int viewIndex);
	int GetIsoOriginX() const { return isoOriginX; }
	int GetIsoOriginY() const { return isoOriginY; }
	Vector2 TransformPathPointForView(Vector2 point, int viewIndex);

private:
	void rotatePoint90(float x, float y,
		float tx, float ty,
		int angleDeg,
		float& outX, float& outY);

	std::vector<GridCell> IsoGridMapArray;

	GridCell* view1[GRID_SIZE_X * GRID_SIZE_Y];
	GridCell* view2[GRID_SIZE_X * GRID_SIZE_Y];
	GridCell* view3[GRID_SIZE_X * GRID_SIZE_Y];
	GridCell* view4[GRID_SIZE_X * GRID_SIZE_Y];

	Texture IsoGridTexture[4];

	std::vector<GridCell> GridMapArray;
	Image PerlinNoiseImage;
	Texture PerlinNoiseTexture;

	Texture PlainsTexture;
	Texture WaterTexture;
	Texture MountainTexture;
	Texture SnowTexture;
	Texture SelectTexture;
	Texture Max_Height;

	Texture IsoMapTexture[4];
	Texture IsoMap1Texture;
	Texture IsoMap2Texture;
	Texture IsoMap3Texture;
	Texture IsoMap4Texture;

	int mapOriginX = 0;
	int mapOriginY = 0;

	int isoOriginX = 0;
	int isoOriginY = 0;

	int currentView;

	PathFinding* pathFinder;
	std::vector<Vector2> currentPath;
	bool showPath;

};
