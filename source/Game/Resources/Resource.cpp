#include "Resource.hpp"
#include "../World/GridMap.hpp"
#include "Dokev/Core/DKColor.hpp"
#include "Dokev/Core/DKUtility.hpp"

Resource::Resource(int x, int y, int amount, ResourceType resourceType) : gridX(x), gridY(y), currentAmount(amount), maxAmount(amount), type(resourceType), isOccupied(false)
{
}

int Resource::Mine(int amount)
{
    if (currentAmount <= 0)
        return 0;

    int mined = std::min(amount, currentAmount);
    currentAmount -= mined;
    return mined;
}

void Resource::Draw(int viewIndex, const GridMap* map) const
{
    if (!map || IsDepleted())
        return;

    Math::fvec2 transformedPos = map->TransformPathPointForView(Math::fvec2(static_cast<float>(gridX), static_cast<float>(gridY)), viewIndex);

    int gridX_int = static_cast<int>(transformedPos.x);
    int gridY_int = static_cast<int>(transformedPos.y);

    if (gridX_int < 0 || gridX_int >= GRID_SIZE_X || gridY_int < 0 || gridY_int >= GRID_SIZE_Y)
        return;

    GridCell** viewArray = map->GetViewArray(viewIndex);
    if (!viewArray)
        return;

    GridCell* cell = viewArray[gridY_int * GRID_SIZE_X + gridX_int];
    if (!cell)
        return;

    float height = static_cast<float>(cell->getHeight());

    int originX = map->GetIsoOriginX();
    int originY = map->GetIsoOriginY();

    int screenX = originX + static_cast<int>((transformedPos.x - transformedPos.y) * TILE_WIDTH_HALF);
    int screenY = originY + static_cast<int>((transformedPos.x + transformedPos.y) * TILE_HEIGHT_HALF);
    int topY    = static_cast<int>(height) - screenY;

    int drawX = screenX;
    int drawY = topY;


    Dokev::DrawTexture(texture, drawX, drawY, Dokev::ColorPalette::WHITE);
}