/**
 * \file Human.hpp
 * \author Hyunwoo Yang
 * \author
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */


 #include <vector>
 #include "Dokev/DKTexture.hpp"
 #include "Engine/Vec2.hpp"
 #include "CS200/RGBA.hpp"

class GridMap;

enum class HumanState
{
	Idle,
    Walking,  
	Arrived
}; 


class Human
{
public:
    Human(Math::fvec2 startGridPos);

    void FollowPath(const std::vector<Math::fvec2>& newPath);
	void Update(float deltaTime, GridMap* map);
    void Draw(int viewIndex, const GridMap* map) const;
    bool IsMoving() const { return isMoving; }


	Math::fvec2 GetActualPosition() const { return actualPosition; }

	void ToggleDebug() { debugMode = !debugMode; }
	bool IsDebugMode() const { return debugMode; }
	HumanState GetState() const { return state; }

private:
    Math::fvec2 actualPosition; // real time position
    Math::fvec2 startPosition;
    Math::fvec2 currentGridPos;
    Math::fvec2 targetGridPos;  


    std::vector<Math::fvec2> path; 
    size_t currentPathIndex;  

    bool isMoving;
	bool debugMode = false;

    float moveTimer;
    const float moveDelay = 0.5f; 

    CS200::RGBA color;
	HumanState state = HumanState::Idle;
};
