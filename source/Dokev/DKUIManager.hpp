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

        void Draw(ResourceManager* resourceMgr, CityMetrics* cm, BuildingCostManager* costMgr = nullptr) const;

        BuildingType GetSelectedBuilding() const { return selectedBuilding; }
        void SetSelectedBuilding(BuildingType type) { selectedBuilding = type; }

    private:
        void CreateBuildingButtons();
        void DrawBox(Math::fvec2 position, Math::fvec2 size, CS200::RGBA color) const;
        void DrawBuildingButton(const BuildingButton& button, ResourceManager* resourceMgr, BuildingCostManager* costMgr) const;
        void DrawResources(ResourceManager* resourceMgr) const;
        void DrawHappiness(CityMetrics* cm) const;

        std::vector<BuildingButton> buildingButtons;
        BuildingType selectedBuilding = BuildingType::NONE;

        Texture2D woodIcon;
        Texture2D rockIcon;
        Texture2D steelIcon;
        Texture2D happyIcon;
        
        std::unique_ptr<CS230::Font> outlined;

        static constexpr float BUTTON_WIDTH = 100.0f;
        static constexpr float BUTTON_HEIGHT = 100.0f;
        static constexpr float BUTTON_SPACING = 10.0f;
        static constexpr float UI_SPACING = 20.0f;
    };
}
