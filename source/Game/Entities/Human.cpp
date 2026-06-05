/**
 * \file Human.cpp
 * \author Hyunwoo Yang
 * \author
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "Human.hpp"
#include "Building.hpp"
#include "WorkBuilding.hpp"
#include "../Managers/NPCNameManager.hpp"
#include "../Managers/ResourceManager.hpp"
#include "../World/GridMap.hpp"
#include "Dokev/Core/DKColor.hpp"
#include "Dokev/Graphics/DKShapes.hpp"
#include "Dokev/Managers/DKGameObjectManager.hpp"
#include "Game/Managers/WorkManager.hpp"
#include "../Common/BuildingType.hpp"
#include "Engine/Path.hpp"
#include "Game/Managers/DayCycleManager.hpp"
#include "Road.hpp"
#include "../Managers/FloatingTextManager.hpp"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <queue>
#include <unordered_map>


Human::Human(Math::fvec2 startGridPos) : currentGridPos(startGridPos), targetGridPos(startGridPos), currentPathIndex(0), isMoving(false), color(Dokev::ColorPalette::BLUE)
{
    startPosition  = currentGridPos;
    actualPosition = currentGridPos;
    moveTimer      = 0.0f;
    humanTexture   = Dokev::LoadTexture(assets::locate_asset("Assets/images/textures/Human.png").string());
    name           = NPCNameManager::Instance().GenerateName();
}

void Human::CancelWork()
{
    if (ai.assignedJobId != -1)
    {
        Job* job = WorkManager::GetInstance().GetJob(ai.assignedJobId);
        if (job && job->targetBuilding)
        {
            if (state == HumanState::Working)
            {
                job->targetBuilding->OnWorkerLeft(this);
                WorkManager::GetInstance().EraseJob(ai.assignedJobId); // slot consumed
            }
            else
            {
                WorkManager::GetInstance().UnassignJob(ai.assignedJobId); // return slot to pool
            }
        }
    }

    ai.assignedJobId     = -1;
    ai.assignedWorkType  = WorkType::None;
    ai.workplacePosition = { -1.0f, -1.0f };
    // isVisible = true;

    if (state == HumanState::Walking || state == HumanState::Working)
    {
        ChangeState(HumanState::Idle);
    }
}

void Human::FollowPath(const std::vector<Math::fvec2>& newPath, CommandSource source)
{
    assert(!newPath.empty() && "Path cannot be empty");
    (void)source;

    if (newPath.size() <= 1)
    {
        isMoving = false;
        path.clear();
        return;
    }

    path             = newPath;
    currentPathIndex = 0;


    currentGridPos = path[0];
    actualPosition = path[0];
    startPosition  = path[0];

    currentPathIndex = 1;
    targetGridPos    = path[currentPathIndex];
    isMoving         = true;
    moveTimer        = 0.0f;
}

void Human::UpdateMovement(float deltaTime, GridMap* map, Dokev::GameObjectManager* gameObjMgr)
{
    if (!isMoving)
        return;

    moveTimer += deltaTime;
    float t = moveTimer / moveDelay;

    if (t > 1.0f)
    {
        t = 1.0f;
    }

    actualPosition.x = startPosition.x + (targetGridPos.x - startPosition.x) * t;
    actualPosition.y = startPosition.y + (targetGridPos.y - startPosition.y) * t;

    if (t >= 1.0f) // Reached target position
    {
        // Snap position to target exactly
        actualPosition = targetGridPos;
        currentGridPos = targetGridPos;

        /// debugging mode
        if (debugMode && map)
        {
            int gridX = (int)currentGridPos.x;
            int gridY = (int)currentGridPos.y;

            if (gridX >= 0 && gridX < GRID_SIZE_X && gridY >= 0 && gridY < GRID_SIZE_Y)
            {
                GridCell** viewArray   = map->GetViewArray(0);
                GridCell* currentCell = viewArray[gridY * GRID_SIZE_X + gridX];

                std::string terrainName;
                double      height = currentCell->getHeight();
                terrainName        = GetTerrainName(height);
                ai.previousGridPos = startPosition;
            }
        }

        currentPathIndex++;

        assert(currentPathIndex <= path.size() && "Path index out of bounds");

        if (currentPathIndex >= path.size())
        {
            isMoving = false;
            path.clear();

            // Reached the edge of the world while leaving — remove from the city.
            if (isEmigrating)
            {
                Destroy();
                return;
            }

            // Arrived at Job
            if (HasWorkAssignment() &&
                static_cast<int>(currentGridPos.x) == static_cast<int>(ai.workplacePosition.x) &&
                static_cast<int>(currentGridPos.y) == static_cast<int>(ai.workplacePosition.y))
            {
                Job* job = WorkManager::GetInstance().GetJob(ai.assignedJobId);
                if (job && job->targetBuilding)
                {
                    job->targetBuilding->OnWorkerArrived(this);
                }
                ChangeState(HumanState::Working);
            }
            else if (HasWorkAssignment())
            {
                CancelWork();
            }
            else if (ai.isHeadingHome &&
                     static_cast<int>(currentGridPos.x) == static_cast<int>(ai.targetHomePosition.x) &&
                     static_cast<int>(currentGridPos.y) == static_cast<int>(ai.targetHomePosition.y))
            {
                ai.isHeadingHome = false;
                ChangeState(HumanState::Resting);
            }
            else if (ai.isSeekingCafe &&
                     static_cast<int>(currentGridPos.x) == static_cast<int>(ai.targetCafePosition.x) &&
                     static_cast<int>(currentGridPos.y) == static_cast<int>(ai.targetCafePosition.y))
            {
                ai.isSeekingCafe = false;
                bool cafeIsOperating = false;
                if (gameObjMgr)
                {
                    for (const auto& building : gameObjMgr->GetBuildings())
                    {
                        if (building->GetBuildingType() == BuildingType::CAFE && building->GetIsBuilt() &&
                            static_cast<int>(building->GetGrid().x) == static_cast<int>(ai.targetCafePosition.x) &&
                            static_cast<int>(building->GetGrid().y) == static_cast<int>(ai.targetCafePosition.y))
                        {
                            if (auto* cafe = dynamic_cast<WorkBuilding*>(building.get()))
                            {
                                cafeIsOperating = cafe->IsServiceOperating();
                            }
                            break;
                        }
                    }
                }

                if (map)
                {
                    int   gx     = static_cast<int>(actualPosition.x);
                    int   gy     = static_cast<int>(actualPosition.y);
                    GridCell** va = map->GetViewArray(0);
                    float cellH  = va ? static_cast<float>(va[gy * GRID_SIZE_X + gx]->getHeight()) : 0.0f;
                    float rawY   = static_cast<float>(map->GetIsoOriginY() + (actualPosition.x + actualPosition.y) * TILE_HEIGHT_HALF);
                    float sx     = static_cast<float>(map->GetIsoOriginX() + (actualPosition.x - actualPosition.y) * TILE_WIDTH_HALF);
                    float sy     = cellH - rawY + TILE_HEIGHT_HALF * 3;
                    if (cafeIsOperating)
                    {
                        FloatingTextManager::Instance().Add("+10 Happiness (Cafe)", { sx, sy }, 0xFFDD44FF);
                    }
                    else
                    {
                        FloatingTextManager::Instance().Add("Cafe closed: need food", { sx, sy }, 0xFF8080FF);
                    }
                }
                if (cafeIsOperating)
                {
                    stats.energy    = std::min(100.0f, stats.energy + 50.0f);
                    stats.happiness = std::min(100.0f, stats.happiness + 10.0f);
                }
                ChangeState(HumanState::Idle);
            }
            else
            {
                ChangeState(HumanState::Idle);
            }

            if (debugMode)
            {
            }
        }
        else // Still more to go
        {
            startPosition = currentGridPos;
            targetGridPos = path[currentPathIndex];
            moveTimer     = 0.0f;

            if (debugMode)
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

void Human::UpdateWorking(float deltaTime, int currentHour)
{
    const float gameHoursPerSecond = 24.0f / DayCycleManager::kDayDuration;

    stats.energy    = std::max(0.0f, stats.energy    - 10.0f * deltaTime * gameHoursPerSecond);
    stats.happiness = std::max(0.0f, stats.happiness -  5.0f * deltaTime * gameHoursPerSecond);

    float multiplier = (stats.energy / 100.0f) * (stats.happiness / 100.0f);
    float earned = stats.hourlyWage * multiplier * deltaTime * gameHoursPerSecond;
    stats.money   += earned;
    shiftEarnings += earned;

    // Knock off at 18:00 — collect tax then go home
    if (currentHour >= 18)
    {
        float tax = shiftEarnings * stats.taxRate;
        if (resourceMgr && tax > 0.0f)
            resourceMgr->AddMoney(static_cast<int>(tax));
        stats.money  -= tax;
        shiftEarnings = 0.0f;

        CancelWork();
        ai.isHeadingHome = true;
        return;
    }

}

void Human::UpdateResting(float deltaTime)
{
    ai.restTimer += deltaTime;
    if (ai.restTimer >= ai.maxRestTime)
    {
        ai.restTimer = 0.0f;
        stats.energy = std::min(100.0f, stats.energy + 50.0f);
        if (ai.assignedHome)
        {
            ai.assignedHome->OnWorkerLeft(this);
            ai.assignedHome     = nullptr;
            ai.assignedWorkType = WorkType::None;
        }
        ChangeState(HumanState::Idle);
    }
}

void Human::UpdateIdle(float deltaTime, GridMap* map, Dokev::GameObjectManager* gameObjMgr)
{
    if (ai.isHomeless)
    {
        const float gameHoursPerSecond = 24.0f / DayCycleManager::kDayDuration;
        stats.happiness = std::max(0.0f, stats.happiness - 4.0f * deltaTime * gameHoursPerSecond);

        homelessTextTimer += deltaTime;
        if (homelessTextTimer >= 10.0f)
        {
            homelessTextTimer = 0.0f;
            SpawnFloatingStatus("Homeless", 0xFF5555FF, map);
        }

        if (gameObjMgr)
        {
            for (const auto& b : gameObjMgr->GetBuildings())
            {
                if (b->GetBuildingType() == BuildingType::HOME && b->GetIsBuilt() && !b->IsFull())
                {
                    ai.isHomeless    = false;
                    ai.isHeadingHome = true;
                    break;
                }
            }
        }

        if (ai.isHomeless)
            return;
    }

    stats.happiness = std::min(100.0f, stats.happiness + 1.0f * deltaTime);

    ai.idleTimer += deltaTime;

    if (ai.idleTimer >= ai.maxIdleTime)
    {
        ai.idleTimer = 0.0f;

        if (ai.isSeekingCafe)
        {
            Math::fvec2 cafePos = { -1.0f, -1.0f };
            if (gameObjMgr)
            {
                for (const auto& building : gameObjMgr->GetBuildings())
                {
                    auto* cafe = dynamic_cast<WorkBuilding*>(building.get());
                    if (building->GetBuildingType() == BuildingType::CAFE && building->GetIsBuilt() && cafe && cafe->IsServiceOperating())
                    {
                        cafePos = building->GetGrid();
                        break;
                    }
                }
            }

            if (cafePos.x >= 0.0f && map)
            {
                map->ClearPath();
                map->FindPath(static_cast<int>(currentGridPos.x), static_cast<int>(currentGridPos.y),
                              static_cast<int>(cafePos.x), static_cast<int>(cafePos.y));
                auto path_ = map->GetCurrentPath();

                if (!path_.empty() && path_.size() > 1)
                {
                    ai.targetCafePosition = cafePos;
                    FollowPath(path_);
                    ChangeState(HumanState::Walking);
                }
                else
                {
                    ai.isSeekingCafe = false;
                }
            }
            else
            {
                ai.isSeekingCafe = false;
            }
            return;
        }

        if (ai.isHeadingHome)
        {
            Math::fvec2 homePos    = { -1.0f, -1.0f };
            Building*   foundHome  = nullptr;
            if (gameObjMgr)
            {
                for (const auto& building : gameObjMgr->GetBuildings())
                {
                    if (building->GetBuildingType() == BuildingType::HOME && building->GetIsBuilt() && !building->IsFull())
                    {
                        homePos   = building->GetGrid();
                        foundHome = building.get();
                        break;
                    }
                }
            }

            if (!foundHome)
            {
                ai.isHeadingHome = false;
                return;
            }

            ai.assignedHome     = foundHome;
            ai.assignedWorkType = WorkType::REST;
            foundHome->OnWorkerArrived(this);

            if (map)
            {
                map->ClearPath();
                map->FindPath(static_cast<int>(currentGridPos.x), static_cast<int>(currentGridPos.y),
                              static_cast<int>(homePos.x), static_cast<int>(homePos.y));
                auto path_ = map->GetCurrentPath();

                if (!path_.empty() && path_.size() > 1)
                {
                    ai.targetHomePosition = homePos;
                    FollowPath(path_);
                    ChangeState(HumanState::Walking);
                }
                else
                {
                    ai.isHeadingHome = false;
                    ChangeState(HumanState::Resting);
                }
            }
            return;
        }

        ChangeState(HumanState::FindingDestination);
    }
}

void Human::UpdateFindingDestination(float deltaTime, GridMap* map, const Road* road, int currentHour, Dokev::GameObjectManager* gameObjMgr)
{
    (void)deltaTime;

    if (!map)
        return;

    bool outsideHours = (currentHour < 9 || currentHour >= 18);

    if (outsideHours)
    {
        bool availableHomeExists = false;
        if (gameObjMgr)
        {
            for (const auto& b : gameObjMgr->GetBuildings())
            {
                if (b->GetBuildingType() == BuildingType::HOME && b->GetIsBuilt() && !b->IsFull())
                {
                    availableHomeExists = true;
                    break;
                }
            }
        }

        if (availableHomeExists)
        {
            ai.isHomeless = false;
            bool onRoad = road && road->hasRoad(static_cast<int>(currentGridPos.x), static_cast<int>(currentGridPos.y));
            if (onRoad)
            {
                ai.isHeadingHome = true;
                ChangeState(HumanState::Idle);
                return;
            }
            ai.isHeadingHome = true;
        }
        else if (currentHour >= 18)
        {
            ai.isHomeless    = true;
            ai.isHeadingHome = false;
            ChangeState(HumanState::Idle);
            return;
        }
        else
        {
            ai.isHeadingHome = false;
            ChangeState(HumanState::Idle);
            return;
        }
    }
    else if (stats.energy <= 0.0f)
    {
        ai.isSeekingCafe = true;
        ChangeState(HumanState::Idle);
        return;
    }
    else
    {
    // Manager Gives Job
    int newJobId = -1;
    Math::fvec2 newJobPos = { -1.0f, -1.0f };
    WorkType newJobType = WorkType::None;

    if (!HasWorkAssignment() && WorkManager::GetInstance().RequestJob(this, currentGridPos, newJobId, newJobPos, newJobType, ai.explorationRadius))
    {
        ai.assignedJobId = newJobId;
        ai.workplacePosition = newJobPos;
        ai.assignedWorkType = newJobType;

        map->ClearPath();
        map->FindPath(static_cast<int>(currentGridPos.x), static_cast<int>(currentGridPos.y), 
                      static_cast<int>(newJobPos.x), static_cast<int>(newJobPos.y));

        auto path_ = map->GetCurrentPath();

        if (!path_.empty() && path_.size() > 1)
        {
            FollowPath(path_);
            ChangeState(HumanState::Walking);
            return;
        }
        else
        {
            CancelWork();
        }
    }
    } // end else (working hours)

    // road movement — runs both during and outside working hours
    if (road)
    {
        bool onRoad = road->hasRoad(static_cast<int>(currentGridPos.x), static_cast<int>(currentGridPos.y));

        if (onRoad)
        {
            bool        foundRoad    = false;
            Math::fvec2 nextRoadTile = FindNextRoadTile(road, foundRoad);

            if (foundRoad)
            {
                std::vector<Math::fvec2> simplePath;
                simplePath.push_back(currentGridPos);
                simplePath.push_back(nextRoadTile);
                FollowPath(simplePath);
                ChangeState(HumanState::Walking);
                return;
            }
        }
        else
        {
            // Not on road: pathfind to the nearest road tile
            Math::fvec2 roadPos;
            if (FindNearbyRoad(road, roadPos))
            {
                map->ClearPath();
                map->FindPath(static_cast<int>(currentGridPos.x), static_cast<int>(currentGridPos.y),
                              static_cast<int>(roadPos.x), static_cast<int>(roadPos.y));
                auto path_ = map->GetCurrentPath();
                if (!path_.empty() && path_.size() > 1)
                {
                    FollowPath(path_);
                    ChangeState(HumanState::Walking);
                    return;
                }
            }
        }
    }

    // No road reachable — stay idle and retry shortly
    ChangeState(HumanState::Idle);
    ai.idleTimer = ai.maxIdleTime - 0.5f;
}

void Human::Update(float deltaTime, GridMap* map, const Road* road, Dokev::GameObjectManager* gameObjMgr, int currentHour)
{
    // Emigration: citizens who are completely unhappy, or who remained homeless
    // through the night until daytime, pack up and leave the city.
    if (!isEmigrating)
    {
        const bool daytime = (currentHour >= 9 && currentHour < 18);
        if (stats.happiness <= 0.0f || (ai.isHomeless && daytime))
        {
            StartEmigrating();
        }
    }

    if (state == HumanState::Walking)
    {
        if (!isMoving)
        {
            if (ai.isHeadingHome)
            {
                ai.isHeadingHome = false;
                ChangeState(HumanState::Resting);
            }
            else
            {
                ChangeState(HumanState::Idle);
            }
            return;
        }

        if (isMoving)
        {
            UpdateMovement(deltaTime, map, gameObjMgr);

            if (!isMoving)
            {
                if (state == HumanState::Working || state == HumanState::Resting)
                    return;

                ChangeState(HumanState::FindingDestination);
            }
        }
    }
    else if (state == HumanState::Idle)
    {
        UpdateIdle(deltaTime, map, gameObjMgr);
    }
    else if (state == HumanState::FindingDestination)
    {
        UpdateFindingDestination(deltaTime, map, road, currentHour, gameObjMgr);
    }
    else if (state == HumanState::Working)
    {
        UpdateWorking(deltaTime, currentHour);
    }
    else if (state == HumanState::Resting)
    {
        UpdateResting(deltaTime);
    }
    else if (state == HumanState::Firefighting)
    {
        if (isMoving)
            UpdateMovement(deltaTime, map);
        else
            UpdateFirefighting();
    }
}

void Human::DispatchToFire(Math::fvec2 fireCenter, GridMap* map)
{
    (void)map;
    if (HasWorkAssignment())
        CancelWork();

    ai.isHeadingHome = false;
    ai.isSeekingCafe = false;

    fireTargetPos    = fireCenter;
    firefightingDone = false;

    ChangeState(HumanState::Firefighting);
    // Stay at fire station — no movement
}

void Human::CancelFirefighting()
{
    firefightingDone = true;
}

bool Human::IsAtFireLocation() const
{
    return state == HumanState::Firefighting && !isMoving;
}

void Human::UpdateFirefighting()
{
    if (firefightingDone)
    {
        firefightingDone = false;
        fireTargetPos    = { 0.0f, 0.0f };
        ChangeState(HumanState::FindingDestination);
    }
}

void Human::StartEmigrating()
{
    if (isEmigrating)
        return;

    // Release any city ties so jobs/home slots are freed for remaining citizens.
    if (HasWorkAssignment())
        CancelWork();
    if (ai.assignedHome)
    {
        ai.assignedHome->OnWorkerLeft(this);
        ai.assignedHome     = nullptr;
        ai.assignedWorkType = WorkType::None;
    }

    ai.isHeadingHome = false;
    ai.isSeekingCafe = false;
    ai.isHomeless    = false;
    isEmigrating     = true;

    // Walk far off the map; UpdateMovement destroys us on arrival.
    std::vector<Math::fvec2> exitPath;
    exitPath.push_back(currentGridPos);
    exitPath.push_back({ 2000.0f, 2000.0f });
    FollowPath(exitPath);
    ChangeState(HumanState::Walking);
}

void Human::SpawnFloatingStatus(const std::string& text, CS200::RGBA color, GridMap* map) const
{
    if (!map)
        return;

    int gx = std::clamp(static_cast<int>(actualPosition.x), 0, GRID_SIZE_X - 1);
    int gy = std::clamp(static_cast<int>(actualPosition.y), 0, GRID_SIZE_Y - 1);

    GridCell** va    = map->GetViewArray(0);
    float      cellH = va ? static_cast<float>(va[gy * GRID_SIZE_X + gx]->getHeight()) : 0.0f;
    float      rawY  = static_cast<float>(map->GetIsoOriginY() + (actualPosition.x + actualPosition.y) * TILE_HEIGHT_HALF);
    float      sx    = static_cast<float>(map->GetIsoOriginX() + (actualPosition.x - actualPosition.y) * TILE_WIDTH_HALF);
    float      sy    = cellH - rawY + TILE_HEIGHT_HALF * 3;

    FloatingTextManager::Instance().Add(text, { sx, sy }, color);
}

void Human::Draw(int viewIndex, const GridMap* map) const
{
    if (state == HumanState::Working)
    {
        return;
    }
    
    Math::fvec2 transformedPos = map->TransformPathPointForView(actualPosition, viewIndex);

    float gridX_float = transformedPos.x;
    float gridY_float = transformedPos.y;

    int gridX_int = std::clamp(static_cast<int>(gridX_float), 0, GRID_SIZE_X - 1);
    int gridY_int = std::clamp(static_cast<int>(gridY_float), 0, GRID_SIZE_Y - 1);

    // Get the view array
    GridCell** viewArray = map->GetViewArray(viewIndex);

    float height = static_cast<float>(viewArray[gridY_int * GRID_SIZE_X + gridX_int]->getHeight());

    int originX = map->GetIsoOriginX();
    int originY = map->GetIsoOriginY();

    int screenX = originX + static_cast<int>((gridX_float - gridY_float) * TILE_WIDTH_HALF);
    int screenY = originY + static_cast<int>((gridX_float + gridY_float) * TILE_HEIGHT_HALF);
    int topY    = static_cast<int>(height) - screenY;

    int drawX = screenX + TILE_WIDTH_HALF - humanTexture.GetWidth() / 2;
    int drawY = topY + TILE_HEIGHT_HALF;

    if (state == HumanState::Firefighting)
    {
        humanTexture.Draw(drawX, drawY, CS200::WHITE, Dokev::RenderState::IMMEDIATE);
        int cx = drawX + humanTexture.GetWidth() / 2;
        int cy = drawY - 6;
        Dokev::DrawCircle(cx, cy, 5.0f, 0x0080FFFF);
    }
    else
    {
        humanTexture.Draw(drawX, drawY);
    }
}

void Human::ChangeState(HumanState newState)
{
    state = newState;

    if (state == HumanState::Working)
    {
    }
    else if (state == HumanState::Idle)
    {
        ai.idleTimer = 0.0f;
    }
}

std::string Human::GetTerrainName(double height) const
{
    if (height < 75.0)
    {
        return "Water";
    }
    else if (height < 150.0)
        return "Plains";
    else if (height < 205.0)
        return "Mountains";
    else
        return "Peak";
}

void Human::DecideNextAction(GridMap* map)
{
    (void)map;
    if (state == HumanState::Idle)
    {
        ChangeState(HumanState::FindingDestination);
    }
}


Math::fvec2 Human::FindNextRoadTile(const Road* road, bool& outFoundRoad) const
{
    outFoundRoad = false;
    if (!road)
        return currentGridPos;

    int currentX = static_cast<int>(currentGridPos.x);
    int currentY = static_cast<int>(currentGridPos.y);


    int prevX = static_cast<int>(startPosition.x);
    int prevY = static_cast<int>(startPosition.y);

    int directions[4][2] = {
        {  1,  0 }, // Right
        { -1,  0 }, // Left
        {  0,  1 }, // Down
        {  0, -1 }  // Up
    };

    std::vector<Math::fvec2> candidates;
    for (auto& dir : directions)
    {
        int nx = currentX + dir[0];
        int ny = currentY + dir[1];

        if (nx < 0 || nx >= GRID_SIZE_X || ny < 0 || ny >= GRID_SIZE_Y)
            continue;

        if (road->hasRoad(nx, ny))
        {
            candidates.push_back(Math::fvec2(static_cast<float>(nx), static_cast<float>(ny)));
        }
    }

    // single road
    if (candidates.empty())
    {
        outFoundRoad = false;
        return currentGridPos;
    }

    // Remove previous road from candidates
    std::vector<Math::fvec2> nextMoveOptions;
    if (candidates.size() > 1) // More than one road
    {
        for (const auto& tile : candidates)
        {
            if (static_cast<int>(tile.x) != prevX || static_cast<int>(tile.y) != prevY)
            {
                nextMoveOptions.push_back(tile);
            }
        }
    }
    else
    {
        // Only one candidate
        nextMoveOptions = candidates;
    }

    // Decide the next move
    if (nextMoveOptions.empty())
    {
        outFoundRoad = true;
        return candidates[0];
    }
    else if (nextMoveOptions.size() == 1)
    {
        // Straight line or corner
        outFoundRoad = true;
        return nextMoveOptions[0];
    }
    else
    {
        // Intersection - choose randomly
        outFoundRoad    = true;
        int randomIndex = rand() % nextMoveOptions.size();
        return nextMoveOptions[randomIndex];
    }
}

bool Human::FindNearbyRoad(const Road* road, Math::fvec2& outRoadPosition) const
{
    if (!road)
        return false;

    int startX = static_cast<int>(currentGridPos.x);
    int startY = static_cast<int>(currentGridPos.y);

    if (road->hasRoad(startX, startY))
    {
        outRoadPosition = Math::fvec2(static_cast<float>(startX), static_cast<float>(startY));
        return true;
    }

    std::queue<std::pair<int, int>> q;
    std::unordered_map<int, bool>   visited;

    auto key = [&](int x, int y)
    {
        return y * GRID_SIZE_X + x;
    };

    q.push({ startX, startY });
    visited[key(startX, startY)] = true;

    int directions[8][2] = {
        {  1,  0 }, // Right
        { -1,  0 }, // Left
        {  0,  1 }, // Down
        {  0, -1 }, // Up
        {  1,  1 }, // Right-Down
        {  1, -1 }, // Right-Up
        { -1,  1 }, // Left-Down
        { -1, -1 }  // Left-Up
    };

    int searchLimit = 1000;
    int searchCount = 0;

    while (!q.empty() && searchCount < searchLimit)
    {
        auto [cx, cy] = q.front();
        q.pop();
        searchCount++;

        for (auto& dir : directions)
        {
            int nx = cx + dir[0];
            int ny = cy + dir[1];

            if (nx < 0 || nx >= GRID_SIZE_X || ny < 0 || ny >= GRID_SIZE_Y)
                continue;

            // Check if already visited
            if (visited[key(nx, ny)])
                continue;

            visited[key(nx, ny)] = true;

            // Check if road is found
            if (road->hasRoad(nx, ny))
            {
                outRoadPosition = Math::fvec2(static_cast<float>(nx), static_cast<float>(ny));
                return true;
            }

            q.push({ nx, ny });
        }
    }

    return false;
}

Human Human::CreateImmigrant(const Road* road, const GridMap* map)
{
    if (road && !road->GetRoadPositions().empty())
    {
        const auto& positions = road->GetRoadPositions();
        int         idx       = rand() % static_cast<int>(positions.size());
        return Human(positions[idx]);
    }

    // Fallback: spiral search from map centre for a walkable (non-water) tile
    if (map)
    {
        GridCell** view = map->GetViewArray(0);
        const int  cx   = GRID_SIZE_X / 2;
        const int  cy   = GRID_SIZE_Y / 2;

        for (int r = 0; r < GRID_SIZE_X / 2; ++r)
        {
            for (int dx = -r; dx <= r; ++dx)
            {
                for (int dy = -r; dy <= r; ++dy)
                {
                    if (std::abs(dx) != r && std::abs(dy) != r)
                        continue; // only check the ring edge
                    int nx = cx + dx, ny = cy + dy;
                    if (nx < 0 || nx >= GRID_SIZE_X || ny < 0 || ny >= GRID_SIZE_Y)
                        continue;
                    if (view[ny * GRID_SIZE_X + nx]->getHeight() >= 75.0)
                        return Human(Math::fvec2(static_cast<float>(nx), static_cast<float>(ny)));
                }
            }
        }
    }

    return Human(Math::fvec2(GRID_SIZE_X / 2.0f, GRID_SIZE_Y / 2.0f));
}

Math::fvec2 Human::FindClosestWalkableTile(GridMap* map) const
{
    if (!map)
    {
        return Math::fvec2(GRID_SIZE_X / 2.0f, GRID_SIZE_Y / 2.0f);
    }

    float       closestDist = 999999.0f;
    Math::fvec2 closestTile(GRID_SIZE_X / 2.0f, GRID_SIZE_Y / 2.0f);

    GridCell** viewArray = map->GetViewArray(0);

    for (int x = 0; x < GRID_SIZE_X; x++)
    {
        for (int y = 0; y < 5 && y < GRID_SIZE_Y; y++)
        {
            GridCell* cell = viewArray[y * GRID_SIZE_X + x];
            if (cell->getHeight() >= 75.0)
            {
                float dx   = x - currentGridPos.x;
                float dy   = y - currentGridPos.y;
                float dist = dx * dx + dy * dy;

                if (dist < closestDist)
                {
                    closestDist = dist;
                    closestTile = Math::fvec2((float)x, (float)y);
                }
            }
        }

        // Bottom edge
        for (int y = GRID_SIZE_Y - 5; y < GRID_SIZE_Y; y++)
        {
            if (y < 0)
                continue;
            GridCell* cell = viewArray[y * GRID_SIZE_X + x];
            if (cell->getHeight() >= 75.0)
            {
                float dx   = x - currentGridPos.x;
                float dy   = y - currentGridPos.y;
                float dist = dx * dx + dy * dy;

                if (dist < closestDist)
                {
                    closestDist = dist;
                    closestTile = Math::fvec2((float)x, (float)y);
                }
            }
        }
    }

    // Left and right edges
    for (int y = 0; y < GRID_SIZE_Y; y++)
    {
        // Left edge
        for (int x = 0; x < 5 && x < GRID_SIZE_X; x++)
        {
            GridCell* cell = viewArray[y * GRID_SIZE_X + x];
            if (cell->getHeight() >= 75.0)
            {
                float dx   = x - currentGridPos.x;
                float dy   = y - currentGridPos.y;
                float dist = dx * dx + dy * dy;

                if (dist < closestDist)
                {
                    closestDist = dist;
                    closestTile = Math::fvec2((float)x, (float)y);
                }
            }
        }

        // Right edge
        for (int x = GRID_SIZE_X - 5; x < GRID_SIZE_X; x++)
        {
            if (x < 0)
                continue;
            GridCell* cell = viewArray[y * GRID_SIZE_X + x];
            if (cell->getHeight() >= 75.0)
            {
                float dx   = x - currentGridPos.x;
                float dy   = y - currentGridPos.y;
                float dist = dx * dx + dy * dy;

                if (dist < closestDist)
                {
                    closestDist = dist;
                    closestTile = Math::fvec2((float)x, (float)y);
                }
            }
        }
    }

    return closestTile;
}
