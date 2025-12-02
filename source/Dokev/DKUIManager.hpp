/**
 * \file DKUIManager.hpp
 * \brief UI Manager for building selection interface
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once

#include "DKTexture.hpp"
#include "DKCityMetrics.hpp"
#include "DKColor.hpp"
#include "Engine/Vec2.hpp"
#include "Engine/Font.hpp"
#include "CS200/RGBA.hpp"
#include "Game/BuildingType.hpp"
#include <vector>
#include <string>


class ResourceManager;
class BuildingCostManager;
class Dokev::CityMetrics;

namespace Dokev
{
    struct BuildingButton
    {
        BuildingType type;
        Texture2D texture;
        Math::fvec2 position;
        Math::fvec2 size;
        std::string name;
        CS200::RGBA tint = Dokev::ColorPalette::WHITE;
        bool isHovered = false;
        bool isSelected = false;
    };

    class UIManager
    {
    public:
        UIManager();

        void Initialize();
        void Update(Math::fvec2 mousePos, bool mouseClicked);

        void Draw(ResourceManager* resourceMgr, CityMetrics* cm, int humanCount, BuildingCostManager* costMgr = nullptr) const;

        BuildingType GetSelectedBuilding() const { return selectedBuilding; }
        void SetSelectedBuilding(BuildingType type) { selectedBuilding = type; }

    private:
        void CreateBuildingButtons();
        void DrawBox(Math::fvec2 position, Math::fvec2 size, CS200::RGBA color) const;
        void DrawBuildingButton(const BuildingButton& button, ResourceManager* resourceMgr, BuildingCostManager* costMgr) const;
        void DrawResources(ResourceManager* resourceMgr, int humanCount) const;
        void DrawHappiness(CityMetrics* cm) const;
        std::string FormatNumber(int value) const;

        std::vector<BuildingButton> buildingButtons;
        BuildingType selectedBuilding = BuildingType::NONE;

        Texture2D woodIcon;
        Texture2D rockIcon;
        Texture2D steelIcon;
        Texture2D happyIcon;
        Texture2D humanIcon;
        Texture2D deleteIcon;
        
        std::unique_ptr<CS230::Font> outlined;

        static constexpr float BUTTON_WIDTH = 60.0f;
        static constexpr float BUTTON_HEIGHT = 60.0f;
        static constexpr float BUTTON_SPACING = 10.0f;
        static constexpr float UI_SPACING = 20.0f; //ui spacing, between draw ui sfuff
        static constexpr double FONT_SCALE = 0.7;  // for font scaling
        static constexpr double ICON_SCALE = 0.8;  // for icon scaling
        static constexpr int ICON_SPACING = 40; // when draw icon, startx - icon spacing 

    };
}
