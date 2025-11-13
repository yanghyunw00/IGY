/**
 * \file DKUIManager.hpp
 * \brief UI Manager for building selection interface
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once

#include "DKTexture.hpp"
#include "Engine/Vec2.hpp"
#include "CS200/RGBA.hpp"
#include "Game/BuildingType.hpp"
#include <vector>
#include <string>

class ResourceManager;

namespace Dokev
{
    struct BuildingButton
    {
        BuildingType type;
        Texture2D texture;
        Math::fvec2 position;
        Math::fvec2 size;
        std::string name;
        CS200::RGBA tint = CS200::WHITE;
        bool isHovered = false;
        bool isSelected = false;
    };

    class UIManager
    {
    public:
        UIManager();

        void Initialize();
        void Update(Math::fvec2 mousePos, bool mouseClicked);
        void Draw(ResourceManager* resourceMgr) const;

        BuildingType GetSelectedBuilding() const { return selectedBuilding; }
        void SetSelectedBuilding(BuildingType type) { selectedBuilding = type; }

    private:
        void CreateBuildingButtons();
        void DrawBox(Math::fvec2 position, Math::fvec2 size, CS200::RGBA color) const;
        void DrawBuildingButton(const BuildingButton& button) const;
        void DrawResources(ResourceManager* resourceMgr) const;

        std::vector<BuildingButton> buildingButtons;
        BuildingType selectedBuilding = BuildingType::NONE;

        Texture2D woodIcon;
        Texture2D rockIcon;
        Texture2D steelIcon;

        // UI Layout constants
        static constexpr float BUTTON_WIDTH = 100.0f;
        static constexpr float BUTTON_HEIGHT = 100.0f;
        static constexpr float BUTTON_SPACING = 10.0f;
        static constexpr float UI_MARGIN = 20.0f;
    };
}
