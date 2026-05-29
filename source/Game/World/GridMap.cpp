/**
 * \file GridMap.cpp
 * \brief
 * \author Junseok Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "GridMap.hpp"
#include "Dokev/Core/DKColor.hpp"
#include "Dokev/Core/DKMetaProgramming.hpp"
#include "Dokev/Core/DKUtility.hpp"
#include "Dokev/State/DKGameState.hpp"
#include "Dokev/World/DKGrid.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Logger.hpp"
#include "Engine/Path.hpp"
#include "Engine/Texture.hpp"
#include "Engine/TextureManager.hpp"
#include "Engine/Window.hpp"
#include "Game/Entities/Building.hpp"
#include "Game/Input/DKMouseSelection.hpp"

#include "OpenGL/GL.hpp"
#include <GL/glew.h>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace
{
    MapGenerationProfile MakeProfile(MapVariant variant)
    {
        switch (variant)
        {
            case MapVariant::WaterHeavy:
                return MapGenerationProfile{
                    MapVariant::WaterHeavy, TerrainThresholds{ 170.0, 210.0, 235.0 },
                     0.08f, "Water Heavy"
                };
            case MapVariant::PlainsHeavy:
                return MapGenerationProfile{
                    MapVariant::PlainsHeavy, TerrainThresholds{ 50.0, 235.0, 255.0 },
                     0.12f, "Plains Heavy"
                };
            case MapVariant::MountainHeavy:
                return MapGenerationProfile{
                    MapVariant::MountainHeavy, TerrainThresholds{ 70.0, 130.0, 180.0 },
                     0.09f, "Mountain Heavy"
                };
            case MapVariant::Balanced:
            default:
                return MapGenerationProfile{
                    MapVariant::Balanced, TerrainThresholds{ 80.0, 155.0, 210.0 },
                     0.1f, "Balanced"
                };
        }
    }
}

MapGenerationProfile GetMapProfile(MapVariant variant)
{
    return MakeProfile(variant);
}

const char* GetMapVariantLabel(MapVariant variant)
{
    return MakeProfile(variant).label;
}

GridMap::GridMap() : currentView(0), pathFinder(nullptr), showPath(false)
{
    PlainsTexture   = Dokev::LoadTexture(assets::locate_asset("Assets/images/textures/PlaneGrid4.png").string());
    WaterTexture    = Dokev::LoadTexture(assets::locate_asset("Assets/images/textures/IsoWater.png").string());
    MountainTexture = Dokev::LoadTexture(assets::locate_asset("Assets/images/textures/IsoMountains.png").string());
    SnowTexture     = Dokev::LoadTexture(assets::locate_asset("Assets/images/textures/IsoPeak.png").string());

    Max_Height    = Dokev::LoadTexture(assets::locate_asset("Assets/images/textures/Max_Height.png").string());
    IsoMouseAtlas = Dokev::LoadTexture(assets::locate_asset("Assets/images/textures/IsoMouseAtlas.png").string());

    static const int roadTileNums[] = {1, 2, 3, 4, 6, 7, 8, 9, 10, 11, 12, 13, 14};
    for (int n : roadTileNums)
    {
        std::string path    = "Assets/images/road/Road1_tile_" + std::to_string(n) + ".png";
        roadTileTextures[n] = Dokev::LoadTexture(assets::locate_asset(path).string());
    }

    for (int y = 0; y < GRID_SIZE_Y; ++y)
        for (int x = 0; x < GRID_SIZE_X; ++x)
            roadConnectionMask[y][x] = -1;

    IsoGridTexture[0] = WaterTexture;    // water (fallback)
    IsoGridTexture[1] = PlainsTexture;   // plane
    IsoGridTexture[2] = MountainTexture; // mountain
    IsoGridTexture[3] = SnowTexture;

    waterAnimation = Dokev::AnimatedSprite(assets::locate_asset("Assets/images/water.spt"));

    ResetPerlinTexture();
    ResetIsoGridMapArray();

    // Initialize TileManager
    tileManager = std::make_unique<Dokev::TileManager>();
    tileManager->Initialize(GRID_SIZE_X, GRID_SIZE_Y);

    //////////////////////////////////////////////////////////////////////////

    GL::GenFramebuffers(1, &mouseAtlasFBO);
    GL::BindFramebuffer(GL_FRAMEBUFFER, mouseAtlasFBO);

    // this texture will save the colors
    GL::GenTextures(1, &mouseAtlasTex);
    GL::BindTexture(GL_TEXTURE_2D, mouseAtlasTex);

    GL::TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1200, 800, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    // No filtering
    GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Attach it as color attachment 0
    GL::FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mouseAtlasTex, 0); // this makes it so that anything drawn goes to the texture

    // Specify draw buffer list (only one)
    GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    GL::DrawBuffers(1, drawBuffers);
    // “This FBO has 1 color output, and it’s color attachment 0.”


    GL::BindFramebuffer(GL_FRAMEBUFFER, 0);
    GL::BindTexture(GL_TEXTURE_2D, 0);
};

void GridMap::UpdateWaterAnimation(double dt)
{
    waterAnimation.Update(dt);
}

void GridMap::ReGenerateMap()
{
    ResetPerlinTexture();
    ResetIsoGridMapArray();
}

namespace
{
    constexpr int MOUSE_ATLAS_WIDTH  = 1200;
    constexpr int MOUSE_ATLAS_HEIGHT = 800;
}

void GridMap::EnsureMouseAtlasInitialized() const
{
    auto windowSize   = Engine::GetWindow().GetSize();
    int  targetWidth  = std::max(1, windowSize.x);
    int  targetHeight = std::max(1, windowSize.y);

    if (mouseAtlasFBO != 0 && mouseAtlasTex != 0)
    {
        // Resize texture if window size changed
        if (mouseAtlasW == targetWidth && mouseAtlasH == targetHeight)
        {
            return;
        }
        GL::BindTexture(GL_TEXTURE_2D, mouseAtlasTex);
    }

    if (mouseAtlasFBO == 0)
    {
        GL::GenFramebuffers(1, &mouseAtlasFBO);
    }
    GL::BindFramebuffer(GL_FRAMEBUFFER, mouseAtlasFBO);

    if (mouseAtlasTex == 0)
    {
        GL::GenTextures(1, &mouseAtlasTex);
    }
    GL::BindTexture(GL_TEXTURE_2D, mouseAtlasTex);

    GL::TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, targetWidth, targetHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    mouseAtlasW = targetWidth;
    mouseAtlasH = targetHeight;

    GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    GL::FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mouseAtlasTex, 0);

    GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    GL::DrawBuffers(1, drawBuffers);

    if (GL::CheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        Engine::GetLogger().LogError("mouseAtlasFBO incomplete; skipping mouse atlas draw.");
        if (mouseAtlasTex != 0)
        {
            GL::DeleteTextures(1, &mouseAtlasTex);
            mouseAtlasTex = 0;
            mouseAtlasW   = 0;
            mouseAtlasH   = 0;
        }
        if (mouseAtlasFBO != 0)
        {
            GL::DeleteFramebuffers(1, &mouseAtlasFBO);
            mouseAtlasFBO = 0;
        }
        GL::BindFramebuffer(GL_FRAMEBUFFER, 0);
        GL::BindTexture(GL_TEXTURE_2D, 0);
        return;
    }

    GL::BindFramebuffer(GL_FRAMEBUFFER, 0);
    GL::BindTexture(GL_TEXTURE_2D, 0);
}

void GridMap::ResetIsoGridMapArray()
{
    m_gridCells.assign(GRID_SIZE_Y, std::vector<GridCell>(GRID_SIZE_X));

    TerrainThresholds thresholds = mapProfile.thresholds;

    std::vector<unsigned int> heights;
    heights.reserve(GRID_SIZE_X * GRID_SIZE_Y);

    // Process sequentially as GetImageColor may not be thread-safe
    for (int y = 0; y < GRID_SIZE_Y; ++y)
    {
        for (int x = 0; x < GRID_SIZE_X; ++x)
        {
            CS200::RGBA  color  = Dokev::GetImageColor(PerlinNoiseImage, x, y);
            unsigned int Height = (color >> 24) & 0xFF;
            heights.push_back(Height);
        }
    }

    if (mapVariant == MapVariant::WaterHeavy && !heights.empty())
    {
        // Raise the water cutoff to at least the median height so 50%+ of tiles become water.
        auto medianIt = heights.begin() + heights.size() / 2;
        std::nth_element(heights.begin(), medianIt, heights.end());
        double medianHeight  = static_cast<double>(*medianIt);
        thresholds.waterMax  = std::max(thresholds.waterMax, medianHeight);
        thresholds.plainsMax = std::max(thresholds.plainsMax, thresholds.waterMax + 1.0);
    }

    mapProfile.thresholds = thresholds;

    size_t idx = 0;
    for (int y = 0; y < GRID_SIZE_Y; ++y)
    {
        for (int x = 0; x < GRID_SIZE_X; ++x)
        {
            double heightVal = static_cast<double>(heights[idx++]);

            m_gridCells[y][x] = GridCell{ heightVal, x, y, thresholds };
        }
    }

    UpdateIsoOrigin();
    InitializeIsoView();
}

void GridMap::UpdateIsoOrigin()
{
    isoOriginX = 0;
    isoOriginY = 0;

    if (!m_gridCells.empty() && !m_gridCells[0].empty())
    {
        isoOriginY = static_cast<int>(m_gridCells[0][0].getHeight());
    }
}

void GridMap::InitializeIsoView()
{
    std::fill(std::begin(view1), std::end(view1), nullptr);
    std::fill(std::begin(view2), std::end(view2), nullptr);
    std::fill(std::begin(view3), std::end(view3), nullptr);
    std::fill(std::begin(view4), std::end(view4), nullptr);

    float new_x = 0;
    float new_y = 0;

    for (int y = 0; y < GRID_SIZE_Y; ++y)
    {
        for (int x = 0; x < GRID_SIZE_X; ++x)
        {
            // Original view
            view1[Dokev::Meta::grid_to_index(x, y, GRID_SIZE_X)] = &m_gridCells[y][x];

            // 90 degree rotation
            rotatePoint90(static_cast<float>(x), static_cast<float>(y), GRID_SIZE_X_HALF, GRID_SIZE_Y_HALF, 90, new_x, new_y);
            int rx90                                                                                             = std::clamp(static_cast<int>(std::round(new_x)), 0, GRID_SIZE_X - 1);
            int ry90                                                                                             = std::clamp(static_cast<int>(std::round(new_y)), 0, GRID_SIZE_Y - 1);
            view2[Dokev::Meta::grid_to_index(static_cast<size_t>(rx90), static_cast<size_t>(ry90), GRID_SIZE_X)] = &m_gridCells[y][x];

            // 180 degree rotation
            rotatePoint90(static_cast<float>(x), static_cast<float>(y), GRID_SIZE_X_HALF, GRID_SIZE_Y_HALF, 180, new_x, new_y);
            int rx180                                                                                              = std::clamp(static_cast<int>(std::round(new_x)), 0, GRID_SIZE_X - 1);
            int ry180                                                                                              = std::clamp(static_cast<int>(std::round(new_y)), 0, GRID_SIZE_Y - 1);
            view3[Dokev::Meta::grid_to_index(static_cast<size_t>(rx180), static_cast<size_t>(ry180), GRID_SIZE_X)] = &m_gridCells[y][x];

            // 270 degree rotation
            rotatePoint90(static_cast<float>(x), static_cast<float>(y), GRID_SIZE_X_HALF, GRID_SIZE_Y_HALF, 270, new_x, new_y);
            int rx270                                                                                              = std::clamp(static_cast<int>(std::round(new_x)), 0, GRID_SIZE_X - 1);
            int ry270                                                                                              = std::clamp(static_cast<int>(std::round(new_y)), 0, GRID_SIZE_Y - 1);
            view4[Dokev::Meta::grid_to_index(static_cast<size_t>(rx270), static_cast<size_t>(ry270), GRID_SIZE_X)] = &m_gridCells[y][x];
        }
    }
}

static int rotateMask4(int mask, int view)
{
    int v = view & 3;
    if (v == 0) return mask;
    return ((mask << v) | (mask >> (4 - v))) & 0xF;
}

static int maskToTileIndex(int mask)
{
    switch (mask)
    {
        case 0:  return 8;
        case 1:  return 2;   // N only → N-S style (consistent with N+S straight)
        case 2:  return 3;   // E only → E-W style (consistent with E+W straight)
        case 4:  return 2;   // S only → N-S style (consistent with N+S straight)
        case 8:  return 3;   // W only → E-W style (consistent with E+W straight)
        case 5:  return 2;   // N+S straight
        case 10: return 3;   // E+W straight
        case 9:  return 9;   // N+W corner
        case 12: return 6;   // S+W corner
        case 6:  return 1;   // E+S corner
        case 3:  return 4;   // N+E corner
        case 7:  return 10;  // T: N+E+S (no W)
        case 11: return 13;  // T: N+E+W (no S)
        case 13: return 14;  // T: N+S+W (no E)
        case 14: return 11;  // T: E+S+W (no N)
        case 15: return 12;  // 4-way
        default: return 2;
    }
}

namespace
{
    struct BuildingDrawTrigger
    {
        int  originalX = 0;
        int  originalY = 0;
        int  viewX     = 0;
        int  viewY     = 0;
        int  order     = 0;
        bool valid     = false;
    };

    BuildingDrawTrigger SelectBuildingDrawTrigger(const Building& building, const GridMap& map, int viewIndex)
    {
        BuildingDrawTrigger result;

        const int baseX = static_cast<int>(std::round(building.GetGrid().x));
        const int baseY = static_cast<int>(std::round(building.GetGrid().y));

        for (int y = 0; y < building.GetHeight(); ++y)
        {
            for (int x = 0; x < building.GetWidth(); ++x)
            {
                const int originalX = baseX + x;
                const int originalY = baseY + y;

                Math::fvec2 transformed = map.TransformPathPointForView({ static_cast<float>(originalX), static_cast<float>(originalY) }, viewIndex);
                const int   viewX       = std::clamp(static_cast<int>(std::round(transformed.x)), 0, GRID_SIZE_X - 1);
                const int   viewY       = std::clamp(static_cast<int>(std::round(transformed.y)), 0, GRID_SIZE_Y - 1);
                const int   order       = viewX + viewY;

                if (!result.valid || order > result.order || (order == result.order && viewX > result.viewX))
                {
                    result = { originalX, originalY, viewX, viewY, order, true };
                }
            }
        }

        return result;
    }
}

void GridMap::IsometricDrawMap(int index, Dokev::RenderState selected_renderer, const Dokev::MouseSelection* mouseSelection, Dokev::GameObjectManager* gameObjMgr) const
{
    currentView = index % 4;

    GridCell* const* selectedView = nullptr;
    switch (index % 4)
    {
        case 0: selectedView = view1; break;
        case 1: selectedView = view2; break;
        case 2: selectedView = view3; break;
        case 3: selectedView = view4; break;
        default: selectedView = view1; break;
    }
    if (!selectedView || m_gridCells.empty())
    {
        Engine::GetLogger().LogError("selectedView is null or m_gridCells is empty!");
        return;
    }

    int originX = isoOriginX;
    int originY = isoOriginY;
    for (int sum = 0; sum < GRID_SIZE_X + GRID_SIZE_Y - 1; ++sum)
    {
        for (int x = 0; x <= sum; ++x)
        {
            int y = sum - x;

            if (x >= 0 && x < GRID_SIZE_X && y >= 0 && y < GRID_SIZE_Y)
            {
                int screenX = originX + (x - y) * TILE_WIDTH_HALF;
                int screenY = originY + (x + y) * TILE_HEIGHT_HALF;

                int count = y * GRID_SIZE_X + x;
                if (!selectedView[count])
                {
                    continue;
                }
                double height       = selectedView[count]->getHeight();
                double renderHeight = height;
                int    topY         = static_cast<int>(renderHeight) - screenY;

                int originalX = selectedView[count]->x;
                int originalY = selectedView[count]->y;

                Dokev::TileType tileType        = tileManager->GetTileType(originalX, originalY);
                int             current_texture = 3;
                if (height < mapProfile.thresholds.mountainMax)
                    current_texture = 2;
                if (height < mapProfile.thresholds.plainsMax)
                    current_texture = 1;
                if (height < mapProfile.thresholds.waterMax)
                    current_texture = 0;

                if (tileType == Dokev::TileType::ROAD)
                {
                    if (current_texture == 0 && mapVariant != MapVariant::WaterHeavy)
                    {
                        renderHeight += GetWaterWaveOffset(originalX, originalY);
                    }
                    topY = static_cast<int>(renderHeight) - screenY;


                    int rawMask     = roadConnectionMask[originalY][originalX];
                    int rotatedMask = rotateMask4(rawMask < 0 ? 0 : rawMask, currentView);
                    int tileIdx     = maskToTileIndex(rotatedMask);
                    Dokev::DrawTexture(roadTileTextures[tileIdx], screenX, topY + TILE_HEIGHT, Dokev::ColorPalette::WHITE, selected_renderer);

                    Dokev::DrawTexture(Max_Height, screenX, topY, Dokev::ColorPalette::WHITE, selected_renderer);
                }
                else
                {
                    // Only apply wave height animation on non-water-heavy variants to avoid visual overlap with buildings/roads.
                    // if (current_texture == 0 && mapVariant != MapVariant::WaterHeavy)
                    // {
                    //     renderHeight += GetWaterWaveOffset(originalX, originalY);
                    // }

                    topY = static_cast<int>(renderHeight) - screenY;

                    if (current_texture == 0 && waterAnimation.GetSpriteSheet().IsValid())
                    {
                        waterAnimation.DrawAtPosition(screenX, topY);
                    }
                    else
                    {
                        Dokev::DrawTexture(IsoGridTexture[current_texture], screenX, topY, Dokev::ColorPalette::WHITE, selected_renderer);
                    }
                    Dokev::DrawTexture(Max_Height, screenX, topY, Dokev::ColorPalette::WHITE, selected_renderer);

                    if (gameObjMgr)
                    {
                        auto& buildings = gameObjMgr->GetBuildings();

                        for (const auto& building : buildings)
                        {
                            BuildingDrawTrigger trigger = SelectBuildingDrawTrigger(*building, *this, currentView);
                            if (trigger.valid && trigger.originalX == originalX && trigger.originalY == originalY)
                            {
                                building->Draw(index, this, screenX, topY + 64);
                            }
                        }
                    }
                }

                if (mouseSelection && mouseSelection->ShouldDrawAtTile(originalX, originalY))
                {
                    mouseSelection->DrawAtTile(screenX, topY);
                }
            }
        }
    }
}

// takes in an index for the selected "view" to genereate the mouse atlas.
// intializes the member variable array IsoAtlasTexture with the texture created. later unpack the rgba values in the texture to get the grid.
void GridMap::IsometricDrawMouseAtlas(int index, Dokev::RenderState selected_renderer) const
{
    (void)selected_renderer;
    EnsureMouseAtlasInitialized();
    if (mouseAtlasFBO == 0 || mouseAtlasTex == 0)
    {
        Engine::GetLogger().LogError("mouseAtlas FBO/texture not initialized; skipping mouse atlas draw.");
        return;
    }

    GL::BindFramebuffer(GL_FRAMEBUFFER, mouseAtlasFBO);

    currentView = (index % 4) + 1;

    GridCell* const* selectedView = nullptr;
    switch (index % 4)
    {
        case 0: selectedView = view1; break;
        case 1: selectedView = view2; break;
        case 2: selectedView = view3; break;
        case 3: selectedView = view4; break;
        default: selectedView = view1; break;
    }

    if (!selectedView || m_gridCells.empty())
    {
        Engine::GetLogger().LogError("selectedView is null or m_gridCells is empty!");
        return;
    }

    int originX = isoOriginX;
    int originY = isoOriginY;

    for (int sum = 0; sum < GRID_SIZE_X + GRID_SIZE_Y - 1; ++sum)
    {
        for (int x = 0; x <= sum; ++x)
        {
            int y = sum - x;

            if (x >= 0 && x < GRID_SIZE_X && y >= 0 && y < GRID_SIZE_Y)
            {
                int screenX = originX + (x - y) * TILE_WIDTH_HALF;
                int screenY = originY + (x + y) * TILE_HEIGHT_HALF;


                int count = y * GRID_SIZE_X + x;
                if (!selectedView[count])
                {
                    continue;
                }
                double height = selectedView[count]->getHeight();
                int    topY   = static_cast<int>(height) - screenY;

                // int originalX = selectedView[count]->x;
                // int originalY = selectedView[count]->y;

                // Dokev::TileType tileType =
                //     tileManager->GetTileType(originalX, originalY);


                /*if (tileType == Dokev::TileType::ROAD)
                {
                    Dokev::DrawTexture(RoadTexture, screenX, topY, Dokev::ColorPalette::WHITE, Dokev::RenderState::IMMEDIATE);
                    Dokev::DrawTexture(Max_Height, screenX, topY, Dokev::ColorPalette::WHITE, Dokev::RenderState::IMMEDIATE);
                }*/


                int current_texture = 3;
                if (height < mapProfile.thresholds.mountainMax)
                    current_texture = 2;
                if (height < mapProfile.thresholds.plainsMax)
                    current_texture = 1;
                if (height < mapProfile.thresholds.waterMax)
                    current_texture = 0;


                std::array<float, 4> result;
                result[0] = static_cast<float>(x) / (GRID_SIZE_X - 1);
                result[1] = static_cast<float>(y) / (GRID_SIZE_Y - 1);
                result[2] = 0;
                result[3] = 255;

                CS200::RGBA drawtint = CS200::pack_color(result);

                // Dokev::DrawTexture(IsoGridTexture[current_texture], screenX, topY, drawtint ,Dokev::RenderState::IMMEDIATE);
                Dokev::DrawTexture(IsoMouseAtlas, screenX, topY, drawtint, Dokev::RenderState::IMMEDIATE);
            }
        }
    }
    GL::BindFramebuffer(GL_FRAMEBUFFER, 0); // restore
}

void GridMap::ResetPerlinTexture()
{
    int   seedX        = Dokev::GetRandomValue(1, 10000);
    int   seedY        = Dokev::GetRandomValue(1, 10000);
    float noiseScale   = mapProfile.noiseScale <= 0.0f ? 0.1f : mapProfile.noiseScale;
    PerlinNoiseImage   = Dokev::GenImagePerlinNoise(GRID_SIZE_X, GRID_SIZE_Y, seedX, seedY, noiseScale);
    PerlinNoiseTexture = LoadTextureFromImage(PerlinNoiseImage);

    ExportImage(PerlinNoiseImage, "PerlinNoise.png");
}

const Dokev::Texture2D* GridMap::GetPerlinTexture() const
{
    return &PerlinNoiseTexture;
}

double GridMap::GetWaterWaveOffset(int gridX, int gridY) const
{
    const auto& env   = Engine::GetWindowEnvironment();
    double      time  = env.ElapsedTime;
    // Wave travels from top-right toward bottom-left by using inverse X in the phase.
    double      phase = (static_cast<double>((GRID_SIZE_X - gridX) + gridY) * 0.35);
    double      wave  = std::sin(time * 1.8 + phase) * 6.0; // amplitude in pixels
    return wave;
}

void GridMap::SetMapVariant(MapVariant variant)
{
    mapVariant = variant;
    mapProfile = GetMapProfile(variant);
}

GridCell** GridMap::GetViewArray(int viewIndex) const
{
    assert(viewIndex >= 0 && viewIndex < 4 && "View index out of range");

    switch (viewIndex)
    {
        case 0: return view1;
        case 1: return view2;
        case 2: return view3;
        case 3: return view4;
        default: return view1;
    }
}

void GridMap::rotatePoint90(float x, float y, float tx, float ty, int angleDeg, float& oX, float& oY) const
{
    float dx = x - tx;
    float dy = y - ty;

    int rot = ((angleDeg % 360) + 360) % 360;

    switch (rot)
    {
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

//------------------------------------Path finding-------------------------------------//

void GridMap::InitializePathFinding()
{
    if (pathFinder)
    {
        pathFinder.reset();
        std::cout << "clear the pathfind" << std::endl;
    }

    pathFinder = std::make_unique<PathFinding>(GRID_SIZE_X, GRID_SIZE_Y, 50.0f);

    pathFinder->UpdateMapFromGrid(m_gridCells);

    std::cout << "PathFinding initialized with UpdateMapFromGrid" << std::endl;
}

void GridMap::HandlePathFindingInput()
{
    ClearPath();
}

void GridMap::FindPath(Math::fvec2 start, Math::fvec2 end)
{
    if (!pathFinder)
    {
        std::cout << "No pathfinder" << std::endl;
        return;
    }
    currentPath = pathFinder->FindPath(start, end);
    showPath    = true;
}

void GridMap::FindPath(int startX, int startY, int endX, int endY)
{
    FindPath({ static_cast<float>(startX), static_cast<float>(startY) }, { static_cast<float>(endX), static_cast<float>(endY) });
}

void GridMap::ClearPath()
{
    currentPath.clear();
    showPath = false;
}

void GridMap::DrawCurrentPathIsometric(CS200::RGBA color, int viewIndex)
{
    if (!pathFinder || currentPath.empty() || !showPath)
    {
        return;
    }

    GridCell** selectedView = GetViewArray(viewIndex);

    int originX = 0;
    int originY = static_cast<int>(selectedView[0]->getHeight());

    std::vector<Math::fvec2> transformedPath;
    transformedPath.reserve(currentPath.size());

    for (const auto& point : currentPath)
    {
        Math::fvec2 transformedPoint = TransformPathPointForView(point, viewIndex);
        transformedPath.push_back(transformedPoint);
    }


    for (size_t i = 0; i < transformedPath.size(); i++)
    {
        int x = static_cast<int>(transformedPath[i].x);
        int y = static_cast<int>(transformedPath[i].y);

        if (x < 0 || x >= GRID_SIZE_X || y < 0 || y >= GRID_SIZE_Y)
        {
            continue;
        }
        float       currentHeight = static_cast<float>(selectedView[Dokev::Meta::grid_to_index(x, y, GRID_SIZE_X)]->getHeight());
        Math::fvec2 worPos        = Dokev::Grid::WorldToIsometric(static_cast<float>(x), static_cast<float>(y), currentHeight, originX, originY, TILE_WIDTH_HALF, TILE_HEIGHT_HALF);
        Dokev::DrawCircle(static_cast<int>(worPos.x), static_cast<int>(worPos.y), 5, color);

        if (i < transformedPath.size() - 1)
        {
            int nextX = static_cast<int>(transformedPath[i + 1].x);
            int nextY = static_cast<int>(transformedPath[i + 1].y);

            if (nextX < 0 || nextX >= GRID_SIZE_X || nextY < 0 || nextY >= GRID_SIZE_Y)
            {
                continue;
            }

            float nextHeight = static_cast<float>(selectedView[Dokev::Meta::grid_to_index(nextX, nextY, GRID_SIZE_X)]->getHeight());

            Math::fvec2 nextPos = Dokev::Grid::WorldToIsometric(static_cast<float>(nextX), static_cast<float>(nextY), nextHeight, originX, originY, TILE_WIDTH_HALF, TILE_HEIGHT_HALF);

            Dokev::DrawLineEx({ (float)worPos.x, (float)worPos.y }, { (float)nextPos.x, (float)nextPos.y }, 2.0f, color);
        }
    }
}

void GridMap::SetWalkable(int x, int y, bool walkable)
{
    if (pathFinder)
    {
        pathFinder->SetWalkable(x, y, walkable);
    }
}

bool GridMap::IsWalkable(int x, int y)
{
    if (pathFinder)
    {
        return pathFinder->IsPositionWalkable(x, y);
    }
    return false;
}

void GridMap::SetPathCost(int x, int y, float cost)
{
    if (pathFinder)
    {
        pathFinder->SetTileCost(x, y, cost);
    }
}

Math::fvec2 GridMap::TransformPathPointForView(Math::fvec2 point, int viewIndex) const
{
    float x = point.x;
    float y = point.y;

    float newX, newY;

    switch (viewIndex)
    {
        case 0: return point;
        case 1: rotatePoint90(x, y, GRID_SIZE_X_HALF, GRID_SIZE_Y_HALF, 90, newX, newY); return { newX, newY };
        case 2: rotatePoint90(x, y, GRID_SIZE_X_HALF, GRID_SIZE_Y_HALF, 180, newX, newY); return { newX, newY };
        case 3: rotatePoint90(x, y, GRID_SIZE_X_HALF, GRID_SIZE_Y_HALF, 270, newX, newY); return { newX, newY };
        default: return point;
    }
}

bool GridMap::CanFlattenTile(int x, int y) const
{
    if (x < 0 || x >= GRID_SIZE_X || y < 0 || y >= GRID_SIZE_Y)
        return false;

    const GridCell& cell = m_gridCells[y][x];

    if (cell.getWater())
        return false;

    if (cell.getFlattenStage() >= 3)
        return false;

    double targetHeight = mapProfile.thresholds.waterMax;
    return std::abs(cell.getHeight() - targetHeight) > 1e-9;
}

bool GridMap::FlattenTile(int x, int y)
{
    if (!CanFlattenTile(x, y))
        return false;

    GridCell& cell        = m_gridCells[y][x];
    double    targetHeight = mapProfile.thresholds.waterMax;

    int stage = cell.getFlattenStage();

    if (stage == 0)
        cell.setFlattenBaseHeight(cell.getHeight());

    stage++;
    cell.setFlattenStage(stage);

    double baseH     = cell.getFlattenBaseHeight();
    double newHeight = (stage >= 3) ? targetHeight
                                    : baseH + (targetHeight - baseH) * (stage / 3.0);

    cell.ApplyTerrain(newHeight, mapProfile.thresholds);

    if (stage >= 3)
    {
        SetPathCost(x, y, 1.0f);
        SetWalkable(x, y, true);
    }

    return true;
}

bool GridMap::LevelAreaToAverage(int startX, int startY, int width, int height)
{
    if (width <= 0 || height <= 0)
    {
        return false;
    }

    double totalHeight = 0.0;

    for (int y = startY; y < startY + height; ++y)
    {
        for (int x = startX; x < startX + width; ++x)
        {
            if (x < 0 || x >= GRID_SIZE_X || y < 0 || y >= GRID_SIZE_Y)
            {
                return false;
            }

            GridCell& cell = m_gridCells[y][x];
            if (cell.getWater())
            {
                return false;
            }

            totalHeight += cell.getHeight();
        }
    }

    const double targetHeight = totalHeight / static_cast<double>(width * height);

    for (int y = startY; y < startY + height; ++y)
    {
        for (int x = startX; x < startX + width; ++x)
        {
            GridCell& cell = m_gridCells[y][x];
            cell.ApplyTerrain(targetHeight, mapProfile.thresholds);
            SetPathCost(x, y, cell.getMovementCost());
            SetWalkable(x, y, cell.isWalkable());
        }
    }

    return true;
}
