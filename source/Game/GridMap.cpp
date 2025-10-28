/**
 * \file GridMap.cpp
 * \brief
 * \author Junseok Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */


#include "GridMap.hpp"
#include "Dokev/DKColor.hpp"
#include "Dokev/DKGrid.hpp"
#include "Dokev/DKUtility.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Logger.hpp"
#include "Engine/Window.hpp"
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

GridMap::GridMap() : currentView(1), pathFinder(nullptr), showPath(false)
{
    // resize the grid because the grid might change in later itereations of the code. current there is no need for this
    PlainsTexture     = Dokev::LoadTexture("Assets/images/textures/IsoPlains.png");
    WaterTexture      = Dokev::LoadTexture("Assets/images/textures/IsoWater.png");
    MountainTexture   = Dokev::LoadTexture("Assets/images/textures/IsoMountains.png");
    SnowTexture       = Dokev::LoadTexture("Assets/images/textures/IsoPeak.png");
    Max_Height        = Dokev::LoadTexture("Assets/images/textures/Max_Height.png"); // Use IsoPeak as placeholder for Max_Height
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


    InitializeIsoView();

    // InitializePathFinding();
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
            // 원본 뷰
            view1[y * GRID_SIZE_X + x] = &m_gridCells[y][x];

            // 90도 회전
            rotatePoint90(x, y, GRID_SIZE_X_HALF, GRID_SIZE_Y_HALF, 90, new_x, new_y);
            int rotIndex90 = static_cast<int>(new_y) * GRID_SIZE_X + static_cast<int>(new_x);
            if (rotIndex90 >= 0 && rotIndex90 < GRID_SIZE_X * GRID_SIZE_Y)
                view2[rotIndex90] = &m_gridCells[y][x];

            // 180도 회전
            rotatePoint90(x, y, GRID_SIZE_X_HALF, GRID_SIZE_Y_HALF, 180, new_x, new_y);
            int rotIndex180 = static_cast<int>(new_y) * GRID_SIZE_X + static_cast<int>(new_x);
            if (rotIndex180 >= 0 && rotIndex180 < GRID_SIZE_X * GRID_SIZE_Y)
                view3[rotIndex180] = &m_gridCells[y][x];

            // 270도 회전
            rotatePoint90(x, y, GRID_SIZE_X_HALF, GRID_SIZE_Y_HALF, 270, new_x, new_y);
            int rotIndex270 = static_cast<int>(new_y) * GRID_SIZE_X + static_cast<int>(new_x);
            if (rotIndex270 >= 0 && rotIndex270 < GRID_SIZE_X * GRID_SIZE_Y)
                view4[rotIndex270] = &m_gridCells[y][x];
        }
    }
}

void GridMap::IsometricDrawMap(int index) const
{
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

    int originX = ((TILE_WIDTH / 2) * GRID_SIZE_X) - TILE_WIDTH / 2;
    int originY = 0;

    for (int sum = 0; sum < GRID_SIZE_X + GRID_SIZE_Y - 1; ++sum)
    {
        for (int x = 0; x <= sum; ++x)
        {
            int y = sum - x;

            if (x >= 0 && x < GRID_SIZE_X && y >= 0 && y < GRID_SIZE_Y)
            {
                int    screenX = originX + (x - y) * TILE_WIDTH_HALF;
                int    screenY = originY + (x + y) * TILE_HEIGHT_HALF;
                int    count   = y * GRID_SIZE_X + x;
                double height  = selectedView[count]->getHeight();
                int    topY    = static_cast<int>(height) - screenY;

                int current_texture = 3;
                if (height < 205)
                    current_texture = 2;
                if (height < 150)
                    current_texture = 1;
                if (height < 75)
                    current_texture = 0;

                
                Dokev::DrawTexture(IsoGridTexture[current_texture], screenX, topY, Dokev::ColorPalette::WHITE);
                Dokev::DrawTexture(Max_Height, screenX, topY, Dokev::ColorPalette::WHITE);
            }
        }
    }
    // DrawCurrentPathIsometric(Dokev::ColorPalette::RED, currentView);
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

GridCell** GridMap::GetViewArray(int viewIndex)
{
    switch (viewIndex)
    {
        case 1: return view1;
        case 2: return view2;
        case 3: return view3;
        case 4: return view4;
        default: return view1;
    }
}

void GridMap::rotatePoint90(int x, int y, float tx, float ty, int angleDeg, float& oX, float& oY)
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
        delete pathFinder;
        pathFinder = nullptr;
        std::cout << "clear the pathfind" << std::endl;
    }

    pathFinder = new PathFinding(GRID_SIZE_X, GRID_SIZE_Y, 50.0f);

    std::vector<double> heights;
    std::vector<float>  costs;

    heights.reserve(IsoGridMapArray.size());
    costs.reserve(IsoGridMapArray.size());

    for (const auto& cell : IsoGridMapArray)
    {
        heights.push_back(cell.getHeight());
        costs.push_back(cell.getMovementCost());
    }

    pathFinder->SetHeightMap(heights);
    pathFinder->SetCostMap(costs);

    int walkableCount = 0;
    for (int y = 0; y < GRID_SIZE_Y; y++)
    {
        for (int x = 0; x < GRID_SIZE_X; x++)
        {
            const auto& currentCell = IsoGridMapArray[y * GRID_SIZE_X + x];
            bool        walkable    = currentCell.isWalkable();
            pathFinder->SetWalkable(x, y, walkable);
            if (walkable)
            {
                walkableCount++;
            }
        }
    }
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
    std::cout << "Path cleared" << std::endl;
}

void GridMap::DrawCurrentPathIsometric(CS200::RGBA color, int viewIndex)
{
    if (!pathFinder || currentPath.empty() || !showPath)
    {
        return;
    }

    GridCell** selectedView = GetViewArray(viewIndex);

    int originX = ((TILE_WIDTH / 2) * GRID_SIZE_X) - TILE_WIDTH / 2;
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
        float       currentHeight = static_cast<float>(selectedView[y * GRID_SIZE_X + x]->getHeight());
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

            float nextHeight = static_cast<float>(selectedView[nextY * GRID_SIZE_X + nextX]->getHeight());

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

Math::fvec2 GridMap::TransformPathPointForView(Math::fvec2 point, int viewIndex)
{
    int x = (int)point.x;
    int y = (int)point.y;

    float newX, newY;

    switch (viewIndex)
    {
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
