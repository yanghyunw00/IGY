/**
 * \file GridMap.cpp
 * \brief
 * \author Junseok Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "GridMap.hpp"
#include "Dokev/DKMouseSelection.hpp"
#include "Dokev/DKColor.hpp"
#include "Dokev/DKGrid.hpp"
#include "Dokev/DKUtility.hpp"
#include "Dokev/DKMetaProgramming.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Logger.hpp"
#include "Engine/Path.hpp"
#include "Engine/Texture.hpp"
#include "Engine/TextureManager.hpp"
#include "Engine/Window.hpp"
#include <GL/glew.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

GridMap::GridMap() : currentView(0), pathFinder(nullptr), showPath(false)
{
    PlainsTexture   = Dokev::LoadTexture(assets::locate_asset("Assets/images/textures/IsoPlains.png").string());
    WaterTexture    = Dokev::LoadTexture(assets::locate_asset("Assets/images/textures/IsoWater.png").string());
    MountainTexture = Dokev::LoadTexture(assets::locate_asset("Assets/images/textures/IsoMountains.png").string());
    SnowTexture     = Dokev::LoadTexture(assets::locate_asset("Assets/images/textures/IsoPeak.png").string());
    Max_Height      = Dokev::LoadTexture(assets::locate_asset("Assets/images/textures/Max_Height.png").string());
    RoadTexture     = Dokev::LoadTexture(assets::locate_asset("Assets/images/textures/road.png").string());
    IsoMouseAtlas   = Dokev::LoadTexture(assets::locate_asset("Assets/images/textures/IsoMouseAtlas.png").string());

    IsoGridTexture[0] = WaterTexture;
    IsoGridTexture[1] = PlainsTexture;
    IsoGridTexture[2] = MountainTexture;
    IsoGridTexture[3] = SnowTexture;

    ResetPerlinTexture();

    ResetIsoGridMapArray();

    if (!IsoGridMapArray.empty())
    {
        // isoOriginX = ((TILE_WIDTH / 2) * GRID_SIZE_X) - TILE_WIDTH / 2;
        isoOriginX = 0;
        isoOriginY = static_cast<int>(IsoGridMapArray[0].getHeight());
        std::cout << "isoOrigin set: X=" << isoOriginX << ", Y=" << isoOriginY << std::endl;
    }


    InitializeIsoView();


    // Initialize TileManager
    tileManager = std::make_unique<Dokev::TileManager>();
    tileManager->Initialize(GRID_SIZE_X, GRID_SIZE_Y);

    //////////////////////////////////////////////////////////////////////////

    glGenFramebuffers(1, &mouseAtlasFBO);

    glBindFramebuffer(GL_FRAMEBUFFER, mouseAtlasFBO);

    // this texture will save the colors
    glGenTextures(1, &mouseAtlasTex);
    glBindTexture(GL_TEXTURE_2D, mouseAtlasTex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1200, 800, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    // No filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Attach it as color attachment 0
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mouseAtlasTex, 0); // this makes it so that anything drawn goes to the texture

    // Specify draw buffer list (only one)
    GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);
    // “This FBO has 1 color output, and it’s color attachment 0.”


    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
};

void GridMap::ReGenerateMap()
{
    ResetPerlinTexture();
    ResetIsoGridMapArray();
    InitializeIsoView();
    IsometricDrawMap(1);
    IsometricDrawMap(2);
    IsometricDrawMap(3);
    IsometricDrawMap(4);
}

void GridMap::ResetIsoGridMapArray()
{
    m_gridCells.assign(GRID_SIZE_Y, std::vector<GridCell>(GRID_SIZE_X));

    // Process sequentially as GetImageColor may not be thread-safe
    for (int y = 0; y < GRID_SIZE_Y; ++y)
    {
        for (int x = 0; x < GRID_SIZE_X; ++x)
        {
            CS200::RGBA  color  = Dokev::GetImageColor(PerlinNoiseImage, x, y);
            unsigned int Height = (color >> 24) & 0xFF;

            m_gridCells[y][x] = GridCell{ (double)Height, x, y };
        }
    }
}

void GridMap::InitializeIsoView()
{
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
            size_t rotIndex90 = Dokev::Meta::grid_to_index(static_cast<size_t>(new_x), static_cast<size_t>(new_y), GRID_SIZE_X);
            if (rotIndex90 < GRID_SIZE_X * GRID_SIZE_Y)
                view2[rotIndex90] = &m_gridCells[y][x];

            // 180 degree rotation
            rotatePoint90(static_cast<float>(x), static_cast<float>(y), GRID_SIZE_X_HALF, GRID_SIZE_Y_HALF, 180, new_x, new_y);
            size_t rotIndex180 = Dokev::Meta::grid_to_index(static_cast<size_t>(new_x), static_cast<size_t>(new_y), GRID_SIZE_X);
            if (rotIndex180 > 0 && rotIndex180 < GRID_SIZE_X * GRID_SIZE_Y)
                view3[rotIndex180] = &m_gridCells[y][x];

            // 270 degree rotation
            rotatePoint90(static_cast<float>(x), static_cast<float>(y), GRID_SIZE_X_HALF, GRID_SIZE_Y_HALF, 270, new_x, new_y);
            size_t rotIndex270 = Dokev::Meta::grid_to_index(static_cast<size_t>(new_x), static_cast<size_t>(new_y), GRID_SIZE_X);
            if (rotIndex270 < GRID_SIZE_X * GRID_SIZE_Y)
                view4[rotIndex270] = &m_gridCells[y][x];
        }
    }
}

void GridMap::IsometricDrawMap(int index, Dokev::RenderState selected_renderer, const Dokev::MouseSelection* mouseSelection) const
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

    // int offsetupper = static_cast<int>(selectedView[0]->getHeight());
    int originX = 0;
    int originY = 0;
    for (int sum = 0; sum < GRID_SIZE_X + GRID_SIZE_Y - 1; ++sum)
    {
        for (int x = 0; x <= sum; ++x)
        {
            int y = sum - x;

            if (x >= 0 && x < GRID_SIZE_X && y >= 0 && y < GRID_SIZE_Y)
            {
                int screenX = originX + (x - y) * TILE_WIDTH_HALF;
                int screenY = originY + (x + y) * TILE_HEIGHT_HALF;

                int    count  = y * GRID_SIZE_X + x;
                double height = selectedView[count]->getHeight();
                int    topY   = static_cast<int>(height) - screenY;

                int originalX = selectedView[count]->x;
                int originalY = selectedView[count]->y;

                Dokev::TileType tileType = tileManager->GetTileType(originalX, originalY);

                if (tileType == Dokev::TileType::ROAD)
                {
                    Dokev::DrawTexture(RoadTexture, screenX, topY, Dokev::ColorPalette::WHITE, selected_renderer);
                    Dokev::DrawTexture(Max_Height, screenX, topY, Dokev::ColorPalette::WHITE, selected_renderer);
                }
                else
                {
                    int current_texture = 3;
                    if (height < 205)
                        current_texture = 2;
                    if (height < 150)
                        current_texture = 1;
                    if (height < 75)
                        current_texture = 0;
                    Dokev::DrawTexture(IsoGridTexture[current_texture], screenX, topY, Dokev::ColorPalette::WHITE, selected_renderer);
                    Dokev::DrawTexture(Max_Height, screenX, topY, Dokev::ColorPalette::WHITE, selected_renderer);
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
    glBindFramebuffer(GL_FRAMEBUFFER, mouseAtlasFBO);

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

    // int offsetupper = static_cast<int>(selectedView[0]->getHeight());

    int originX = 0;
    int originY = 0;

    for (int sum = 0; sum < GRID_SIZE_X + GRID_SIZE_Y - 1; ++sum)
    {
        for (int x = 0; x <= sum; ++x)
        {
            int y = sum - x;

            if (x >= 0 && x < GRID_SIZE_X && y >= 0 && y < GRID_SIZE_Y)
            {
                int screenX = originX + (x - y) * TILE_WIDTH_HALF;
                int screenY = originY + (x + y) * TILE_HEIGHT_HALF;


                int    count  = y * GRID_SIZE_X + x;
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
                if (height < 205)
                    current_texture = 2;
                if (height < 150)
                    current_texture = 1;
                if (height < 75)
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
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // restore
}

void GridMap::ResetPerlinTexture()
{
    int seedX          = Dokev::GetRandomValue(1, 10000);
    int seedY          = Dokev::GetRandomValue(1, 10000);
    PerlinNoiseImage   = Dokev::GenImagePerlinNoise(GRID_SIZE_X, GRID_SIZE_Y, seedX, seedY, 0.1f);
    PerlinNoiseTexture = LoadTextureFromImage(PerlinNoiseImage);

    ExportImage(PerlinNoiseImage, "PerlinNoise.png");
}

const Dokev::Texture2D* GridMap::GetPerlinTexture() const
{
    return &PerlinNoiseTexture;
}

GridCell** GridMap::GetViewArray(int viewIndex) const
{
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
