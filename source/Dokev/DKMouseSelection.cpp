#include "DKMouseSelection.hpp"
#include "Dokev/DKColor.hpp"
#include "Dokev/DKShapes.hpp"
#include "Game/GridCell.hpp"
#include "Game/GridMap.hpp"
#include <algorithm>
#include <cmath>

namespace Dokev
{
    void MouseSelection::Load(const std::filesystem::path& texturePath)
    {
        selectionTexture = Dokev::LoadTexture(texturePath.string());
    }

    void MouseSelection::Update(float x, float y)
    {
        position.x = x;
        position.y = y;
        hasValidPosition = true;
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

        int count = y * GRID_SIZE_X + x;
        double height = selectedView[count]->getHeight();
        int topY = static_cast<int>(height) - screenY;

        if (selectionTexture.IsValid())
        {
            selectionTexture.Draw(screenX, topY);
        }
        else
        {
            CS200::RGBA fillColor = ColorAlpha(Dokev::ColorPalette::YELLOW, 0.3f);
            DrawRectangle(screenX, topY, TILE_WIDTH, TILE_HEIGHT, fillColor);
            DrawRectangleLinesEx(screenX, topY, TILE_WIDTH, TILE_HEIGHT, 2.0f, Dokev::ColorPalette::YELLOW);
        }
    }

    bool MouseSelection::ShouldDrawAtTile(int tileX, int tileY) const
    {
        if (!hasValidPosition)
        {
            return false;
        }

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
        else
        {
            CS200::RGBA fillColor = ColorAlpha(Dokev::ColorPalette::YELLOW, 0.3f);
            DrawRectangle(screenX, topY, TILE_WIDTH, TILE_HEIGHT, fillColor);
            DrawRectangleLinesEx(screenX, topY, TILE_WIDTH, TILE_HEIGHT, 2.0f, Dokev::ColorPalette::YELLOW);
        }
    }
}
