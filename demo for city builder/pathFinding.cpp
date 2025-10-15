#include "PathFinding.h"
#include "GridCell.h" 
#include <algorithm>
#include <iostream>
#include <limits>

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
	const float INF = std::numeric_limits<float>::infinity(); 

	for(int y = 0; y < gridSizeY; y++)
	{
		for(int x = 0; x < gridSizeX; x++)
		{
			Node& node = allNodes[y][x];
			node.gCost = INF;
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
			path = RetracePath(startNode, targetNode);
			break;
		}

		for(Node* neighbor : GetNeighbors(currentNode))
		{

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
	return path;
}


//BFS 탐색
//Vector2 FindNearestGround(const Map& map, Vector2 inputPos) 
//{
//    int startX = (int)(inputPos.x / TILE_SIZE_W);
//    int startY = (int)(inputPos.y / TILE_SIZE_H);
//
//    if (map.IsWalkable(startY, startX))
//    {
//        return { startX * (float)TILE_SIZE_W, startY * (float)TILE_SIZE_H };
//    }
//
//
//    //BFS 알고리즘
//    std::queue<std::pair<int, int>> q;
//    std::unordered_map<int, bool> visited; //방문한 곳을 기록
//
//    //2차원을 1차원으로 나열
//    auto key = [&](int x, int y) { return y * MAP_WIDTH + x; };
//
//    q.push({ startX, startY });
//    visited[key(startX, startY)] = true;
//
//    //8방향의 타일을 확인
//    int dir[8][2] = {
//    {1, 0},   //  오른쪽
//    {-1, 0},  //  왼쪽
//    {0, 1},   //  아래
//    {0, -1},  //  위
//    {1, 1},   //  오른쪽 아래
//    {1, -1},  //  오른쪽 위
//    {-1, 1},  //  왼쪽 아래
//    {-1, -1}  //  왼쪽 위
//    };
//
//    while (!q.empty()) {
//        //젤 앞에를 꺼내서 확인
//        auto [cx, cy] = q.front();
//        q.pop();
//
//        for (auto& d : dir) 
//        {
//            //4방향 순회
//            int nx = cx + d[0];
//            int ny = cy + d[1];
//
//            //맵 범위 밖, 방문한 노드 continue;
//            if (nx < 0 || nx >= MAP_WIDTH || ny < 0 || ny >= MAP_HEIGHT) continue;
//            if (visited[key(nx, ny)]) continue;
//
//            visited[key(nx, ny)] = true;
//
//            if (map.IsWalkable(ny, nx))
//            {
//                return { nx * (float)TILE_SIZE_W, ny * (float)TILE_SIZE_H };
//            }
//
//            q.push({ nx, ny });
//        }
//    }
//
//    //없다면 그냥 반환(4방향이 막혔을 경우
//    return { startX * (float)TILE_SIZE_W, startY * (float)TILE_SIZE_H };
//}
//

