/**
 * \file Human.hpp
 * \author Hyunwoo Yang
 * \author
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include "CS200/RGBA.hpp"
#include "Dokev/GameObject/DKGameObject.hpp"
#include "Dokev/Graphics/DKTexture.hpp"
#include "Engine/Vec2.hpp"
#include <string>
#include <vector>


class GridMap;
class Road;
namespace Dokev { class GameObjectManager; }

enum class HumanState
{
    Idle,               // doing nothing
    Walking,            // Moving along a path
    FindingDestination, // Deciding where to go next
    Working,            // work
    Resting,            // resting at home after fatigue
};

struct HumanStats
{
    float happiness = 100.0f; // 0-100: happiness
    float energy    = 100.0f; // 0-100: Energy level
    float walkSpeed = 1.0f;   // Movement speed
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
    Autonomous,   // Human decides on their own
    PlayerOrdered // Player gave a direct command
};

struct HumanAI
{
    float       idleTimer       = 0.0f; // Time spent idle
    float       maxIdleTime     = 0.5f; // Max time before finding new destination
    Math::fvec2 previousGridPos = { -1.0f, -1.0f };

    float explorationRadius = 15.0f; // How far from current position to explore
    bool  isExploring       = true;  // Currently in exploration mode

    Math::fvec2              lastDirection = { 0.0f, 0.0f }; // Last movement direction
    std::vector<Math::fvec2> recentDestinations;             // Recent visited positions
    static constexpr size_t  maxRecentDestinations = 5;      // How many positions to remember

    int         assignedJobId = -1;                 // -1 = No assigned any work
    WorkType    assignedWorkType = WorkType::None;
    Math::fvec2 workplacePosition = { -1.0f, -1.0f };

    float workSessionTimer   = 0.0f;  // realtime works session
    float maxWorkSessionTime = 7.0f;  // 5sec for work

    // Fatigue system
    int   workSessionCount             = 0;
    int   maxWorkSessionsBeforeFatigue = 3;
    float restTimer                    = 0.0f;
    float maxRestTime                  = 7.0f;
    Math::fvec2 targetHomePosition     = { -1.0f, -1.0f };
    bool  isHeadingHome                = false;

    // Road following
    bool                 preferRoads           = true;  // Should try to use roads when pathfinding
    bool                 isFollowingRoad       = true;  // Currently following a road
    bool                 roadDirection         = true;  // true = forward, false = backward
    int                  roadReturnAttempts    = 0;     // Number of attempts to return to road
    static constexpr int maxRoadReturnAttempts = 3;     // Max attempts before giving up

    CommandSource currentCommand = CommandSource::Autonomous;
};

class Human : public Dokev::GameObject
{
public:

    Human(Math::fvec2 startGridPos);
    ~Human() = default;

    std::string TypeName() override
    {
        return "Human";
    }

    GameObjectType Type() override
    {
        return GameObjectType::Human;
    }

    // CORE UPDATE & DRAW
    void Update(float deltaTime, GridMap* map, const Road* road, Dokev::GameObjectManager* gameObjMgr = nullptr);
    void Draw(int viewIndex, const GridMap* map) const;


    void FollowPath(const std::vector<Math::fvec2>& newPath, CommandSource source = CommandSource::Autonomous);

    bool IsMoving() const
    {
        return isMoving;
    }

    // AI BEHAVIOR

    void DecideNextAction(GridMap* map);

    static Human CreateImmigrant(const Road* road = nullptr);

    Math::fvec2 FindClosestWalkableTile(GridMap* map) const;

    bool FindNearbyRoad(const Road* road, Math::fvec2& outRoadPosition) const;

    Math::fvec2 FindNextRoadTile(const Road* road, bool& outFoundRoad) const;

    // Work Functions

    bool HasWorkAssignment() const { return ai.assignedJobId != -1; }
    void CancelWork(); // Cancel the work when some situation happened

    int         GetAssignedJobId()       const { return ai.assignedJobId; }
    Math::fvec2 GetWorkplacePosition()   const { return ai.workplacePosition; }
    float       GetWorkSessionTimer()    const { return ai.workSessionTimer; }
    float       GetMaxWorkSessionTime()  const { return ai.maxWorkSessionTime; }
    void        SetMaxWorkSessionTime(float t) { ai.maxWorkSessionTime = t; }
    float       GetJobSearchRadius()     const { return ai.explorationRadius; }
    void        SetJobSearchRadius(float r)    { ai.explorationRadius = r; }


    // GETTERS
    Math::fvec2 GetActualPosition() const
    {
        return actualPosition;
    }

    Math::fvec2 GetGridPosition() const
    {
        return currentGridPos;
    }

    HumanState GetState() const
    {
        return state;
    }

    const HumanStats& GetStats() const
    {
        return stats;
    }

    float GetHappiness() const
    {
        return stats.happiness;
    }

    const std::string& GetName() const
    {
        return name;
    }

    // DEBUG
    void ToggleDebug()
    {
        debugMode = !debugMode;
    }

    bool IsDebugMode() const
    {
        return debugMode;
    }

private:

    // INTERNAL UPDATE FUNCTIONS
    void UpdateMovement(float deltaTime, GridMap* map);
    void UpdateIdle(float deltaTime, GridMap* map, Dokev::GameObjectManager* gameObjMgr);
    void UpdateFindingDestination(float deltaTime, GridMap* map, const Road* road);
    void ChangeState(HumanState newState);

    void UpdateWorking(float deltaTime);
    void UpdateResting(float deltaTime);

    // TERRAIN HELPERS
    std::string GetTerrainName(double height) const;


private:
    std::string name;

    // POSITION & MOVEMENT
    Math::fvec2 actualPosition; //  real-time position
    Math::fvec2 startPosition;  // Start of movement
    Math::fvec2 currentGridPos; // Current grid tile
    Math::fvec2 targetGridPos;  // Target grid tile


    std::vector<Math::fvec2> path;             // Current path
    size_t                   currentPathIndex; // Index in path array

    bool isMoving;

    float       moveTimer;
    const float moveDelay = 0.5f;


    // STATE & AI
    HumanState state = HumanState::Idle;
    HumanStats stats;
    HumanAI    ai;


    // RENDERING
    CS200::RGBA color;
    bool        debugMode = false;
    // bool isVisible = true;
    Dokev::Texture2D humanTexture;
};
