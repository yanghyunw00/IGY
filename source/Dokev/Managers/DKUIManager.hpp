/**
 * \file DKUIManager.hpp
 * \brief UI Manager for building selection interface
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once

#include "../Core/DKColor.hpp"
#include "../Graphics/DKTexture.hpp"
#include "CS200/RGBA.hpp"
#include "Engine/Font.hpp"
#include "Engine/Vec2.hpp"
#include "Game/Common/BuildingType.hpp"
#include "Game/Managers/DKCityMetrics.hpp"
#include <string>
#include <vector>


class ResourceManager;
class BuildingCostManager;
class Dokev::CityMetrics;

namespace Dokev
{
    enum class MenuState
    {
        MAIN,
        CATEGORY,
        BUILDING
    };

    enum class CategoryType
    {
        NONE,
        CONSTRUCTION,
        HOME_CATEGORY,
        WORK_CATEGORY,
        ROAD_CATEGORY,
        DELETE_CATEGORY
    };

    struct BuildingButton
    {
        BuildingType type;
        Texture2D    texture;
        Math::fvec2  position;
        Math::fvec2  size;
        std::string  name;
        CS200::RGBA  tint       = Dokev::ColorPalette::WHITE;
        bool         isHovered  = false;
        bool         isSelected = false;
        float        hoverScale = 1.0f;
    };

    class UIManager
    {
    public:
        UIManager();

        void Initialize();
        void Update(Math::fvec2 mousePos, bool mouseClicked, float dt = 0.016f);

        void Draw(ResourceManager* resourceMgr, CityMetrics* cm, int humanCount, BuildingCostManager* costMgr = nullptr) const;

        BuildingType GetSelectedBuilding() const
        {
            return selectedBuilding;
        }

        void SetSelectedBuilding(BuildingType type)
        {
            selectedBuilding = type;
        }

        bool IsMenuActive() const
        {
            return currentMenuState != MenuState::MAIN;
        }

        void OpenBuildingCategory(CategoryType category)
        {
            currentMenuState = MenuState::BUILDING;
            currentCategory  = category;
            CreateCategoryButtons();
            CreateBuildingButtons(category);
        }

    private:
        void        CreateMainButtons();
        void        CreateCategoryButtons();
        void        CreateBuildingButtons(CategoryType category);
        void        UpdateMainMenu(Math::fvec2 mousePos, bool mouseClicked);
        void        UpdateCategoryMenu(Math::fvec2 mousePos, bool mouseClicked);
        void        UpdateBuildingMenu(Math::fvec2 mousePos, bool mouseClicked);
        void        DrawBox(Math::fvec2 position, Math::fvec2 size, CS200::RGBA color) const;
        void        DrawBuildingButton(const BuildingButton& button, ResourceManager* resourceMgr, BuildingCostManager* costMgr, float time) const;
        void        DrawResources(ResourceManager* resourceMgr, int humanCount) const;
        void        DrawHappiness(CityMetrics* cm) const;
        std::string FormatNumber(int value) const;
        std::string GetHotkeyForButton(const BuildingButton& button) const;

        float        totalTime        = 0.0f;
        MenuState    currentMenuState = MenuState::MAIN;
        CategoryType currentCategory  = CategoryType::NONE;

        std::vector<BuildingButton> mainButtons;
        std::vector<BuildingButton> categoryButtons;
        std::vector<BuildingButton> buildingButtons;
        BuildingType                selectedBuilding = BuildingType::NONE;

        Texture2D woodIcon;
        Texture2D rockIcon;
        Texture2D steelIcon;
        Texture2D happyIcon;
        Texture2D humanIcon;
        Texture2D deleteIcon;

        std::unique_ptr<CS230::Font> outlined;

        static constexpr float  BUTTON_WIDTH   = 60.0f;
        static constexpr float  BUTTON_HEIGHT  = 60.0f;
        static constexpr float  BUTTON_SPACING = 10.0f;
        static constexpr float  UI_SPACING     = 20.0f; // ui spacing, between draw ui stuff
        static constexpr double FONT_SCALE     = 0.7;   // for font scaling
        static constexpr double ICON_SCALE     = 0.8;   // for icon scaling
        static constexpr int    ICON_SPACING   = 40;    // when draw icon, startX - icon spacing
    };
}
