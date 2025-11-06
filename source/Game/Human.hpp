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
 #include "Engine/Vec2.hpp"
 #include "CS200/RGBA.hpp"

class GridMap;

enum class HumanState
{
    Idle,              // Standing still, doing nothing
    Walking,           // Moving along a path
    FindingDestination,// Deciding where to go next (AI thinking)
    Working,           // Performing work (gathering, building, etc.)
    Resting            // Resting to recover energy (future use)
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
    Gathering,         // Gathering resources
    Building,          // Building structures
    Farming            // Farming crops
};

/**
 * \brief Command source - who is controlling this human?
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

    float explorationRadius = 30.0f;  // How far from current position to explore
    bool isExploring = true;          // Currently in exploration mode

    Math::fvec2 lastDirection = {0.0f, 0.0f};  // Last movement direction
    std::vector<Math::fvec2> recentDestinations;  // Recent visited positions
    static constexpr size_t maxRecentDestinations = 5;  // How many positions to remember

    // Future: work assignment
    WorkType assignedWork = WorkType::None;
    Math::fvec2 workplacePosition = {-1, -1};  // Where to work
    bool hasWorkplace = false;

    // Future: road detection
    bool preferRoads = true;          // Should try to use roads when pathfinding

    CommandSource currentCommand = CommandSource::Autonomous;
};

class Human
{
public:
    
    Human(Math::fvec2 startGridPos);
    ~Human() = default;

    // ========================================================================
    // CORE UPDATE & DRAW
    // ========================================================================
    void Update(float deltaTime, GridMap* map);
    void Draw(int viewIndex, const GridMap* map) const;


    // ========================================================================
    // MOVEMENT & PATHFINDING
    // ========================================================================
    void FollowPath(const std::vector<Math::fvec2>& newPath, CommandSource source = CommandSource::Autonomous);
    void StopMovement();
    bool IsMoving() const { return isMoving; }

    // ========================================================================
    // AI BEHAVIOR
    // ========================================================================

    /**
     * \brief Make human decide what to do next (autonomous AI)
     */
    void DecideNextAction(GridMap* map);

    /**
     * \brief Find a random walkable destination for exploration
     * \return Random grid position to explore
     */
    Math::fvec2 FindRandomDestination(GridMap* map);

    /**
     * \brief Create a human starting from outside the map (immigration)
     * \param map The grid map
     * \return Human positioned outside map boundaries
     */
    static Human CreateImmigrant(GridMap* map);

    /**
     * \brief Find closest walkable tile from current position (for entering map)
     * \param map The grid map
     * \return Closest valid tile position
     */
    Math::fvec2 FindClosestWalkableTile(GridMap* map) const;

    /**
     * \brief Check if there's a road nearby and path to it (future feature)
     * Using BFS
     */
    bool FindNearbyRoad(GridMap* map, Math::fvec2& outRoadPosition) const;

    // ========================================================================
    // WORK SYSTEM (Future)
    // ========================================================================

    /**
     * \brief Assign work to this human
     */
    void AssignWork(WorkType type, Math::fvec2 workplacePos);

    /**
     * \brief Remove work assignment
     */
    void UnassignWork();

    /**
     * \brief Go to assigned workplace
     */
    void GoToWorkplace(GridMap* map);

    WorkType GetAssignedWork() const { return ai.assignedWork; }
    bool HasWorkAssignment() const { return ai.assignedWork != WorkType::None; }

    // ========================================================================
    // GETTERS
    // ========================================================================
    Math::fvec2 GetActualPosition() const { return actualPosition; }
    Math::fvec2 GetGridPosition() const { return currentGridPos; }
    HumanState GetState() const { return state; }
    const HumanStats& GetStats() const { return stats; }
    float GetHappiness() const { return stats.happiness; }

    // ========================================================================
    // DEBUG
    // ========================================================================
	void ToggleDebug() { debugMode = !debugMode; }
	bool IsDebugMode() const { return debugMode; }

private:
    // ========================================================================
    // INTERNAL UPDATE FUNCTIONS
    // ========================================================================
    void UpdateMovement(float deltaTime, GridMap* map);
    void UpdateIdle(float deltaTime, GridMap* map);
    void UpdateFindingDestination(float deltaTime, GridMap* map);
    void UpdateWorking(float deltaTime, GridMap* map);
    void ChangeState(HumanState newState);

    // ========================================================================
    // TERRAIN HELPERS
    // ========================================================================
    bool IsTerrainWalkable(const GridMap* map, int gridX, int gridY) const;
    std::string GetTerrainName(double height) const;


private:
    // ========================================================================
    // POSITION & MOVEMENT
    // ========================================================================
    Math::fvec2 actualPosition;      // Interpolated real-time position
    Math::fvec2 startPosition;       // Start of current movement segment
    Math::fvec2 currentGridPos;      // Current grid tile
    Math::fvec2 targetGridPos;       // Target grid tile for current segment


    std::vector<Math::fvec2> path; // Current path 
    size_t currentPathIndex;       // Index in path array

    bool isMoving;

    float moveTimer; // Timer for interpolation
    const float moveDelay = 0.5f; 


    // ========================================================================
    // STATE & AI
    // ========================================================================
    HumanState state = HumanState::Idle;
    HumanStats stats;
    HumanAI ai;


    // ========================================================================
    // RENDERING
    // ========================================================================
    CS200::RGBA color;
	bool debugMode = false;

};
