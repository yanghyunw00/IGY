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
#include "GridMap.hpp"
#include <iostream>

Human::Human(Math::fvec2 startGridPos) : currentGridPos(startGridPos), targetGridPos(startGridPos), currentPathIndex(0), isMoving(false), color(Dokev::ColorPalette::BLUE)
{
    startPosition  = currentGridPos;
    actualPosition = currentGridPos;
    moveTimer      = 0.0f;
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
    ai.idleTimer += deltaTime;
    
    if (ai.idleTimer >= ai.maxIdleTime)
    {
        ai.idleTimer = 0.0f;
        std::cout << "Human: Time to find new destination!" << std::endl;
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
    
    auto path = map->GetCurrentPath();
    
    if (!path.empty())
    {
        FollowPath(path);
        ChangeState(HumanState::Walking);
    }
    else
    {
        ChangeState(HumanState::Idle);
        ai.idleTimer = ai.maxIdleTime - 0.5f;  // 0.5초 후 다시 시도
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
    float gridX_float = actualPosition.x;
    float gridY_float = actualPosition.y;

    int gridX_int = static_cast<int>(gridX_float);
    int gridY_int = static_cast<int>(gridY_float);

    if (gridX_int < 0 || gridX_int >= GRID_SIZE_X || gridY_int < 0 || gridY_int >= GRID_SIZE_Y)
        return;

    // Get the view array
    GridCell** viewArray = map->GetViewArray(viewIndex);

   // map->TransformPathPointForView

    float height = static_cast<float>(viewArray[gridY_int * GRID_SIZE_X + gridX_int]->getHeight());

    int originX = ((TILE_WIDTH / 2) * GRID_SIZE_X) - TILE_WIDTH / 2;
    int originY = 0;

    int screenX = originX + static_cast<int>((gridX_float - gridY_float) * TILE_WIDTH_HALF);
    int screenY = originY + static_cast<int>((gridX_float + gridY_float) * TILE_HEIGHT_HALF);
    int topY    = static_cast<int>(height) - screenY;

    int drawX = screenX + TILE_WIDTH_HALF;
    int drawY = topY;

    Dokev::DrawCircle(drawX, drawY, 20, Dokev::ColorPalette::RED);
    Dokev::DrawCircleLines(drawX, drawY, 20, Dokev::ColorPalette::YELLOW);
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
    if (state == HumanState::Idle)
    {
        ChangeState(HumanState::FindingDestination);
    }
}

Math::fvec2 Human::FindRandomDestination(GridMap* map) const
{
    (void)map;
    
    float maxDistance = ai.explorationRadius; 
    
    int attempts = 0;
    const int maxAttempts = 10;
    
    while (attempts < maxAttempts)
    {
        float angle = (rand() % 360) * (3.14159f / 180.0f);
        float distance = (rand() % (int)maxDistance) + 3.0f;  
        
        Math::fvec2 destination;
        destination.x = currentGridPos.x + cos(angle) * distance;
        destination.y = currentGridPos.y + sin(angle) * distance;
        
        // 맵 경계 체크
        destination.x = std::max(0.0f, std::min((float)(GRID_SIZE_X - 1), destination.x));
        destination.y = std::max(0.0f, std::min((float)(GRID_SIZE_Y - 1), destination.y));
        
        // 지형 체크 (나중에 구현)
        // if (IsTerrainWalkable(map, (int)destination.x, (int)destination.y))
        // {
        //     return destination;
        // }
        
        // 일단은 모든 위치 허용
        return destination;
        
        attempts++;
    }
    
    // 실패 시 현재 위치 근처 반환
    return Math::fvec2(
        currentGridPos.x + (rand() % 5 - 2), 
        currentGridPos.y + (rand() % 5 - 2)
    );
}

bool Human::FindNearbyRoad(GridMap* map, Math::fvec2& outRoadPosition) const
{
    return false;
}
