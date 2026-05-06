#include "Earthquake.hpp"
#include "Dokev/Core/DKUtility.hpp"
#include "Game/Entities/Building.hpp"
#include "Game/Managers/DKCityMetrics.hpp"
#include "Game/World/GridMap.hpp"
#include "Engine/Path.hpp"

void Earthquake::OnStart()
{
    cx = Dokev::GetRandomInt(0, GRID_SIZE_X - 1);
    cy = Dokev::GetRandomInt(0, GRID_SIZE_Y - 1);

    // sprite = Dokev::AnimatedSprite(assets::locate_asset("Assets/images/meteor.spt"));
    // sprite.PlayAnimation("meteor");

    struct RemovedBuilding { Math::ivec2 pos; int w; int h; };
    std::vector<RemovedBuilding> removeRange;
    for (const auto& b : gameObjectManager->GetBuildings())
    {
        auto  pos = b->GetGrid();
        float dx  = pos.x - cx;
        float dy  = pos.y - cy;
        if (dx * dx + dy * dy <= radius * radius)
            removeRange.push_back({ { static_cast<int>(pos.x), static_cast<int>(pos.y) }, b->GetWidth(), b->GetHeight() });
    }
    for (auto& item : removeRange)
    {
        gameObjectManager->RemoveBuilding(item.pos.x, item.pos.y);
        if (gridMap) gridMap->GetTileManager()->DeallocateArea(item.pos.x, item.pos.y, item.w, item.h);
    }

    cityMetrics->GetImmigrationManager()->Block();
}

void Earthquake::OnEnd()
{
    cityMetrics->GetImmigrationManager()->Free();
}

void Earthquake::Draw() const
{
    // sprite.DrawAtPosition(drawX, drawY);

    if (!gridMap) return;

    GridCell** viewArray = gridMap->GetViewArray(0);
    if (!viewArray) return;

    GridCell* cell = viewArray[cy * GRID_SIZE_X + cx];
    if (!cell) return;

    int originX = gridMap->GetIsoOriginX();
    int originY = gridMap->GetIsoOriginY();
    int screenX = originX + (cx - cy) * TILE_WIDTH_HALF;
    int screenY = originY + (cx + cy) * TILE_HEIGHT_HALF;
    int topY    = static_cast<int>(cell->getHeight()) - screenY;
    int centerX = screenX + TILE_WIDTH_HALF;
    int centerY = topY + TILE_HEIGHT * 2;

    if (!debugDraw) return;

    int halfW = static_cast<int>(radius * 1.41421f * TILE_WIDTH_HALF);
    int halfH = static_cast<int>(radius * 1.41421f * TILE_HEIGHT_HALF);

    Dokev::DrawRectangleLinesEx(centerX - halfW, centerY - halfH,
                                halfW * 2, halfH * 2, 6.0f, 0xFF0000FF);
}
