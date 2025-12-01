#include "DKMouseSelection.hpp"
#include "Dokev/DKColor.hpp"
#include "Dokev/DKShapes.hpp"


namespace Dokev
{
    void MouseSelection::Update(float x, float y)
    {
        position.x = x;
        position.y = y;
    }

    void MouseSelection::Draw() const
    {
        // int screenX = (position.x - position.y) * TILE_WIDTH_HALF;
        // int screenY = (position.x + position.y) * TILE_HEIGHT_HALF;
        // GridCell* const* cell = map->GetSelectedView();
        // int count = map->GetCount();

        // double height = cell[count]->getHeight();
        // int    topY   = static_cast<int>(height) - screenY;

        //DrawRectangle(screenX, screenY, GRID_SIZE_X, GRID_SIZE_Y, Dokev::ColorPalette::BLACK);
    }
}
