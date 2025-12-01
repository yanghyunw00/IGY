#pragma once
#include "Engine/Vec2.hpp"
#include "Dokev/DKTexture.hpp"

// Forward declarations
class GridMap;
struct GridCell;

namespace Dokev
{

    class MouseSelection
    {
    public:
        MouseSelection() : position{-1.0f, -1.0f}, map(nullptr), currentViewIndex(0), hasValidPosition(false) {}

        void SetGridMap(GridMap* gridMap) { map = gridMap; }
        void SetViewIndex(int viewIndex) { currentViewIndex = viewIndex; }

        void Load(const std::filesystem::path& texturePath);
        void Update(float x, float y);
        void Draw() const;

        bool ShouldDrawAtTile(int tileX, int tileY) const;
        void DrawAtTile(int screenX, int topY) const;

    private:
        Math::fvec2 position;
        GridMap* map;
        int currentViewIndex;
        Texture2D selectionTexture;
        bool hasValidPosition;

    };
}