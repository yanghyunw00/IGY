/**
 * \file GridMap.hpp
 * \brief
 * \author Junseok Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include "../World/pathFinding.hpp"
#include "Dokev/Graphics/DKAnimationSprite.hpp"
#include "Dokev/Graphics/DKImage.hpp"
#include "Dokev/Graphics/DKRenderState.hpp"
#include "Dokev/Graphics/DKShapes.hpp"
#include "Dokev/Graphics/DKTexture.hpp"
#include "Dokev/Managers/DKTileManager.hpp"
#include "Dokev/Managers/DKGameObjectManager.hpp"

#include "GridCell.hpp"
#include "OpenGL/GL.hpp"
#include <memory>
#include <vector>


enum class MapVariant
{
    Balanced = 0,
    WaterHeavy,
    PlainsHeavy,
    MountainHeavy
};

struct MapGenerationProfile
{
    MapVariant        variant    = MapVariant::Balanced;
    TerrainThresholds thresholds = DefaultTerrainThresholds();
    float             noiseScale = 0.1f;
    const char*       label      = "Balanced";
};

MapGenerationProfile GetMapProfile(MapVariant variant);
const char*          GetMapVariantLabel(MapVariant variant);

// Forward declaration
namespace Dokev
{
    class MouseSelection;
}

#define GRID_SIZE_Y 67
#define GRID_SIZE_X 67

#define GRID_SIZE_Y_HALF (33)
#define GRID_SIZE_X_HALF (33)

#define TILE_WIDTH       64
#define TILE_HEIGHT      32
#define TILE_WIDTH_HALF  32
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
    void UpdateWaterAnimation(double dt);
    void IsometricDrawMap(int index, Dokev::RenderState selected_renderer = Dokev::RenderState::INSTANCED, const Dokev::MouseSelection* mouseSelection = nullptr, Dokev::GameObjectManager* gameObjMgr = nullptr) const;

    void IsometricDrawMouseAtlas(int index, Dokev::RenderState selected_renderer = Dokev::RenderState::INSTANCED) const;
    void EnsureMouseAtlasInitialized() const;

    GLuint GetMouseAtlasFBO() const
    {
        return mouseAtlasFBO;
    }

    void                    ResetPerlinTexture();
    const Dokev::Texture2D* GetPerlinTexture() const;
    GridCell**              GetViewArray(int viewIndex) const;

    void SetMapVariant(MapVariant variant);

    MapVariant GetMapVariant() const
    {
        return mapVariant;
    }

    const MapGenerationProfile& GetGenerationProfile() const
    {
        return mapProfile;
    }

    double GetWaterWaveOffset(int gridX, int gridY) const;

    // Path Finding
    void InitializePathFinding();
    void HandlePathFindingInput();
    void FindPath(Math::fvec2 start, Math::fvec2 end);
    void FindPath(int startX, int startY, int endX, int endY);
    void ClearPath();
    void DrawCurrentPathIsometric(CS200::RGBA color, int viewIndex);
    void SetWalkable(int x, int y, bool walkable);
    bool IsWalkable(int x, int y);
    void SetPathCost(int x, int y, float cost);

    // //For Human ai
    std::vector<Math::fvec2> GetCurrentPath() const
    {
        return currentPath;
    }

    int GetIsoOriginX() const
    {
        return isoOriginX;
    }

    int GetIsoOriginY() const
    {
        return isoOriginY;
    }

    Math::fvec2 TransformPathPointForView(Math::fvec2 point, int viewIndex) const;

    // TileManager access
    Dokev::TileManager* GetTileManager()
    {
        return tileManager.get();
    }

    const Dokev::TileManager* GetTileManager() const
    {
        return tileManager.get();
    }


private:
    void UpdateIsoOrigin();
    void rotatePoint90(float x, float y, float tx, float ty, int angleDeg, float& outX, float& outY) const;

    std::vector<GridCell> IsoGridMapArray;

    std::vector<std::vector<GridCell>> m_gridCells;


    mutable GridCell* view1[GRID_SIZE_X * GRID_SIZE_Y];
    mutable GridCell* view2[GRID_SIZE_X * GRID_SIZE_Y];
    mutable GridCell* view3[GRID_SIZE_X * GRID_SIZE_Y];
    mutable GridCell* view4[GRID_SIZE_X * GRID_SIZE_Y];

    Dokev::Texture2D      IsoGridTexture[4];
    Dokev::AnimatedSprite waterAnimation;

    std::vector<GridCell> GridMapArray;
    Dokev::Image          PerlinNoiseImage;
    Dokev::Texture2D      PerlinNoiseTexture;

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
    std::vector<Math::fvec2>     currentPath;
    bool                         showPath;

    MapVariant           mapVariant = MapVariant::Balanced;
    MapGenerationProfile mapProfile = GetMapProfile(MapVariant::Balanced);

    // Tile management for buildings and roads
    std::unique_ptr<Dokev::TileManager> tileManager;

    Dokev::Texture2D roadTileTextures[15];
    int roadConnectionMask[GRID_SIZE_Y][GRID_SIZE_X];

public:

    // Called by Road when a road tiles connections change
    void SetRoadConnectionMask(int x, int y, int mask)
    {
        if (x >= 0 && x < GRID_SIZE_X && y >= 0 && y < GRID_SIZE_Y)
            roadConnectionMask[y][x] = mask;
    }

    void ClearRoadConnectionMask(int x, int y)
    {
        if (x >= 0 && x < GRID_SIZE_X && y >= 0 && y < GRID_SIZE_Y)
            roadConnectionMask[y][x] = -1;
    }


    mutable GLuint mouseAtlasFBO = 0;
    mutable GLuint mouseAtlasTex = 0;
};
