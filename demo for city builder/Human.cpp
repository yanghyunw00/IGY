#include "Human.h"
#include "GridMap.h" 
#include <iostream>

Human::Human(Vector2 startGridPos)
    : currentGridPos(startGridPos)
    , targetGridPos(startGridPos)
    , currentPathIndex(0)
    , isMoving(false)
    , color(BLUE)
{
    startPosition = currentGridPos;
    actualPosition = currentGridPos;
    moveTimer = 0.0f;
}

void Human::FollowPath(const std::vector<Vector2>& newPath)
{
    if(newPath.size() <= 1)
    {
        isMoving = false;
        path.clear();
        return;
    }

    path = newPath;
    currentPathIndex = 0;


    currentGridPos = path[0];
    actualPosition = path[0];
    startPosition = path[0];

    currentPathIndex = 1;
    targetGridPos = path[currentPathIndex];
    isMoving = true;
    moveTimer = 0.0f;

}

void Human::Update(float deltaTime)
{
    if(!isMoving) return;

    moveTimer += deltaTime;
    float t = moveTimer / moveDelay;

    if(t > 1.0f)
    {
        t = 1.0f;
    }

    actualPosition.x = startPosition.x + (targetGridPos.x - startPosition.x) * t;
    actualPosition.y = startPosition.y + (targetGridPos.y - startPosition.y) * t;

    if(t >= 1.0f) // 목표 지점 도착
    {
        // 위치를 목표 지점에 정확히 맞춤
        actualPosition = targetGridPos;
        currentGridPos = targetGridPos;

        currentPathIndex++; 

        // 끝에 도달했는지 
        if(currentPathIndex >= path.size())
        {
            isMoving = false; 
            path.clear();
            std::cout << "도착!" << std::endl;
        }
        else // 아직 덜 갓으면
        {
            startPosition = currentGridPos;
            targetGridPos = path[currentPathIndex];
            moveTimer = 0.0f; 
        }
    }
    else 
    {
        actualPosition.x = startPosition.x + (targetGridPos.x - startPosition.x) * t;
        actualPosition.y = startPosition.y + (targetGridPos.y - startPosition.y) * t;
    }
}

void Human::Draw(int viewIndex, GridMap* map) const
{
    Vector2 transformedPos = map->TransformPathPointForView(actualPosition, viewIndex);

    float gridX_float = transformedPos.x;
    float gridY_float = transformedPos.y;

    int gridX_int = static_cast<int>(gridX_float);
    int gridY_int = static_cast<int>(gridY_float);

    if(gridX_int < 0 || gridX_int >= GRID_SIZE_X || gridY_int < 0 || gridY_int >= GRID_SIZE_Y)
        return;

    GridCell** viewArray = map->GetViewArray(viewIndex);
    float height = viewArray[gridY_int * GRID_SIZE_X + gridX_int]->getHeight();

    int originX = map->GetIsoOriginX();
    int originY = map->GetIsoOriginY();

    int isoX = originX + (gridX_float - gridY_float) * TILE_WIDTH_HALF;
    int isoY = originY + (gridX_float + gridY_float) * TILE_HEIGHT_HALF - static_cast<int>(height);


    DrawCircle(isoX + TILE_WIDTH_HALF, isoY + TILE_HEIGHT_HALF, 10, color);
}
