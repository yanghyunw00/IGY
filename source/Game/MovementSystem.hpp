#pragma once
#include "GameObjectManager.hpp"
#include "GridMap.hpp"
#include "Component.hpp"

class MovementSystem
{
public:
    void Update(GameObjectManager* goManager, float deltaTime, GridMap* map)
    {
        for (auto& obj : goManager->GetGameObjects())
        {
            if (!obj->IsActive() || !obj->HasComponent<TransformComponent>() || !obj->HasComponent<PathComponent>())
            {
                continue;
            }

            auto transform = obj->GetComponent<TransformComponent>();
            auto path = obj->GetComponent<PathComponent>();
            auto ai = obj->GetComponent<AIComponent>();

            if (!path->isMoving)
            {
                continue;
            }
            
            path->moveTimer += deltaTime;
            float t = path->moveTimer / path->moveDelay;
            if (t > 1.0f) t = 1.0f;

            transform->position.x = path->startPosition.x + (path->targetGridPos.x - path->startPosition.x) * t;
            transform->position.y = path->startPosition.y + (path->targetGridPos.y - path->startPosition.y) * t;

            if (t >= 1.0f)
            {
                transform->position = path->targetGridPos;
                transform->gridPosition = path->targetGridPos;

                path->currentPathIndex++;
                if (path->currentPathIndex >= path->path.size())
                {
                    path->isMoving = false;
                    path->path.clear();
                    if (ai) ai->state = HumanState::Idle;
                }
                else
                {
                    path->startPosition = transform->gridPosition;
                    path->targetGridPos = path->path[path->currentPathIndex];
                    path->moveTimer = 0.0f;
                }
            }
        }
    }
};