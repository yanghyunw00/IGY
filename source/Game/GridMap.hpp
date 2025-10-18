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
#include "Game/pathFinding.hpp"
//#include "MouseInput.h"
//#include "Human.h"
//#include "PathFinding.h"


#define GRID_SIZE_Y 31
#define GRID_SIZE_X 31

#define GRID_SIZE_Y_HALF (15)
#define GRID_SIZE_X_HALF (15)

#define TILE_WIDTH 64
#define TILE_HEIGHT 32
#define TILE_WIDTH_HALF 32
#define TILE_HEIGHT_HALF 16


class GridMap
{
public:
	
	GridMap();

	~GridMap()
	{
		// if (pathFinder)
		// {
		// 	delete pathFinder;
		// 	pathFinder = nullptr;
		// }

		UnloadImage(PerlinNoiseImage);
	};
	
	void ReGenerateMap();
	void ResetIsoGridMapArray();
	void InitializeIsoView();
	void IsometricDrawMap(int index) const;
	void ResetPerlinTexture();
	const Dokev::Texture2D* GetPerlinTexture() const;
	GridCell** GetViewArray(int viewIndex);


	//Path Finding
	void InitializePathFinding();
	void HandlePathFindingInput();
	void FindPath(Math::fvec2 start, Math::fvec2 end);
	void FindPath(int startX, int startY, int endX, int endY);
	void ClearPath();
	void DrawCurrentPathIsometric(CS200::RGBA color, int viewIndex);
	void SetWalkable(int x, int y, bool walkable);
	bool IsWalkable(int x, int y);

	// //For Human ai
	std::vector<Math::fvec2> GetCurrentPath() const { return currentPath; }
	int GetIsoOriginX() const { return isoOriginX; }
	int GetIsoOriginY() const { return isoOriginY; }
	Math::fvec2 TransformPathPointForView(Math::fvec2 point, int viewIndex);

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

	PathFinding* pathFinder;
	std::vector<Math::fvec2> currentPath;
	bool showPath;

};
