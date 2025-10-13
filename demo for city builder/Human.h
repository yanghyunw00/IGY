
#pragma once
#include "raylib.h"
#include <vector>

class GridMap;

enum class HumanState
{
	Idle,           // 가만히 있음
	SeekingRoad,    // 가장 가까운 도로 찾는 중
	MovingToRoad,   // 도로로 이동 중
	FollowingRoad,  // 도로를 따라 이동 중
	Arrived         // 목적지 도착
};
class Human
{
public:
    Human(Vector2 startGridPos);

    void FollowPath(const std::vector<Vector2>& newPath);
	void Update(float deltaTime, GridMap* map);
    void Draw(int viewIndex, GridMap* map) const;
    bool IsMoving() const { return isMoving; }


	Vector2 GetActualPosition() const { return actualPosition; }

	void ToggleDebug() { debugMode = !debugMode; }
	bool IsDebugMode() const { return debugMode; }
	HumanState GetState() const { return state; }

private:
    Vector2 actualPosition; // 실시간 위치
    Vector2 startPosition;
    Vector2 currentGridPos; // 현재 그리드 위치
    Vector2 targetGridPos;  


    std::vector<Vector2> path; 
    size_t currentPathIndex;  

    bool isMoving;
	bool debugMode = false;

    float moveTimer;
    const float moveDelay = 0.5f; 

    Color color;
	HumanState state = HumanState::Idle;
};
