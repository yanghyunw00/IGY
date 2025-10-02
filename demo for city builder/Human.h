
#pragma once
#include "raylib.h"
#include <vector>

class GridMap;

class Human
{
public:
    Human(Vector2 startGridPos);

    void FollowPath(const std::vector<Vector2>& newPath);

    void Update(float deltaTime); 
    void Draw(int viewIndex, GridMap* map) const;

    bool IsMoving() const { return isMoving; }

	Vector2 GetActualPosition() { return actualPosition; }

private:
    Vector2 actualPosition; // 실시간 위치
    Vector2 startPosition;
    Vector2 currentGridPos; // 현재 그리드 위치
    Vector2 targetGridPos;  


    std::vector<Vector2> path; 
    size_t currentPathIndex;  

    bool isMoving;
    float moveTimer;
    const float moveDelay = 0.5f; 

    Color color;
};
