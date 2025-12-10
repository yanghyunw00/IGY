/**
 * \file Human.cpp
 * \author Hyunwoo Yang
 * \author
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "Human.hpp"
#include "Dokev/DKColor.hpp"
#include "Engine/Path.hpp"
#include "GridMap.hpp"
#include "Road.hpp"
#include <iostream>
#include <queue>
#include <unordered_map>

Human::Human(Math::fvec2 startGridPos) : currentGridPos(startGridPos), targetGridPos(startGridPos), currentPathIndex(0), isMoving(false), color(Dokev::ColorPalette::BLUE)
{
    startPosition  = currentGridPos;
    actualPosition = currentGridPos;
    moveTimer      = 0.0f;
    humanTexture   = Dokev::LoadTexture(assets::locate_asset("Assets/images/textures/Human.png").string());
}

void Human::FollowPath(const std::vector<Math::fvec2>& newPath, CommandSource source)
{
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

void Human::UpdateMovement(float deltaTime, GridMap* map)
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
                GridCell*  currentCell = viewArray[gridY * GRID_SIZE_X + gridX];

                std::string terrainName;
                double      height = currentCell->getHeight();
                terrainName        = GetTerrainName(height);
                ai.previousGridPos = startPosition;
            }
        }

        currentPathIndex++;

        // Check if reached the end
        if (currentPathIndex >= path.size())
        {
            isMoving = false;
            path.clear();
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

void Human::UpdateIdle(float deltaTime, GridMap* map)
{
    (void)map;
    ai.idleTimer += deltaTime;

    if (ai.idleTimer >= ai.maxIdleTime)
    {
        ai.idleTimer = 0.0f;
        ChangeState(HumanState::FindingDestination);
    }
}

void Human::UpdateFindingDestination(float deltaTime, GridMap* map, const Road* road)
{
    (void)deltaTime;

    if (!map)
        return;

    if (ai.isFollowingRoad && road)
    {
        bool onRoad = road->hasRoad(static_cast<int>(currentGridPos.x),
                                    static_cast<int>(currentGridPos.y));

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
    }

    Math::fvec2 destination = FindRandomDestination(map, road);

    map->ClearPath();
    map->FindPath(static_cast<int>(currentGridPos.x), static_cast<int>(currentGridPos.y), static_cast<int>(destination.x), static_cast<int>(destination.y));

    auto path_ = map->GetCurrentPath();

    if (!path_.empty() && path_.size() > 1)
    {
        FollowPath(path_);
        ChangeState(HumanState::Walking);
    }
    else
    {
        ChangeState(HumanState::Idle);
        ai.idleTimer = ai.maxIdleTime - 0.5f;
    }
}

void Human::Update(float deltaTime, GridMap* map, const Road* road)
{
    if (state == HumanState::Walking)
    {
        if (!isMoving)
        {
            ai.isFollowingRoad = false;
            ChangeState(HumanState::Idle);
            return;
        }

        if (ai.isFollowingRoad && road)
        {
            bool onRoad = road->hasRoad(static_cast<int>(currentGridPos.x),
                                               static_cast<int>(currentGridPos.y));

            if (!onRoad)
            {
                isMoving = false;
                path.clear();
                ChangeState(HumanState::FindingDestination);
                return;
            }

        }

        if (isMoving)
        {
            UpdateMovement(deltaTime, map);

            if (!isMoving)
            {
                bool onRoad = road && road->hasRoad(static_cast<int>(currentGridPos.x),
                                                    static_cast<int>(currentGridPos.y));

                if (onRoad)
                {
                    ai.isFollowingRoad = true;
                    ai.roadReturnAttempts = 0;
                    ChangeState(HumanState::FindingDestination);
                }
                else
                {
                    if (ai.isFollowingRoad)
                    {
                        ChangeState(HumanState::FindingDestination);
                    }
                    else
                    {
                        ai.isFollowingRoad = false;
                        ai.roadReturnAttempts = 0;
                        ChangeState(HumanState::Idle);
                    }
                }
            }
        }
    }
    else if (state == HumanState::Idle)
    {
        UpdateIdle(deltaTime, map);
    }
    else if (state == HumanState::FindingDestination)
    {
        UpdateFindingDestination(deltaTime, map, road);
    }
}

void Human::Draw(int viewIndex, const GridMap* map) const
{
    Math::fvec2 transformedPos = map->TransformPathPointForView(actualPosition, viewIndex);

    float gridX_float = transformedPos.x;
    float gridY_float = transformedPos.y;

    int gridX_int = static_cast<int>(gridX_float);
    int gridY_int = static_cast<int>(gridY_float);

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

    // For Debugging stuffs
   // Dokev::DrawCircle(drawX, drawY, 20, Dokev::ColorPalette::RED);
   // Dokev::DrawCircleLines(drawX, drawY, 20, Dokev::ColorPalette::YELLOW);
    humanTexture.Draw(drawX, drawY);
}

void Human::ChangeState(HumanState newState)
{
    state = newState;
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

Math::fvec2 Human::FindRandomDestination(GridMap* map, const Road* road)
{
    if (!map)
    {
        return currentGridPos;
    }

    if (road)
    {
        if (ai.isFollowingRoad)
        {
            bool onRoad = road->hasRoad(static_cast<int>(currentGridPos.x),
                                       static_cast<int>(currentGridPos.y));

            if (onRoad)
            {
                bool        foundRoad    = false;
                Math::fvec2 nextRoadTile = FindNextRoadTile(road, foundRoad);

                if (foundRoad)
                {
                    ai.lastDirection.x = nextRoadTile.x - currentGridPos.x;
                    ai.lastDirection.y = nextRoadTile.y - currentGridPos.y;
                    ai.roadReturnAttempts = 0;  
                    return nextRoadTile;
                }
            }

            Math::fvec2 roadPos;
            if (FindNearbyRoad(road, roadPos))
            {
                std::cout << "Returning to road - attempt " << ai.roadReturnAttempts + 1 << std::endl;
                ai.recentDestinations.push_back(roadPos);
                if (ai.recentDestinations.size() > ai.maxRecentDestinations)
                {
                    ai.recentDestinations.erase(ai.recentDestinations.begin());
                }
                return roadPos;
            }
            else
            {
                ai.roadReturnAttempts++;
                if (ai.roadReturnAttempts >= ai.maxRoadReturnAttempts)
                {
                    ai.isFollowingRoad = false;
                    ai.roadReturnAttempts = 0;
                }
            }
        }

        if (!ai.isFollowingRoad)
        {
            Math::fvec2 roadPos;
            if (FindNearbyRoad(road, roadPos))
            {
                ai.recentDestinations.push_back(roadPos);
                if (ai.recentDestinations.size() > ai.maxRecentDestinations)
                {
                    ai.recentDestinations.erase(ai.recentDestinations.begin());
                }
                return roadPos;
            }
        }
    }

    float maxDistance = ai.explorationRadius;
    float minDistance = maxDistance * 0.5f;

    int       attempts    = 0;
    const int maxAttempts = 10;

    while (attempts < maxAttempts)
    {
        float angle    = (rand() % 360) * (3.14159f / 180.0f);
        float distance = minDistance + (rand() % (int)(maxDistance - minDistance + 1));

        Math::fvec2 destination;
        destination.x = currentGridPos.x + cos(angle) * distance;
        destination.y = currentGridPos.y + sin(angle) * distance;

        destination.x = std::max(0.0f, std::min((float)(GRID_SIZE_X - 1), destination.x));
        destination.y = std::max(0.0f, std::min((float)(GRID_SIZE_Y - 1), destination.y));

        int gridX = (int)destination.x;
        int gridY = (int)destination.y;

        if (gridX >= 0 && gridX < GRID_SIZE_X && gridY >= 0 && gridY < GRID_SIZE_Y)
        {
            GridCell** viewArray = map->GetViewArray(0);
            GridCell*  cell      = viewArray[gridY * GRID_SIZE_X + gridX];
            double     height    = cell->getHeight();

            if (height < 75.0) // if water, try again
            {
                attempts++;
                continue;
            }
        }

        bool tooClose = false;
        for (const auto& recentDest : ai.recentDestinations)
        {
            float dx     = destination.x - recentDest.x;
            float dy     = destination.y - recentDest.y;
            float distSq = dx * dx + dy * dy;

            if (distSq < 100.0f)
            {
                tooClose = true;
                break;
            }
        }

        if (tooClose)
        {
            attempts++;
            continue;
        }

        ai.lastDirection.x = destination.x - currentGridPos.x;
        ai.lastDirection.y = destination.y - currentGridPos.y;

        ai.recentDestinations.push_back(destination);
        if (ai.recentDestinations.size() > ai.maxRecentDestinations)
        {
            ai.recentDestinations.erase(ai.recentDestinations.begin());
        }

        return destination;
    }

    return currentGridPos;
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
        outFoundRoad = true;
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

Human Human::CreateImmigrant()
{
    int margin = GRID_SIZE_X / 4;

    float spawnX = (float)(margin + rand() % (GRID_SIZE_X - 2 * margin));
    float spawnY = (float)(margin + rand() % (GRID_SIZE_Y - 2 * margin));

    Human immigrant(Math::fvec2(spawnX, spawnY));
    return immigrant;
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
