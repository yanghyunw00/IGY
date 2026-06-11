/**
 * \file DKMouseSelection.hpp
 * \brief
 * \author
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include "Dokev/Graphics/DKTexture.hpp"
#include "Engine/Vec2.hpp"


// Forward declarations
class GridMap;
struct GridCell;

namespace Dokev
{

    class MouseSelection
    {
    public:
        MouseSelection() : position{ -1.0f, -1.0f }, map(nullptr), currentViewIndex(0), hasValidPosition(false), isActive(false)
        {
        }

        void SetGridMap(GridMap* gridMap)
        {
            map = gridMap;
        }

        void SetViewIndex(int viewIndex)
        {
            currentViewIndex = viewIndex;
        }

        void Load(const std::filesystem::path& texturePath, const std::filesystem::path& invalidtexturePath);
        void Update(float x, float y);

        void SetHasValidPosition(bool isValid)
        {
            hasValidPosition = isValid;
        }

        void SetActive(bool active)
        {
            isActive = active;
        }

        void Draw() const;

        bool ShouldDrawAtTile(int tileX, int tileY) const;
        void DrawAtTile(int screenX, int topY) const;

        Math::fvec2 GetGridPosition() const
        {
            return position;
        }

        bool HasValidPosition() const
        {
            return hasValidPosition;
        }

    private:
        Math::fvec2 position;
        GridMap*    map;
        int         currentViewIndex;
        Texture2D   selectionTexture;
        Texture2D   invalidSelectionTexture;
        bool        hasValidPosition;
        bool        isActive;
    };
}
