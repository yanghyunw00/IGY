#include "DKMouseSelection.hpp"
#include "Dokev/DKColor.hpp"
#include "Dokev/DKShapes.hpp"
#include "Game/GridCell.hpp"
#include "Game/GridMap.hpp"
#include <algorithm>
#include <cmath>

namespace Dokev
{
    void MouseSelection::Load(const std::filesystem::path& texturePath, const std::filesystem::path& invalidtexturePath)
    {
        selectionTexture        = Dokev::LoadTexture(texturePath.string());
        invalidSelectionTexture = Dokev::LoadTexture(invalidtexturePath.string());
    }

    void MouseSelection::Update(float x, float y)
    {
        position.x = x;
        position.y = y;
    }

    void MouseSelection::Draw() const
    {
        if (!map || !hasValidPosition)
        {
            return;
        }

        int x = static_cast<int>(std::round(position.x));
        int y = static_cast<int>(std::round(position.y));

        if (x < 0 || x >= GRID_SIZE_X || y < 0 || y >= GRID_SIZE_Y)
        {
            return;
        }

        GridCell** selectedView = map->GetViewArray(currentViewIndex);
        if (!selectedView)
        {
            return;
        }

        int originX = 0;
        int originY = 0;

        int screenX = originX + (x - y) * TILE_WIDTH_HALF;
        int screenY = originY + (x + y) * TILE_HEIGHT_HALF;

        int    count  = y * GRID_SIZE_X + x;
        double height = selectedView[count]->getHeight();
        int    topY   = static_cast<int>(height) - screenY;

        if (selectionTexture.IsValid())
        {
            selectionTexture.Draw(screenX, topY);
        }
        if (!hasValidPosition)
        {
            invalidSelectionTexture.Draw(screenX, topY);
        }
    }

    bool MouseSelection::ShouldDrawAtTile(int tileX, int tileY) const
    {


        int x = static_cast<int>(std::round(position.x));
        int y = static_cast<int>(std::round(position.y));

        return (x == tileX && y == tileY);
    }

    void MouseSelection::DrawAtTile(int screenX, int topY) const
    {
        if (selectionTexture.IsValid())
        {
            selectionTexture.Draw(screenX, topY);
        }

        if (!hasValidPosition)
        {
            invalidSelectionTexture.Draw(screenX, topY);
        }
    }
}
