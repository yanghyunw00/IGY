/**
 * \file GridMap.hpp
 * \brief 
 * \author Junseok Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include <vector>
#include "GridCell.hpp"
#include "Dokev/DKTexture.hpp"
#include "Dokev/DKImage.hpp"
#include "Dokev/DKShapes.hpp"
//#include "MouseInput.h"
//#include "Human.h"
//#include "PathFinding.h"


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
	GridMap() :  currentView(1) // pathFinder(nullptr), showPath(false),
	{

		//resize the grid because the grid might change in later itereations of the code. current there is no need for this
		PlainsTexture = Dokev::LoadTexture("Assets/images/textures/IsoPlains.png");
		WaterTexture = Dokev::LoadTexture("Assets/images/textures/IsoWater.png");
		MountainTexture = Dokev::LoadTexture("Assets/images/textures/IsoMountains.png");
		SnowTexture = Dokev::LoadTexture("Assets/images/textures/IsoPeak.png");
		Max_Height = Dokev::LoadTexture("Assets/images/textures/Max_Height.png");  // Use IsoPeak as placeholder for Max_Height
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

		//InitializePathFinding();

	};

	~GridMap()
	{
		// if (pathFinder)
		// {
		// 	delete pathFinder;
		// 	pathFinder = nullptr;
		// }

		UnloadImage(PerlinNoiseImage);
	};

	void IsometricExportOriginalMap();
	void IsometricExportMap(int view);

	void ResetIsoGridMapArray();
	void InitializeIsoView();

	void IsometricDrawMap(int index) const;


	void ResetPerlinTexture();
	const Dokev::Texture2D* GetPerlinTexture() const;
	GridCell** GetViewArray(int viewIndex);


	// void InitializePathFinding();
	// void HandlePathFindingInput();
	// void FindPath(Vector2 start, Vector2 end);
	// void FindPath(int startX, int startY, int endX, int endY);
	// void ClearPath();
	// void DrawCurrentPathIsometric(Color color, int viewIndex);
	// void SetWalkable(int x, int y, bool walkable);
	// bool IsWalkable(int x, int y);


	// Vector2 ScreenToGridIsometric(Vector2 screenPos);
	// Vector2 GridToScreenIsometric(Vector2 gridPos);
	// Vector2 WorldToIsometric(float gridX, float gridY, float height) const;

	// //For Human ai
	// std::vector<Vector2> GetCurrentPath() const { return currentPath; }
	// int GetIsoOriginX() const { return isoOriginX; }
	// int GetIsoOriginY() const { return isoOriginY; }
	// Vector2 TransformPathPointForView(Vector2 point, int viewIndex);

private:
	void rotatePoint90(int x, int y,
		float tx, float ty,
		int angleDeg,
		float& outX, float& outY);

	std::vector<GridCell> IsoGridMapArray;

	std::vector<std::vector<GridCell>> m_gridCells;


	GridCell* view1[GRID_SIZE_X * GRID_SIZE_Y];
	GridCell* view2[GRID_SIZE_X * GRID_SIZE_Y];
	GridCell* view3[GRID_SIZE_X * GRID_SIZE_Y];
	GridCell* view4[GRID_SIZE_X * GRID_SIZE_Y];

	Dokev::Texture2D IsoGridTexture[4];

	std::vector<GridCell> GridMapArray;
	Dokev::Image PerlinNoiseImage;
	Dokev::Texture2D PerlinNoiseTexture;

	Dokev::Texture2D PlainsTexture;
	Dokev::Texture2D WaterTexture;
	Dokev::Texture2D MountainTexture;
	Dokev::Texture2D SnowTexture;
	Dokev::Texture2D Max_Height;

	Dokev::Texture2D IsoMapTexture[4];
	Dokev::Texture2D IsoMap1Texture;
	Dokev::Texture2D IsoMap2Texture;
	Dokev::Texture2D IsoMap3Texture;
	Dokev::Texture2D IsoMap4Texture;

	int mapOriginX = 0;
	int mapOriginY = 0;

	int isoOriginX = 0;
	int isoOriginY = 0;

	mutable int currentView;

	// PathFinding* pathFinder;
	// std::vector<Vector2> currentPath;
	// bool showPath;

};
