/**
 * \file GridMap.hpp
 * \brief 
 * \author Junseok Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include <vector>
#include <memory>
#include "GridCell.hpp"
#include "Dokev/DKTexture.hpp"
#include "Dokev/DKImage.hpp"
#include "Dokev/DKShapes.hpp"
#include "Dokev/DKTileManager.hpp"
#include "Dokev/DKRenderState.hpp"
#include "OpenGL/GL.hpp"
#include "Game/pathFinding.hpp"

// Forward declaration
namespace Dokev { class MouseSelection; }

#define GRID_SIZE_Y 67
#define GRID_SIZE_X 67

#define GRID_SIZE_Y_HALF (33)
#define GRID_SIZE_X_HALF (33)

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
		UnloadImage(PerlinNoiseImage);
	};
	
	void ReGenerateMap();
	void ResetIsoGridMapArray();
	void InitializeIsoView();
	void IsometricDrawMap(int index,Dokev::RenderState selected_renderer = Dokev::RenderState::INSTANCED, const Dokev::MouseSelection* mouseSelection = nullptr) const;

	void IsometricDrawMouseAtlas(int index,Dokev::RenderState selected_renderer = Dokev::RenderState::INSTANCED) const;

	void ResetPerlinTexture();
	const Dokev::Texture2D* GetPerlinTexture() const;
	GridCell** GetViewArray(int viewIndex) const;


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
	Math::fvec2 TransformPathPointForView(Math::fvec2 point, int viewIndex) const;

	// TileManager access
	Dokev::TileManager* GetTileManager() { return tileManager.get(); }
	const Dokev::TileManager* GetTileManager() const { return tileManager.get(); }


private:
	void rotatePoint90(float x, float y,
		float tx, float ty,
		int angleDeg,
		float& outX, float& outY) const;

	std::vector<GridCell> IsoGridMapArray;

	std::vector<std::vector<GridCell>> m_gridCells;


	mutable GridCell* view1[GRID_SIZE_X * GRID_SIZE_Y];
	mutable GridCell* view2[GRID_SIZE_X * GRID_SIZE_Y];
	mutable GridCell* view3[GRID_SIZE_X * GRID_SIZE_Y];
	mutable GridCell* view4[GRID_SIZE_X * GRID_SIZE_Y];

	Dokev::Texture2D IsoGridTexture[4];
	
	std::vector<GridCell> GridMapArray;
	Dokev::Image PerlinNoiseImage;
	Dokev::Texture2D PerlinNoiseTexture;

	Dokev::Texture2D PlainsTexture;
	Dokev::Texture2D WaterTexture;
	Dokev::Texture2D MountainTexture;
	Dokev::Texture2D SnowTexture;
	Dokev::Texture2D Max_Height;
	
	Dokev::Texture2D IsoMouseAtlas;




	int mapOriginX = 0;
	int mapOriginY = 0;

	int isoOriginX = 0;
	int isoOriginY = 0;

	mutable int currentView;

	std::unique_ptr<PathFinding> pathFinder;
	std::vector<Math::fvec2> currentPath;
	bool showPath;

	// Tile management for buildings and roads
	std::unique_ptr<Dokev::TileManager> tileManager;
	Dokev::Texture2D RoadTexture;

public: 


	GLuint mouseAtlasFBO   = 0;
    GLuint mouseAtlasTex   = 0;





};
