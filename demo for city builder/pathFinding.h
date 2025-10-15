#pragma once
#include <vector>
#include <queue>
#include <cmath> 
#include "raylib.h"

struct Node
{
    int x, y;
    float gCost;    // 시작 노드로부터의 비용
    float hCost;    // 목표 노드까지의 예상 비용 
    float fCost;    // gCost + hCost
    Node* parent;   // 경로를 역추적하기 위한 부모 노드 포인터

    Node(int x = 0, int y = 0) : x(x), y(y), gCost(0), hCost(0), fCost(0), parent(nullptr) {}

    void CalculateFCost()
    {
        fCost = gCost + hCost;
    }
};

struct NodeComparator
{
    bool operator()(Node* a, Node* b)
    {
        if(a->fCost == b->fCost)
        {
            return a->hCost > b->hCost;
        }
        return a->fCost > b->fCost;
    }
};

class PathFinding
{
public:
    PathFinding(int sizeX, int sizeY, float maxClimb = 50.0f);

    void SetHeightMap(const std::vector<double>& heights);
    void SetWalkable(int x, int y, bool walkable);
    void SetCostMap(const std::vector<float>& costs);
    bool IsPositionWalkable(int x, int y);

    std::vector<Vector2> FindPath(Vector2 start, Vector2 end);
    std::vector<Vector2> FindPath(int startX, int startY, int endX, int endY);


private:
    float GetManhattanDistance(Node* nodeA, Node* nodeB);
    bool CanClimb(int fromX, int fromY, int toX, int toY);
    std::vector<Node*> GetNeighbors(Node* node);
    std::vector<Vector2> RetracePath(Node* startNode, Node* endNode);

    int gridSizeX, gridSizeY;
    float maxClimbHeight;

    std::vector<std::vector<float>> heightMap;
    std::vector<std::vector<bool>> walkableMap;
    std::vector<std::vector<float>> costMap;

    std::vector<std::vector<Node>> allNodes;
};
