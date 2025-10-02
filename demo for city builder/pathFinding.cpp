#include "PathFinding.h"
#include <algorithm>
#include <iostream>

PathFinding::PathFinding(int sizeX, int sizeY, float maxClimb)
	: gridSizeX(sizeX), gridSizeY(sizeY), maxClimbHeight(maxClimb)
{
	heightMap.resize(gridSizeY, std::vector<float>(gridSizeX, 0.0f));
	walkableMap.resize(gridSizeY, std::vector<bool>(gridSizeX, true));

	allNodes.resize(gridSizeY);
	for(int y = 0; y < gridSizeY; y++) {
		allNodes[y].resize(gridSizeX);
		for(int x = 0; x < gridSizeX; x++) {
			allNodes[y][x] = Node(x, y);
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
			costMap[y][x] = costs[y * gridSizeX + x];
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
	return std::abs(nodeA->x - nodeB->x) + std::abs(nodeA->y - nodeB->y);
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

std::vector<Vector2> PathFinding::RetracePath(Node* startNode, Node* endNode)
{
	std::vector<Vector2> path;
	Node* currentNode = endNode;

	while(currentNode != startNode)
	{
		path.push_back({ static_cast<float>(currentNode->x), static_cast<float>(currentNode->y) });
		currentNode = currentNode->parent;
	}
	path.push_back({ static_cast<float>(startNode->x), static_cast<float>(startNode->y) });

	std::reverse(path.begin(), path.end());
	return path;
}

std::vector<Vector2> PathFinding::FindPath(Vector2 start, Vector2 end)
{
	return FindPath(static_cast<int>(start.x), static_cast<int>(start.y),
		static_cast<int>(end.x), static_cast<int>(end.y));
}

std::vector<Vector2> PathFinding::FindPath(int startX, int startY, int endX, int endY)
{
	const float UPHILL_COST_MULTIPLIER = 2.0f;
	const float DOWNHILL_COST_MULTIPLIER = 0.3f;

	for(int y = 0; y < gridSizeY; y++)
	{
		for(int x = 0; x < gridSizeX; x++)
		{
			Node& node = allNodes[y][x];
			node.gCost = 0;
			node.hCost = 0;
			node.parent = nullptr;
		}
	}

	std::vector<Vector2> path;

	if(startX < 0 || startX >= gridSizeX || startY < 0 || startY >= gridSizeY ||
		endX < 0 || endX >= gridSizeX || endY < 0 || endY >= gridSizeY)
	{
		return path;
	}

	if(!IsPositionWalkable(startX, startY) || !IsPositionWalkable(endX, endY))
	{
		return path;
	}


	std::priority_queue<Node*, std::vector<Node*>, NodeComparator> openSet;
	std::vector<std::vector<bool>> closedSet(gridSizeY, std::vector<bool>(gridSizeX, false));

	Node* startNode = &allNodes[startY][startX];
	Node* targetNode = &allNodes[endY][endX];

	openSet.push(startNode);

	while(!openSet.empty())
	{
		Node* currentNode = openSet.top();
		openSet.pop();

		closedSet[currentNode->y][currentNode->x] = true;

		if(currentNode == targetNode) 
		{
			path = RetracePath(startNode, targetNode);
			break;
		}

		for(Node* neighbor : GetNeighbors(currentNode))
		{
			if(closedSet[neighbor->y][neighbor->x])
			{
				continue;
			}

			float newMovementCostToNeighbor = currentNode->gCost;
			newMovementCostToNeighbor += GetManhattanDistance(currentNode, neighbor); 
			newMovementCostToNeighbor += costMap[neighbor->y][neighbor->x]; 

			float heightDiff = heightMap[neighbor->y][neighbor->x] - heightMap[currentNode->y][currentNode->x];

			if(heightDiff > 0)
			{
				newMovementCostToNeighbor += heightDiff * UPHILL_COST_MULTIPLIER;
			}
			else if(heightDiff < 0)
			{
				newMovementCostToNeighbor += std::abs(heightDiff) * DOWNHILL_COST_MULTIPLIER;
			}

			if(newMovementCostToNeighbor < neighbor->gCost || neighbor->gCost == 0) 
			{
				neighbor->gCost = newMovementCostToNeighbor;
				neighbor->hCost = GetManhattanDistance(neighbor, targetNode);
				neighbor->parent = currentNode;
				neighbor->CalculateFCost();

				openSet.push(neighbor);
			}
		}
	}
	return path;
}
