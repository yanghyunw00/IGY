#pragma once
#include "GameObjectManager.hpp"
#include "GridMap.hpp"
#include "Component.hpp"
#include "Dokev/DKShapes.hpp"

class RenderSystem
{
public:
    void Draw(GameObjectManager* goManager, int viewIndex, const GridMap* map) const
    {
        for (auto& obj : goManager->GetGameObjects())
        {
            if (!obj->IsActive() || !obj->HasComponent<TransformComponent>() || !obj->HasComponent<RenderComponent>())
            {
                continue;
            }

            auto transform = obj->GetComponent<TransformComponent>();
            auto render = obj->GetComponent<RenderComponent>();

            Math::fvec2 transformedPos = map->TransformPathPointForView(transform->position, viewIndex);
            
            int gridX_int = static_cast<int>(transformedPos.x);
            int gridY_int = static_cast<int>(transformedPos.y);

            if (gridX_int < 0 || gridX_int >= GRID_SIZE_X || gridY_int < 0 || gridY_int >= GRID_SIZE_Y)
                continue;

            GridCell** viewArray = map->GetViewArray(viewIndex);
            float height = static_cast<float>(viewArray[gridY_int * GRID_SIZE_X + gridX_int]->getHeight());

            int originX = ((TILE_WIDTH / 2) * GRID_SIZE_X) - TILE_WIDTH / 2;
            int originY = 0;

            int screenX = originX + static_cast<int>((transformedPos.x - transformedPos.y) * TILE_WIDTH_HALF);
            int screenY = originY + static_cast<int>((transformedPos.x + transformedPos.y) * TILE_HEIGHT_HALF);
            int topY    = static_cast<int>(height) - screenY;

            int drawX = screenX + TILE_WIDTH_HALF;
            int drawY = topY;

            Dokev::DrawCircle(drawX, drawY, render->radius, render->color);
        }
    }
};