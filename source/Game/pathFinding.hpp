/**
 * \file pathFinding.hpp
 * \author Hyunwoo Yang
 * \author
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#pragma once
#include <vector>
#include "GridCell.hpp"
#include "Engine/Vec2.hpp"

struct Node
{
	int x, y;
	float gCost;   
	float hCost;   
	float fCost;    // gCost + hCost
	Node* parent;   

	Node(int x = 0, int y = 0) : x(x), y(y), gCost(0), hCost(0), fCost(0), parent(nullptr) {}

	void CalculateFCost()
	{
		fCost = gCost + hCost;
	}
};

struct NodeComparator
{
	bool operator()(const Node* a, const Node* b) const
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

	std::vector<Math::fvec2> FindPath(Math::fvec2 start, Math::fvec2 end);
	std::vector<Math::fvec2> FindPath(int startX, int startY, int endX, int endY);

	// Each tile for futrue...
	void SetHeightMap(const std::vector<double>& heights);
	void SetWalkable(int x, int y, bool walkable);
	void SetCostMap(const std::vector<float>& costs);
	bool IsPositionWalkable(int x, int y);


private:
	// A*  helper function
	float GetManhattanDistance(Node* nodeA, Node* nodeB);
	bool CanClimb(int fromX, int fromY, int toX, int toY);
	std::vector<Node*> GetNeighbors(Node* node);
	std::vector<Math::fvec2> RetracePath(Node* startNode, Node* endNode);

	int gridSizeX, gridSizeY;
	float maxClimbHeight;

	std::vector<std::vector<float>> heightMap;
	std::vector<std::vector<bool>> walkableMap;
	std::vector<std::vector<float>> costMap;

    //save all nodes to here
	std::vector<std::vector<Node>> allNodes;
};
