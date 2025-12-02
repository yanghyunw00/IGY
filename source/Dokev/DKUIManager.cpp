/**
 * \file DKUIManager.cpp
 * \brief UI Manager implementation for building selection interface
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */


#include "DKUIManager.hpp"
#include "DKCollision.hpp"
#include "DKShapes.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Path.hpp"
#include "Engine/Window.hpp"
#include "Game/ResourceManager.hpp"
#include "Game/BuildingCostManager.hpp"
#include <cmath>
#include <string>

namespace Dokev
{

    UIManager::UIManager()
    {
    }

    void UIManager::Initialize()
    {
        CreateBuildingButtons();
        woodIcon  = LoadTexture(assets::locate_asset("Assets/resources/tree_1.png").string());
        rockIcon  = LoadTexture(assets::locate_asset("Assets/resources/rock_1.png").string());
        steelIcon = LoadTexture(assets::locate_asset("Assets/resources/steel_1.png").string());
        happyIcon = LoadTexture(assets::locate_asset("Assets/images/textures/smile.png").string());
        humanIcon = LoadTexture(assets::locate_asset("Assets/images/textures/human.png").string());
        outlined  = std::make_unique<CS230::Font>(assets::locate_asset("Assets/fonts/Font_Outlined.png").string());
    }

    void UIManager::CreateBuildingButtons()
    {
        buildingButtons.clear();

        auto windowSize = Engine::GetWindow().GetSize();

        float          startX = static_cast<float>(windowSize.x) - BUTTON_WIDTH - UI_SPACING;
        float          startY = UI_SPACING;
        BuildingButton homeButton;
        homeButton.type     = BuildingType::HOME;
        homeButton.texture  = LoadTexture(assets::locate_asset("Assets/images/textures/home.png").string());
        homeButton.position = Math::fvec2(startX, startY);
        homeButton.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
        homeButton.name     = "Home";
        homeButton.tint     = Dokev::ColorPalette::WHITE;
        buildingButtons.push_back(homeButton);
        BuildingButton woodButton;
        woodButton.type     = BuildingType::WOOD_WORKSHOP;
        woodButton.texture  = LoadTexture(assets::locate_asset("Assets/images/textures/work.png").string());
        woodButton.position = Math::fvec2(startX, startY + (BUTTON_HEIGHT + BUTTON_SPACING));
        woodButton.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
        woodButton.name     = "Wood Workshop";
        woodButton.tint     = Dokev::ColorPalette::BROWN;
        buildingButtons.push_back(woodButton);
        BuildingButton rockButton;
        rockButton.type     = BuildingType::ROCK_WORKSHOP;
        rockButton.texture  = LoadTexture(assets::locate_asset("Assets/images/textures/work.png").string());
        rockButton.position = Math::fvec2(startX, startY + (BUTTON_HEIGHT + BUTTON_SPACING) * 2);
        rockButton.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
        rockButton.name     = "Rock Workshop";
        rockButton.tint     = Dokev::ColorPalette::GRAY;
        buildingButtons.push_back(rockButton);
        BuildingButton steelButton;
        steelButton.type     = BuildingType::STEEL_WORKSHOP;
        steelButton.texture  = LoadTexture(assets::locate_asset("Assets/images/textures/work.png").string());
        steelButton.position = Math::fvec2(startX, startY + (BUTTON_HEIGHT + BUTTON_SPACING) * 3);
        steelButton.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
        steelButton.name     = "Steel Workshop";
        steelButton.tint     = Dokev::ColorPalette::LIGHTGRAY;
        buildingButtons.push_back(steelButton);
        BuildingButton roadButton;
        roadButton.type     = BuildingType::ROAD;
        roadButton.texture  = LoadTexture(assets::locate_asset("Assets/images/textures/road.png").string());
        roadButton.position = Math::fvec2(startX, startY + (BUTTON_HEIGHT + BUTTON_SPACING) * 4);
        roadButton.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
        roadButton.name     = "Road";
        roadButton.tint     = Dokev::ColorPalette::WHITE;
        buildingButtons.push_back(roadButton);

        //Delete Button 
        BuildingButton deleteButton;
        deleteButton.type     = BuildingType::DELETE;
        deleteButton.texture  = LoadTexture(assets::locate_asset("Assets/images/textures/delete.png").string());
        deleteButton.position = Math::fvec2(startX, startY + (BUTTON_HEIGHT + BUTTON_SPACING) * 5);
        deleteButton.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
        deleteButton.name     = "Delete";
        deleteButton.tint     = Dokev::ColorPalette::WHITE;
        buildingButtons.push_back(deleteButton);
    }

    void UIManager::Update(Math::fvec2 mousePos, bool mouseClicked)
    {
        for (auto& button : buildingButtons)
        {
            button.isHovered = PointInRect(mousePos, button.position, button.size);

            if (button.isHovered && mouseClicked)
            {
                // Toggle
                if (selectedBuilding == button.type)
                {
                    selectedBuilding = BuildingType::NONE; //
                }
                else
                {
                    selectedBuilding = button.type;
                }
            }

            button.isSelected = (button.type == selectedBuilding);
        }
    }

    void UIManager::Draw(ResourceManager* resourceMgr, CityMetrics* cm, int humanCount, BuildingCostManager* costMgr) const
    {
        DrawResources(resourceMgr, humanCount);
        DrawHappiness(cm);

        for (const auto& button : buildingButtons)
        {
            DrawBuildingButton(button, resourceMgr, costMgr);
        }
    }

    void UIManager::DrawResources(ResourceManager* resourceMgr, int humanCount) const
    {
        if (!resourceMgr)
            return;

        auto  windowSize = Engine::GetWindow().GetSize();
        float startX     = static_cast<float>(windowSize.x) - 100.0f - UI_SPACING + 32.0f;
        float resourceY  = static_cast<float>(windowSize.y) - 100.0f;

        float size        = 32.0f;
        float height      = size + 3.0f;
        float paddingY    = -10;
        int   iconPaddingY = 15;
        float iconScale = 0.8;

        float currentX = startX;
        float currentY = resourceY;

        if (woodIcon.IsValid())
        {
            woodIcon.DrawEx(static_cast<int>(currentX) - ICON_SPACING, static_cast<int>(currentY) + iconPaddingY, 0.0f, iconScale, Dokev::ColorPalette::WHITE);
        }
        if (outlined)
        {
            std::string woodText = FormatNumber(resourceMgr->totalWood);
            if (auto textTexture = outlined->PrintToTexture(woodText, Dokev::ColorPalette::WHITE))
            {
                textTexture->Draw(Math::TranslationMatrix(Math::vec2{ currentX, currentY - paddingY }) * Math::ScaleMatrix(Math::vec2{ FONT_SCALE,FONT_SCALE }), Dokev::ColorPalette::WHITE);
            }
        }
        currentY -= height;

        if (rockIcon.IsValid())
        {
            rockIcon.DrawEx(static_cast<int>(currentX) - ICON_SPACING, static_cast<int>(currentY) + iconPaddingY, 0.0f, iconScale, Dokev::ColorPalette::WHITE);
        }
        if (outlined)
        {
            std::string stoneText = FormatNumber(resourceMgr->totalStone);
            if (auto textTexture = outlined->PrintToTexture(stoneText, Dokev::ColorPalette::WHITE))
            {
                textTexture->Draw(Math::TranslationMatrix(Math::vec2{ currentX, currentY - paddingY }) * Math::ScaleMatrix(Math::vec2{ FONT_SCALE, FONT_SCALE }), Dokev::ColorPalette::WHITE);
            }
        }
        currentY -= height;

        if (steelIcon.IsValid())
        {
            steelIcon.DrawEx(static_cast<int>(currentX) - ICON_SPACING, static_cast<int>(currentY) + iconPaddingY, 0.0f, iconScale, Dokev::ColorPalette::WHITE);
        }
        if (outlined)
        {
            std::string steelText = FormatNumber(resourceMgr->totalSteel);
            if (auto textTexture = outlined->PrintToTexture(steelText, Dokev::ColorPalette::WHITE))
            {
                textTexture->Draw(Math::TranslationMatrix(Math::vec2{ currentX, currentY - paddingY }) * Math::ScaleMatrix(Math::vec2{ FONT_SCALE, FONT_SCALE }), Dokev::ColorPalette::WHITE);
            }
        }
        currentY -= height + 5.0f;

        if (humanIcon.IsValid())
        {
            humanIcon.DrawEx(static_cast<int>(currentX) - ICON_SPACING, static_cast<int>(currentY) + iconPaddingY, 0.0f, iconScale, Dokev::ColorPalette::WHITE);
        }
        if (outlined)
        {
            std::string humanText = FormatNumber(humanCount);
            if (auto textTexture = outlined->PrintToTexture(humanText, Dokev::ColorPalette::WHITE))
            {
                textTexture->Draw(Math::TranslationMatrix(Math::vec2{ currentX, currentY - paddingY }) * Math::ScaleMatrix(Math::vec2{FONT_SCALE, FONT_SCALE }), Dokev::ColorPalette::WHITE);
            }
        }
    }

    void UIManager::DrawHappiness(CityMetrics* cm) const
    {
        auto  windowSize = Engine::GetWindow().GetSize();
        float startX     = static_cast<float>(windowSize.x) -  100.0f - UI_SPACING + 32.0f;
        float startY     = static_cast<float>(windowSize.y) - 60.0f;
        int iconPaddingY = 15;

        if (happyIcon.IsValid())
        {
            happyIcon.DrawEx(static_cast<int>(startX) - ICON_SPACING, static_cast<int>(startY) + iconPaddingY, 0.0f, 0.6f, Dokev::ColorPalette::WHITE);
        }

        if (outlined)
        {
            std::string happinesstext = std::to_string(cm->GetHappiness());
            if (auto textTexture = outlined->PrintToTexture(happinesstext, Dokev::ColorPalette::WHITE))
            {
                textTexture->Draw(Math::TranslationMatrix(Math::vec2{ startX, startY + 10.0f }) * Math::ScaleMatrix(Math::vec2{ FONT_SCALE, FONT_SCALE }), Dokev::ColorPalette::WHITE);
            }
        }
    }

    void UIManager::DrawBox(Math::fvec2 position, Math::fvec2 size, CS200::RGBA color) const
    {
        DrawRectangle(static_cast<int>(position.x), static_cast<int>(position.y), static_cast<int>(size.x), static_cast<int>(size.y), color);
    }

    void UIManager::DrawBuildingButton(const BuildingButton& button, ResourceManager* resourceMgr, BuildingCostManager* costMgr) const
    {
        CS200::RGBA boxColor = Dokev::ColorPalette::BOX_INSIDE_DEFAULT;

        if (button.isSelected)
        {
            boxColor = Dokev::ColorPalette::BOX_INSIDE_SELECTED;
        }
        else if (button.isHovered)
        {
            boxColor = Dokev::ColorPalette::BOX_INSIDE_HOVERED;
        }

        DrawBox(button.position, button.size, boxColor);

        CS200::RGBA borderColor = button.isSelected ? Dokev::ColorPalette::BOX_OUTLINE_SELECTED : Dokev::ColorPalette::BOX_OUTLINE_DEFAULT;

        DrawRectangleLinesEx(static_cast<int>(button.position.x), static_cast<int>(button.position.y), static_cast<int>(button.size.x), static_cast<int>(button.size.y), 3.0f, borderColor);

        
        //Draw Building Texture.
        if (button.texture.IsValid())
        {

            int   textureWidth  = button.texture.GetWidth();
            int   textureHeight = button.texture.GetHeight();

            float scaledWidth  = textureWidth *  static_cast<float>(ICON_SCALE);
            float scaledHeight = textureHeight * static_cast<float>(ICON_SCALE);

            Math::fvec2 imagePos = Math::fvec2(button.position.x + (button.size.x - scaledWidth) / 2.0f, button.position.y + (button.size.y - scaledHeight) / 2.0f);

            button.texture.DrawEx(static_cast<int>(imagePos.x), static_cast<int>(imagePos.y), 0.0f, static_cast<float>(ICON_SCALE), button.tint);
        }

        // Draw required resources
        if (button.isHovered && (button.type == BuildingType::ROAD || button.type == BuildingType::WOOD_WORKSHOP || button.type == BuildingType::ROCK_WORKSHOP ||
                                 button.type == BuildingType::STEEL_WORKSHOP || button.type == BuildingType::HOME))
        {
            float tooltipWidth  = 120.0f;
            float tooltipHeight = 50.0f;
            if (button.type == BuildingType::WOOD_WORKSHOP || button.type == BuildingType::ROCK_WORKSHOP || button.type == BuildingType::STEEL_WORKSHOP)
                tooltipHeight = 65.0f;
            if (button.type == BuildingType::HOME)
                tooltipHeight = 80.0f;

            Math::fvec2 tooltipPos = Math::fvec2(button.position.x - tooltipWidth - 10.0f, button.position.y);

            DrawBox(tooltipPos, Math::fvec2(tooltipWidth, tooltipHeight), Dokev::ColorPalette::TOOLTIP_BACKGROUND);

            DrawRectangleLinesEx(static_cast<int>(tooltipPos.x), static_cast<int>(tooltipPos.y), static_cast<int>(tooltipWidth), static_cast<int>(tooltipHeight), 2.0f, Dokev::ColorPalette::LIGHTGRAY);

            if (outlined && resourceMgr && costMgr)
            {
                // Get cost from JSON
                BuildingCost cost = costMgr->GetCost(button.type);

                // building name
                float nameYPos = 25.0f;
                int resourceCount = (cost.wood > 0 ? 1 : 0) + (cost.stone > 0 ? 1 : 0) + (cost.steel > 0 ? 1 : 0);
                if (resourceCount == 2) nameYPos = 40.0f;
                if (resourceCount == 3) nameYPos = 55.0f;

                if (auto nameTexture = outlined->PrintToTexture(button.name, Dokev::ColorPalette::WHITE))
                {
                    nameTexture->Draw(Math::TranslationMatrix(Math::vec2{ tooltipPos.x + 10.0f, tooltipPos.y + nameYPos }) * Math::ScaleMatrix(Math::vec2{ 0.35, 0.35 }), Dokev::ColorPalette::WHITE);
                }

                // Draw resource requirements dynamically
                float yOffset = 5.0f;
                if (cost.wood > 0)
                {
                    std::string woodText = "Wood: " + std::to_string(resourceMgr->totalWood) + " / " + std::to_string(cost.wood);
                    if (auto textTexture = outlined->PrintToTexture(woodText, Dokev::ColorPalette::WHITE))
                    {
                        textTexture->Draw(Math::TranslationMatrix(Math::vec2{tooltipPos.x + 10.0f, tooltipPos.y + yOffset}) *
                                        Math::ScaleMatrix(Math::vec2{0.3, 0.3}), Dokev::ColorPalette::WHITE);
                    }
                    yOffset += 15.0f;
                }
                if (cost.stone > 0)
                {
                    std::string rockText = "Rock: " + std::to_string(resourceMgr->totalStone) + " / " + std::to_string(cost.stone);
                    if (auto textTexture = outlined->PrintToTexture(rockText, Dokev::ColorPalette::WHITE))
                    {
                        textTexture->Draw(Math::TranslationMatrix(Math::vec2{tooltipPos.x + 10.0f, tooltipPos.y + yOffset}) *
                                        Math::ScaleMatrix(Math::vec2{0.3, 0.3}), Dokev::ColorPalette::WHITE);
                    }
                    yOffset += 15.0f;
                }
                if (cost.steel > 0)
                {
                    std::string steelText = "Steel: " + std::to_string(resourceMgr->totalSteel) + " / " + std::to_string(cost.steel);
                    if (auto textTexture = outlined->PrintToTexture(steelText, Dokev::ColorPalette::WHITE))
                    {
                        textTexture->Draw(Math::TranslationMatrix(Math::vec2{tooltipPos.x + 10.0f, tooltipPos.y + yOffset}) *
                                        Math::ScaleMatrix(Math::vec2{0.3, 0.3}), Dokev::ColorPalette::WHITE);
                    }
                }
            }
        }
    }

    std::string UIManager::FormatNumber(int value) const
    {
        if (value >= 1000)
        {
            float displayValue = value / 1000.0f;
            char buffer[32];
            std::snprintf(buffer, sizeof(buffer), "%.1fk", displayValue);
            return std::string(buffer);
        }
        return std::to_string(value);
    }
}
