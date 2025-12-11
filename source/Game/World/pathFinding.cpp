/**
 * \file pathFinding.cpp
 * \author Hyunwoo Yang
 * \author
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "PathFinding.hpp"
#include "GridCell.hpp" 
#include <algorithm>
#include <iostream>
#include <limits>
#include <queue> 

PathFinding::PathFinding(int sizeX, int sizeY, float maxClimb)
	: gridSizeX(sizeX), gridSizeY(sizeY), maxClimbHeight(maxClimb)
{
	heightMap.resize(gridSizeY, std::vector<float>(gridSizeX, 0.0f));
	walkableMap.resize(gridSizeY, std::vector<bool>(gridSizeX, true));
	closedSet.resize(gridSizeY, std::vector<bool>(gridSizeX, false)); // Pre-allocate once

	allNodes.resize(gridSizeY);
	for(int y = 0; y < gridSizeY; y++) {
		allNodes[y].resize(gridSizeX);
		for(int x = 0; x < gridSizeX; x++) {
			allNodes[y][x] = Node(x, y);
		}
	}
}

void PathFinding::UpdateMapFromGrid(const std::vector<std::vector<GridCell>>& grid)
{
	if(costMap.empty()) {
		costMap.resize(gridSizeY, std::vector<float>(gridSizeX, 1.0f));
	}

	for(int y = 0; y < gridSizeY; ++y) {
		for(int x = 0; x < gridSizeX; ++x) {
			walkableMap[y][x] = grid[y][x].isWalkable();
			costMap[y][x] = grid[y][x].getMovementCost();
			heightMap[y][x] = static_cast<float>(grid[y][x].getHeight());
		}
	}
}

void PathFinding::SetHeightMap(const std::vector<double>& heights)
{
	for(int y = 0; y < gridSizeY; y++)
	{
		for(int x = 0; x < gridSizeX; x++)
		{
			if(y * gridSizeX + x < heights.size())
			{
				heightMap[y][x] = static_cast<float>(heights[y * gridSizeX + x]);
			}
		}
	}
}

void PathFinding::SetWalkable(int x, int y, bool walkable)
{
	if(x >= 0 && x < gridSizeX && y >= 0 && y < gridSizeY)
	{
		walkableMap[y][x] = walkable;
	}
}

void PathFinding::SetCostMap(const std::vector<float>& costs)
{
	if(costMap.empty()) {
		costMap.resize(gridSizeY, std::vector<float>(gridSizeX, 1.0f));
	}
	for(int y = 0; y < gridSizeY; y++)
	{
		for(int x = 0; x < gridSizeX; x++)
		{
			if(y * gridSizeX + x < costs.size()) {
				costMap[y][x] = costs[y * gridSizeX + x];
			}
		}
	}
}

bool PathFinding::IsPositionWalkable(int x, int y)
{
	if(x < 0 || x >= gridSizeX || y < 0 || y >= gridSizeY)
	{
		return false;
	}
	return walkableMap[y][x];
}

bool PathFinding::CanClimb(int fromX, int fromY, int toX, int toY)
{
	if(!IsPositionWalkable(fromX, fromY) || !IsPositionWalkable(toX, toY))
	{
		return false;
	}

	float heightDiff = std::abs(heightMap[toY][toX] - heightMap[fromY][fromX]);
	return heightDiff <= maxClimbHeight;
}

float PathFinding::GetManhattanDistance(Node* nodeA, Node* nodeB)
{
	return static_cast<float>(std::abs(nodeA->x - nodeB->x) + std::abs(nodeA->y - nodeB->y));
}

std::vector<Node*> PathFinding::GetNeighbors(Node* node)
{
	std::vector<Node*> neighbors;

	int dx[] = { 0, 0, 1, -1 };
	int dy[] = { 1, -1, 0, 0 };

	for(int i = 0; i < 4; i++)
	{
		int checkX = node->x + dx[i];
		int checkY = node->y + dy[i];

		if(checkX >= 0 && checkX < gridSizeX &&
			checkY >= 0 && checkY < gridSizeY &&
			CanClimb(node->x, node->y, checkX, checkY))
		{
			neighbors.push_back(&allNodes[checkY][checkX]);
		}
	}

	return neighbors;
}

std::vector<Math::fvec2> PathFinding::RetracePath(Node* startNode, Node* endNode)
{
	std::vector<Math::fvec2> path;
	Node* currentNode = endNode;

	while(currentNode != nullptr && currentNode != startNode)
	{
		path.push_back({ static_cast<float>(currentNode->x), static_cast<float>(currentNode->y) });
		currentNode = currentNode->parent;
	}
	path.push_back({ static_cast<float>(startNode->x), static_cast<float>(startNode->y) });

	std::reverse(path.begin(), path.end());
	return path;
}

std::vector<Math::fvec2> PathFinding::FindPath(Math::fvec2 start, Math::fvec2 end)
{
	return FindPath(static_cast<int>(start.x), static_cast<int>(start.y),
		static_cast<int>(end.x), static_cast<int>(end.y));
}

std::vector<Math::fvec2> PathFinding::FindPath(int startX, int startY, int endX, int endY)
{
	const float UPHILL_COST_MULTIPLIER = 2.0f;
	const float DOWNHILL_COST_MULTIPLIER = 0.5f; 
	const float INF = std::numeric_limits<float>::infinity();

	for(int y = 0; y < gridSizeY; y++)
	{
		for(int x = 0; x < gridSizeX; x++)
		{
			Node& node = allNodes[y][x];
			node.gCost = INF;
			node.hCost = 0;
			node.parent = nullptr;
			closedSet[y][x] = false;
		}
	}

	if(startX < 0 || startX >= gridSizeX || startY < 0 || startY >= gridSizeY ||
		endX < 0 || endX >= gridSizeX || endY < 0 || endY >= gridSizeY ||
		!IsPositionWalkable(startX, startY) || !IsPositionWalkable(endX, endY))
	{
		return {};
	}

	Node* startNode = &allNodes[startY][startX];
	Node* targetNode = &allNodes[endY][endX];

	std::priority_queue<Node*, std::vector<Node*>, NodeComparator> openSet;

	startNode->gCost = 0;
	startNode->hCost = GetManhattanDistance(startNode, targetNode);
	startNode->CalculateFCost();
	openSet.push(startNode);

	while(!openSet.empty())
	{
		Node* currentNode = openSet.top();
		openSet.pop();

		if(closedSet[currentNode->y][currentNode->x])
		{
			continue;
		}
		closedSet[currentNode->y][currentNode->x] = true;

		if(currentNode == targetNode)
		{
			return RetracePath(startNode, targetNode);
		}

		for(Node* neighbor : GetNeighbors(currentNode))
		{
			if(closedSet[neighbor->y][neighbor->x]) continue;

			float distanceCost = GetManhattanDistance(currentNode, neighbor);
			float terrainCost = costMap[neighbor->y][neighbor->x];
			float heightDiff = heightMap[neighbor->y][neighbor->x] - heightMap[currentNode->y][currentNode->x];
			float slopeCost = 0;

			if(heightDiff > 0) slopeCost = heightDiff * UPHILL_COST_MULTIPLIER;  
			else if(heightDiff < 0) slopeCost = std::abs(heightDiff) * DOWNHILL_COST_MULTIPLIER;


			float newMovementCostToNeighbor = currentNode->gCost + distanceCost + terrainCost + slopeCost;

			if(newMovementCostToNeighbor < neighbor->gCost)
			{
				neighbor->gCost = newMovementCostToNeighbor;
				neighbor->hCost = GetManhattanDistance(neighbor, targetNode);
				neighbor->parent = currentNode;
				neighbor->CalculateFCost();

				openSet.push(neighbor);
			}
		}
	}

	return {}; 
}
