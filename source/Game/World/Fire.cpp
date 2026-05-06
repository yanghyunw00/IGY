#include "Fire.hpp"
#include "Dokev/Core/DKUtility.hpp"
#include "Game/Entities/Building.hpp"
#include "Game/World/GridMap.hpp"
#include "Engine/Path.hpp"
#include <algorithm>

Dokev::Texture2D Fire::fireTexture;

void Fire::OnStart()
{
    if (!fireTexture.IsValid())
        fireTexture = Dokev::LoadTexture(assets::locate_asset("Assets/images/textures/fire.png").string());

    cx = Dokev::GetRandomInt(0, GRID_SIZE_X - 1);
    cy = Dokev::GetRandomInt(0, GRID_SIZE_Y - 1);

    for (const auto& b : gameObjectManager->GetBuildings())
    {
        auto  pos = b->GetGrid();
        float dx  = pos.x - cx;
        float dy  = pos.y - cy;

        if (dx * dx + dy * dy <= radius * radius)
        {
            fireRange.push_back(b.get());
        }
    }
}

void Fire::OnTick(double dt)
{
    elapsed     += dt;
    damageTimer += dt;

    if (damageTimer < damageInterval) return;

    damageTimer = 0;

    fireRange.erase(
        std::remove_if(fireRange.begin(), fireRange.end(),
            [](Building* b)
            { 
                return b->GetHp() <= 0;
            }),
        fireRange.end());

    for (auto* b : fireRange)
    {
        if (Dokev::GetRandomInt(0, 1) == 0)
        {
            b->setHP(b->GetHp() - fireDamage);
            if (b->GetHp() <= 0)
            {
                auto pos = b->GetGrid();
                int  x   = static_cast<int>(pos.x);
                int  y   = static_cast<int>(pos.y);
                int  w   = b->GetWidth();
                int  h   = b->GetHeight();
                gameObjectManager->RemoveBuilding(x, y);
                if (gridMap) gridMap->GetTileManager()->DeallocateArea(x, y, w, h);
            }
        }
    }
}

void Fire::OnEnd()
{
}

bool Fire::IsFinished() const
{
    return elapsed >= maxDuration;
}

void Fire::Draw() const
{
    if (gridMap)
    {
        GridCell** viewArray = gridMap->GetViewArray(0);
        if (viewArray)
        {
            GridCell* cell = viewArray[cy * GRID_SIZE_X + cx];
            if (cell)
            {
                int originX = gridMap->GetIsoOriginX();
                int originY = gridMap->GetIsoOriginY();
                int screenX = originX + (cx - cy) * TILE_WIDTH_HALF;
                int screenY = originY + (cx + cy) * TILE_HEIGHT_HALF;
                int topY    = static_cast<int>(cell->getHeight()) - screenY;
                int centerX = screenX + TILE_WIDTH_HALF;
                int centerY = topY + TILE_HEIGHT * 2;

                if (debugDraw)
                {
                    int halfW = static_cast<int>(radius * 1.41421f * TILE_WIDTH_HALF);
                    int halfH = static_cast<int>(radius * 1.41421f * TILE_HEIGHT_HALF);

                    Dokev::DrawRectangleLinesEx(centerX - halfW, centerY - halfH,
                                                halfW * 2, halfH * 2, 6.0f, 0xFF0000FF);
                }
            }
        }
    }

    if (!fireTexture.IsValid() || !gridMap) return;

    for (auto* b : fireRange)
    {
        auto pos = b->CalculateScreenPosition(0, gridMap);
        fireTexture.Draw(pos.x, pos.y);
    }
}
