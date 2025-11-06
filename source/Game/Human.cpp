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
#include <iostream>

Human::Human(Math::fvec2 startGridPos) : currentGridPos(startGridPos), targetGridPos(startGridPos), currentPathIndex(0), isMoving(false), color(Dokev::ColorPalette::BLUE)
{
    startPosition  = currentGridPos;
    actualPosition = currentGridPos;
    moveTimer      = 0.0f;
    humanTexture = Dokev::LoadTexture("Assets/images/textures/Human.png");
}

void Human::FollowPath(const std::vector<Math::fvec2>& newPath, CommandSource source)
{
    (void) source;

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

    if (t >= 1.0f) // 목표 지점 도착
    {
        // 위치를 목표 지점에 정확히 맞춤ccccccccccccc
        actualPosition = targetGridPos;
        currentGridPos = targetGridPos;

        /// debugging mode
        if (debugMode && map)
        {
            int gridX = (int)currentGridPos.x;
            int gridY = (int)currentGridPos.y;

            if (gridX >= 0 && gridX < GRID_SIZE_X && gridY >= 0 && gridY < GRID_SIZE_Y)
            {
                GridCell** viewArray   = map->GetViewArray(1);
                GridCell*  currentCell = viewArray[gridY * GRID_SIZE_X + gridX];

                std::string terrainName;
                double      height = currentCell->getHeight();
				terrainName = GetTerrainName(height);
                std::cout << "Arrived Tile: (" << gridX << ", " << gridY << ") - " << terrainName << " [Height: " << (int)height << "]" << std::endl;
            }
        }

        currentPathIndex++;

        // 끝에 도달했는지
        if (currentPathIndex >= path.size())
        {
            isMoving = false;
            path.clear();
            if (debugMode)
            {
                std::cout << "=== Destination ===" << std::endl;
            }
        }
        else // 아직 덜 갔으면
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
    (void) map;
    ai.idleTimer += deltaTime;
    
    if (ai.idleTimer >= ai.maxIdleTime)
    {
        ai.idleTimer = 0.0f;
        ChangeState(HumanState::FindingDestination);
    }
}

void Human::UpdateFindingDestination(float deltaTime, GridMap* map)
{
    (void) deltaTime;
    
    if (!map) return;
    
    Math::fvec2 destination = FindRandomDestination(map);
    
    map->ClearPath();
    map->FindPath(
        static_cast<int>(currentGridPos.x), 
        static_cast<int>(currentGridPos.y),
        static_cast<int>(destination.x), 
        static_cast<int>(destination.y)
    );
    
    auto path_ = map->GetCurrentPath();
    
    if (!path_.empty())
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
void Human::Update(float deltaTime, GridMap* map)
{
   if (state == HumanState::Walking && isMoving)
    {
        UpdateMovement(deltaTime, map);
        
        if (!isMoving)
        {
            ChangeState(HumanState::Idle);
        }
    }
    else if (state == HumanState::Idle)
    {
        UpdateIdle(deltaTime, map);
    }
    else if (state == HumanState::FindingDestination)
    {
        UpdateFindingDestination(deltaTime, map);
    }
}

void Human::Draw(int viewIndex, const GridMap* map) const
{
    // Transform the position based on the view rotation
    Math::fvec2 transformedPos = map->TransformPathPointForView(actualPosition, viewIndex);

    float gridX_float = transformedPos.x;
    float gridY_float = transformedPos.y;

    int gridX_int = static_cast<int>(gridX_float);
    int gridY_int = static_cast<int>(gridY_float);

    // if (gridX_int < 0 || gridX_int >= GRID_SIZE_X || gridY_int < 0 || gridY_int >= GRID_SIZE_Y)
    //     return;

    // Get the view array
    GridCell** viewArray = map->GetViewArray(viewIndex);

    float height = static_cast<float>(viewArray[gridY_int * GRID_SIZE_X + gridX_int]->getHeight());

    int originX = ((TILE_WIDTH / 2) * GRID_SIZE_X) - TILE_WIDTH / 2;
    int originY = 0;

    int screenX = originX + static_cast<int>((gridX_float - gridY_float) * TILE_WIDTH_HALF);
    int screenY = originY + static_cast<int>((gridX_float + gridY_float) * TILE_HEIGHT_HALF);
    int topY    = static_cast<int>(height) - screenY;

    int drawX = screenX + TILE_WIDTH_HALF;
    int drawY = topY;

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
    (void) map;
    if (state == HumanState::Idle)
    {
        ChangeState(HumanState::FindingDestination);
    }
}

Math::fvec2 Human::FindRandomDestination(GridMap* map)
{
    if (!map)
    {
        return currentGridPos;
    }

    // Range of random tile to search
    float maxDistance = ai.explorationRadius;
    float minDistance = maxDistance * 0.5f;

    int attempts = 0;
    const int maxAttempts = 10;

    while (attempts < maxAttempts)
    {
        float angle = (rand() % 360) * (3.14159f / 180.0f);
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
            GridCell** viewArray = map->GetViewArray(1);
            GridCell* cell = viewArray[gridY * GRID_SIZE_X + gridX];
            double height = cell->getHeight();

            if (height < 75.0)  //if water , try again
            {
                attempts++;
                continue;
            }
        }

        bool tooClose = false;
        for (const auto& recentDest : ai.recentDestinations)
        {
            float dx = destination.x - recentDest.x;
            float dy = destination.y - recentDest.y;
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
}

bool Human::FindNearbyRoad(GridMap* map, Math::fvec2& outRoadPosition) const
{
    return false;
}

Human Human::CreateImmigrant(GridMap* map)
{
    // Randomly select one of the 4 sides of the map
    int side = rand() % 4;  // 0=top, 1=right, 2=bottom, 3=left

    float spawnX, spawnY;

    switch (side)
    {
        case 0: // Top (entering from north)
            spawnX = (float)(rand() % GRID_SIZE_X);
            spawnY = 0.0f;
            break;
        case 1: // Right (entering from east)
            spawnX = GRID_SIZE_X - 1;
            spawnY = (float)(rand() % GRID_SIZE_Y);
            break;
        case 2: // Bottom (entering from south)
            spawnX = (float)(rand() % GRID_SIZE_X);
            spawnY = GRID_SIZE_Y - 1;
            break;
        case 3: // Left (entering from west)
            spawnX = 0.0f;
            spawnY = (float)(rand() % GRID_SIZE_Y);
            break;
        default:
            spawnX = 0.0f;
            spawnY = 0.0f;
            break;
    }

    Human immigrant(Math::fvec2(spawnX, spawnY));

    //// Find path to enter the map
    //if (map)
    //{
    //    //closest land tile
    //    Math::fvec2 entryPoint = immigrant.FindClosestWalkableTile(map);

    //    map->ClearPath();

    //    std::vector<Math::fvec2> entryPath;

    //    float dx = entryPoint.x - spawnX;
    //    float dy = entryPoint.y - spawnY;
    //    float totalDist = sqrt(dx * dx + dy * dy);

    //    int numPoints = (int)totalDist + 1; 

    //    for (int i = 0; i <= numPoints; i++)
    //    {
    //        float t = (float)i / (float)numPoints; 
    //        Math::fvec2 point;
    //        point.x = spawnX + dx * t;
    //        point.y = spawnY + dy * t;
    //        entryPath.push_back(point);
    //    }

    //    immigrant.FollowPath(entryPath);
    //    immigrant.ChangeState(HumanState::Walking);
    //}

    return immigrant;
}

Math::fvec2 Human::FindClosestWalkableTile(GridMap* map) const
{
    if (!map)
    {
        return Math::fvec2(GRID_SIZE_X / 2.0f, GRID_SIZE_Y / 2.0f);
    }

    float closestDist = 999999.0f;
    Math::fvec2 closestTile(GRID_SIZE_X / 2.0f, GRID_SIZE_Y / 2.0f);

    // Search along map boundaries (top, bottom, left, right edges only)
    GridCell** viewArray = map->GetViewArray(1);

    for (int x = 0; x < GRID_SIZE_X; x++)
    {
        for (int y = 0; y < 5 && y < GRID_SIZE_Y; y++)
        {
            GridCell* cell = viewArray[y * GRID_SIZE_X + x];
            if (cell->getHeight() >= 75.0)  
            {
                float dx = x - currentGridPos.x;
                float dy = y - currentGridPos.y;
                float dist = dx * dx + dy * dy;

                if (dist < closestDist)
                {
                    closestDist = dist;
                    closestTile = Math::fvec2((float)x, (float)y);
                }
            }
        }

        // Bottom edge (check last 5 rows)
        for (int y = GRID_SIZE_Y - 5; y < GRID_SIZE_Y; y++)
        {
            if (y < 0) continue;
            GridCell* cell = viewArray[y * GRID_SIZE_X + x];
            if (cell->getHeight() >= 75.0)
            {
                float dx = x - currentGridPos.x;
                float dy = y - currentGridPos.y;
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
        // Left edge (check first 5 columns)
        for (int x = 0; x < 5 && x < GRID_SIZE_X; x++)
        {
            GridCell* cell = viewArray[y * GRID_SIZE_X + x];
            if (cell->getHeight() >= 75.0)
            {
                float dx = x - currentGridPos.x;
                float dy = y - currentGridPos.y;
                float dist = dx * dx + dy * dy;

                if (dist < closestDist)
                {
                    closestDist = dist;
                    closestTile = Math::fvec2((float)x, (float)y);
                }
            }
        }

        // Right edge (check last 5 columns)
        for (int x = GRID_SIZE_X - 5; x < GRID_SIZE_X; x++)
        {
            if (x < 0) continue;
            GridCell* cell = viewArray[y * GRID_SIZE_X + x];
            if (cell->getHeight() >= 75.0)
            {
                float dx = x - currentGridPos.x;
                float dy = y - currentGridPos.y;
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
