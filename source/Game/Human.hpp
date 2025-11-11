/**
 * \file Human.hpp
 * \author Hyunwoo Yang
 * \author
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

 #pragma once
 #include <vector>
 #include "Dokev/DKTexture.hpp"
 #include "Dokev/DKGameObject.hpp"
 #include "Engine/Vec2.hpp"
 #include "CS200/RGBA.hpp"

class GridMap;
class Road;

enum class HumanState
{
    Idle,              // Standing still, doing nothing
    Walking,           // Moving along a path
    FindingDestination,// Deciding where to go next
    Working,           // Performing work 
    Resting            // Resting to recover energy 
};

struct HumanStats
{
    float happiness = 100.0f;      // 0-100: Overall happiness
    float energy = 100.0f;         // 0-100: Energy level (future: affects speed)
    float walkSpeed = 1.0f;        // Movement speed multiplier
};

enum class WorkType
{
    None,
    Gathering,         
    Building,          
    Farming           
};

/**
 * \brief  who is controlling this human?
 */
enum class CommandSource
{
    Autonomous,        // Human decides on their own
    PlayerOrdered      // Player gave a direct command
};


struct HumanAI
{
    float idleTimer = 0.0f;           // Time spent idle
    float maxIdleTime = 0.5f;         // Max time before finding new destination
    Math::fvec2 previousGridPos = {-1.0f, -1.0f}; 

    float explorationRadius = 30.0f;  // How far from current position to explore
    bool isExploring = true;          // Currently in exploration mode

    Math::fvec2 lastDirection = {0.0f, 0.0f};  // Last movement direction
    std::vector<Math::fvec2> recentDestinations;  // Recent visited positions
    static constexpr size_t maxRecentDestinations = 5;  // How many positions to remember

    WorkType assignedWork = WorkType::None;
    Math::fvec2 workplacePosition = {-1, -1};
    bool hasWorkplace = false;

    // Road following
    bool preferRoads = true;          // Should try to use roads when pathfinding
    bool isFollowingRoad = false;     // Currently following a road
    bool roadDirection = true;        // true = forward, false = backward

    CommandSource currentCommand = CommandSource::Autonomous;
};

class Human : public Dokev::GameObject
{
public:
    
    Human(Math::fvec2 startGridPos);
    ~Human() = default;

    std::string TypeName() override { return "Human"; }
    GameObjectType Type() override { return GameObjectType::Human; }

    // CORE UPDATE & DRAW
    void Update(float deltaTime, GridMap* map, const Road* road);
    void Draw(int viewIndex, const GridMap* map) const;


    // MOVEMENT & PATHFINDING
    void FollowPath(const std::vector<Math::fvec2>& newPath, CommandSource source = CommandSource::Autonomous);
    void StopMovement();
    bool IsMoving() const { return isMoving; }

    // AI BEHAVIOR

    void DecideNextAction(GridMap* map);

    Math::fvec2 FindRandomDestination(GridMap* map, const Road* road);

    static Human CreateImmigrant();

    Math::fvec2 FindClosestWalkableTile(GridMap* map) const;

    bool FindNearbyRoad(const Road* road, Math::fvec2& outRoadPosition) const;

    Math::fvec2 FindNextRoadTile(const Road* road, bool& outFoundRoad) const;

    // WORK SYSTEM (Future)

    void AssignWork(WorkType type, Math::fvec2 workplacePos);

    void UnassignWork();

    void GoToWorkplace(GridMap* map);

    WorkType GetAssignedWork() const { return ai.assignedWork; }
    bool HasWorkAssignment() const { return ai.assignedWork != WorkType::None; }

    // GETTERS
    Math::fvec2 GetActualPosition() const { return actualPosition; }
    Math::fvec2 GetGridPosition() const { return currentGridPos; }
    HumanState GetState() const { return state; }
    const HumanStats& GetStats() const { return stats; }
    float GetHappiness() const { return stats.happiness; }

    // DEBUG
	void ToggleDebug() { debugMode = !debugMode; }
	bool IsDebugMode() const { return debugMode; }

private:

    // INTERNAL UPDATE FUNCTIONS
    void UpdateMovement(float deltaTime, GridMap* map);
    void UpdateIdle(float deltaTime, GridMap* map);
    void UpdateFindingDestination(float deltaTime, GridMap* map, const Road* road);
    void UpdateWorking(float deltaTime, GridMap* map);
    void ChangeState(HumanState newState);

    // TERRAIN HELPERS
    bool IsTerrainWalkable(const GridMap* map, int gridX, int gridY) const;
    std::string GetTerrainName(double height) const;


private:
    // POSITION & MOVEMENT
    Math::fvec2 actualPosition;      //  real-time position
    Math::fvec2 startPosition;       // Start of movement
    Math::fvec2 currentGridPos;      // Current grid tile
    Math::fvec2 targetGridPos;       // Target grid tile


    std::vector<Math::fvec2> path; // Current path 
    size_t currentPathIndex;       // Index in path array

    bool isMoving;

    float moveTimer;
    const float moveDelay = 0.5f; 


    // STATE & AI
    HumanState state = HumanState::Idle;
    HumanStats stats;
    HumanAI ai;


    // RENDERING
    CS200::RGBA color;
	bool debugMode = false;

    Dokev::Texture2D humanTexture;

};
