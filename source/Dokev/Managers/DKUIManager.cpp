/**
 * \file DKUIManager.cpp
 * \brief UI Manager implementation for building selection interface
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */


#include "DKUIManager.hpp"
#include "../Graphics/DKShapes.hpp"
#include "../World/DKCollision.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Input.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Path.hpp"
#include "Engine/Window.hpp"
#include "Game/Managers/BuildingCostManager.hpp"
#include "Game/Managers/ResourceManager.hpp"
#include <cmath>
#include <string>


namespace
{
    constexpr int kGranaryWarningThresholdPercent = 90;
}

namespace Dokev
{

    UIManager::UIManager()
    {
    }

    void UIManager::Initialize()
    {
        CreateMainButtons();
        happyIcon = LoadTexture(assets::locate_asset("Assets/images/textures/smile.png").string());
        humanIcon = LoadTexture(assets::locate_asset("Assets/images/textures/human.png").string());
        outlined  = std::make_unique<CS230::Font>(assets::locate_asset("Assets/fonts/DungGeunMo.ttf"), 40);
    }

    void UIManager::CreateBuildingButtons(CategoryType category)
    {
        buildingButtons.clear();

        auto windowSize = Engine::GetWindow().GetSize();

        float totalWidth = BUTTON_WIDTH * 4 + BUTTON_SPACING * 3;
        float startX     = (static_cast<float>(windowSize.x) - totalWidth) / 2.0f;

        switch (category)
        {
            case CategoryType::HOME_CATEGORY:
                {
                    float startY  = UI_SPACING + (BUTTON_HEIGHT + BUTTON_SPACING) * 2;
                    float offsetX = startX + (BUTTON_WIDTH + BUTTON_SPACING) * 4;

                    BuildingButton cancelButton;
                    cancelButton.type     = BuildingType::NONE;
                    cancelButton.texture  = LoadTexture(assets::locate_asset("Assets/images/textures/delete.png").string());
                    cancelButton.position = Math::fvec2(offsetX, startY);
                    cancelButton.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
                    cancelButton.name     = "Cancel";
                    cancelButton.tint     = Dokev::ColorPalette::RED;
                    buildingButtons.push_back(cancelButton);

                    BuildingButton homeButton;
                    homeButton.type     = BuildingType::HOME;
                    homeButton.texture  = LoadTexture(assets::locate_asset("Assets/images/building/house2.png").string());
                    homeButton.position = Math::fvec2(offsetX, startY + (BUTTON_HEIGHT + BUTTON_SPACING));
                    homeButton.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
                    homeButton.name     = "Home";
                    homeButton.tint     = Dokev::ColorPalette::WHITE;
                    buildingButtons.push_back(homeButton);

                    BuildingButton casinoButton;
                    casinoButton.type     = BuildingType::CASINO;
                    casinoButton.texture  = LoadTexture(assets::locate_asset("Assets/images/building/casino2.png").string());
                    casinoButton.position = Math::fvec2(offsetX, startY + (BUTTON_HEIGHT + BUTTON_SPACING) * 2);
                    casinoButton.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
                    casinoButton.name     = "Casino";
                    casinoButton.tint     = Dokev::ColorPalette::WHITE;
                    buildingButtons.push_back(casinoButton);

                    BuildingButton eduButton;
                    eduButton.type     = BuildingType::EDU_BUILDING;
                    eduButton.texture  = LoadTexture(assets::locate_asset("Assets/images/building/edu_building.png").string());
                    eduButton.position = Math::fvec2(offsetX, startY + (BUTTON_HEIGHT + BUTTON_SPACING) * 3);
                    eduButton.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
                    eduButton.name     = "School";
                    eduButton.tint     = Dokev::ColorPalette::WHITE;
                    buildingButtons.push_back(eduButton);
                    break;
                }

            case CategoryType::WORK_CATEGORY:
                {
                    float startY   = UI_SPACING + (BUTTON_HEIGHT + BUTTON_SPACING) * 2;
                    float offsetX  = startX + (BUTTON_WIDTH + BUTTON_SPACING) * 4;
                    float offsetX2 = offsetX + BUTTON_WIDTH + BUTTON_SPACING;  // second column for food buildings

                    // Column 1: production buildings
                    BuildingButton cancelButton;
                    cancelButton.type     = BuildingType::NONE;
                    cancelButton.texture  = LoadTexture(assets::locate_asset("Assets/images/textures/delete.png").string());
                    cancelButton.position = Math::fvec2(offsetX, startY);
                    cancelButton.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
                    cancelButton.name     = "Cancel";
                    cancelButton.tint     = Dokev::ColorPalette::RED;
                    buildingButtons.push_back(cancelButton);

                    BuildingButton steelButton;
                    steelButton.type     = BuildingType::STEEL_WORKSHOP;
                    steelButton.texture  = LoadTexture(assets::locate_asset("Assets/resources/IronMine.png").string());
                    steelButton.position = Math::fvec2(offsetX, startY + (BUTTON_HEIGHT + BUTTON_SPACING));
                    steelButton.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
                    steelButton.name     = "Steel Workshop";
                    steelButton.tint     = Dokev::ColorPalette::LIGHTGRAY;
                    buildingButtons.push_back(steelButton);

                    BuildingButton rockButton;
                    rockButton.type     = BuildingType::ROCK_WORKSHOP;
                    rockButton.texture  = LoadTexture(assets::locate_asset("Assets/resources/Quarry.png").string());
                    rockButton.position = Math::fvec2(offsetX, startY + (BUTTON_HEIGHT + BUTTON_SPACING) * 2);
                    rockButton.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
                    rockButton.name     = "Rock Workshop";
                    rockButton.tint     = Dokev::ColorPalette::GRAY;
                    buildingButtons.push_back(rockButton);

                    BuildingButton woodButton;
                    woodButton.type     = BuildingType::WOOD_WORKSHOP;
                    woodButton.texture  = LoadTexture(assets::locate_asset("Assets/resources/LoggingCamp.png").string());
                    woodButton.position = Math::fvec2(offsetX, startY + (BUTTON_HEIGHT + BUTTON_SPACING) * 3);
                    woodButton.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
                    woodButton.name     = "Wood Workshop";
                    woodButton.tint     = Dokev::ColorPalette::BROWN;
                    buildingButtons.push_back(woodButton);

                    BuildingButton granaryButton;
                    granaryButton.type     = BuildingType::GRANARY;
                    granaryButton.texture  = LoadTexture(assets::locate_asset("Assets/resources/Granary_2x2.png").string());
                    granaryButton.position = Math::fvec2(offsetX, startY + (BUTTON_HEIGHT + BUTTON_SPACING) * 4);
                    granaryButton.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
                    granaryButton.name     = "Granary";
                    granaryButton.tint     = Dokev::ColorPalette::WHITE;
                    buildingButtons.push_back(granaryButton);

                    BuildingButton farmButton;
                    farmButton.type     = BuildingType::FARM;
                    farmButton.texture  = LoadTexture(assets::locate_asset("Assets/resources/Farm.png").string());
                    farmButton.position = Math::fvec2(offsetX, startY + (BUTTON_HEIGHT + BUTTON_SPACING) * 5);
                    farmButton.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
                    farmButton.name     = "Farm";
                    farmButton.tint     = Dokev::ColorPalette::WHITE;
                    buildingButtons.push_back(farmButton);

                    // Column 2: food/service buildings
                    BuildingButton cafeButton;
                    cafeButton.type     = BuildingType::CAFE;
                    cafeButton.texture  = LoadTexture(assets::locate_asset("Assets/resources/Cafe.png").string());
                    cafeButton.position = Math::fvec2(offsetX2, startY + (BUTTON_HEIGHT + BUTTON_SPACING));
                    cafeButton.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
                    cafeButton.name     = "Cafe";
                    cafeButton.tint     = Dokev::ColorPalette::WHITE;
                    buildingButtons.push_back(cafeButton);

                    BuildingButton restaurantButton;
                    restaurantButton.type     = BuildingType::RESTAURANT2;
                    restaurantButton.texture  = LoadTexture(assets::locate_asset("Assets/resources/restaurant2.png").string());
                    restaurantButton.position = Math::fvec2(offsetX2, startY + (BUTTON_HEIGHT + BUTTON_SPACING) * 2);
                    restaurantButton.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
                    restaurantButton.name     = "Restaurant";
                    restaurantButton.tint     = Dokev::ColorPalette::WHITE;
                    buildingButtons.push_back(restaurantButton);

                    BuildingButton wp3Button;
                    wp3Button.type     = BuildingType::WORKPLACE3;
                    wp3Button.texture  = LoadTexture(assets::locate_asset("Assets/images/building/workplace3.png").string());
                    wp3Button.position = Math::fvec2(offsetX2, startY + (BUTTON_HEIGHT + BUTTON_SPACING) * 3);
                    wp3Button.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
                    wp3Button.name     = "Fire Station";
                    wp3Button.tint     = Dokev::ColorPalette::WHITE;
                    buildingButtons.push_back(wp3Button);

                    BuildingButton wp4Button;
                    wp4Button.type     = BuildingType::WORKPLACE4;
                    wp4Button.texture  = LoadTexture(assets::locate_asset("Assets/images/building/workplace4.png").string());
                    wp4Button.position = Math::fvec2(offsetX2, startY + (BUTTON_HEIGHT + BUTTON_SPACING) * 4);
                    wp4Button.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
                    wp4Button.name     = "Workplace 4";
                    wp4Button.tint     = Dokev::ColorPalette::WHITE;
                    buildingButtons.push_back(wp4Button);
                    break;
                }

            case CategoryType::ROAD_CATEGORY:
                {
                    float startY  = UI_SPACING + (BUTTON_HEIGHT + BUTTON_SPACING) * 2;
                    float offsetX = startX + (BUTTON_WIDTH + BUTTON_SPACING) * 4;

                    BuildingButton roadButton;
                    roadButton.type     = BuildingType::ROAD;
                    roadButton.texture  = LoadTexture(assets::locate_asset("Assets/images/road/roadUI_6432.png").string());
                    roadButton.position = Math::fvec2(offsetX, startY + (BUTTON_HEIGHT + BUTTON_SPACING));
                    roadButton.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
                    roadButton.name     = "Road";
                    roadButton.tint     = Dokev::ColorPalette::WHITE;
                    buildingButtons.push_back(roadButton);

                    BuildingButton cancelButton;
                    cancelButton.type     = BuildingType::NONE;
                    cancelButton.texture  = LoadTexture(assets::locate_asset("Assets/images/textures/delete.png").string());
                    cancelButton.position = Math::fvec2(offsetX, startY);
                    cancelButton.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
                    cancelButton.name     = "Cancel";
                    cancelButton.tint     = Dokev::ColorPalette::RED;
                    buildingButtons.push_back(cancelButton);
                    break;
                }

            case CategoryType::DECORATION_CATEGORY:
                {
                    float startY  = UI_SPACING + (BUTTON_HEIGHT + BUTTON_SPACING) * 2;
                    float offsetX = startX + (BUTTON_WIDTH + BUTTON_SPACING) * 4;

                    BuildingButton cancelButton;
                    cancelButton.type     = BuildingType::NONE;
                    cancelButton.texture  = LoadTexture(assets::locate_asset("Assets/images/textures/delete.png").string());
                    cancelButton.position = Math::fvec2(offsetX, startY);
                    cancelButton.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
                    cancelButton.name     = "Cancel";
                    cancelButton.tint     = Dokev::ColorPalette::RED;
                    buildingButtons.push_back(cancelButton);

                    BuildingButton missingButton;
                    missingButton.type     = BuildingType::MISSING_TEXTURE;
                    missingButton.texture  = LoadTexture(assets::locate_asset("Assets/images/textures/MissingTexture.png").string());
                    missingButton.position = Math::fvec2(offsetX, startY + (BUTTON_HEIGHT + BUTTON_SPACING));
                    missingButton.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
                    missingButton.name     = "Missing Texture";
                    missingButton.tint     = Dokev::ColorPalette::WHITE;
                    buildingButtons.push_back(missingButton);

                    BuildingButton streetLightButton;
                    streetLightButton.type     = BuildingType::STREET_LIGHT;
                    streetLightButton.texture  = LoadTexture(assets::locate_asset("Assets/resources/street_light.png").string());
                    streetLightButton.position = Math::fvec2(offsetX, startY + (BUTTON_HEIGHT + BUTTON_SPACING) * 2);
                    streetLightButton.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
                    streetLightButton.name     = "Street Light";
                    streetLightButton.tint     = Dokev::ColorPalette::WHITE;
                    buildingButtons.push_back(streetLightButton);

                    BuildingButton parkButton;
                    parkButton.type     = BuildingType::PARK;
                    parkButton.texture  = LoadTexture(assets::locate_asset("Assets/images/building/park.png").string());
                    parkButton.position = Math::fvec2(offsetX, startY + (BUTTON_HEIGHT + BUTTON_SPACING) * 3);
                    parkButton.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
                    parkButton.name     = "Park";
                    parkButton.tint     = Dokev::ColorPalette::WHITE;
                    buildingButtons.push_back(parkButton);
                    break;
                }

            default: break;
        }
    }

    void UIManager::CreateMainButtons()
    {
        mainButtons.clear();

        auto windowSize = Engine::GetWindow().GetSize();

        float totalWidth = BUTTON_WIDTH * 4 + BUTTON_SPACING * 3;
        float startX     = (static_cast<float>(windowSize.x) - totalWidth) / 2.0f;
        float startY     = UI_SPACING;

        BuildingButton constructionButton;
        constructionButton.type     = BuildingType::NONE;
        constructionButton.texture  = LoadTexture(assets::locate_asset("Assets/images/building/house2.png").string());
        constructionButton.position = Math::fvec2(startX, startY);
        constructionButton.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
        constructionButton.name     = "Build";
        constructionButton.tint     = Dokev::ColorPalette::WHITE;
        mainButtons.push_back(constructionButton);

        BuildingButton flattenButton;
        flattenButton.type     = BuildingType::FLATTEN;
        flattenButton.texture  = LoadTexture(assets::locate_asset("Assets/images/textures/IsoPlains.png").string());
        flattenButton.position = Math::fvec2(startX + (BUTTON_WIDTH + BUTTON_SPACING), startY);
        flattenButton.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
        flattenButton.name     = "Flatten";
        flattenButton.tint     = Dokev::ColorPalette::WHITE;
        mainButtons.push_back(flattenButton);

        BuildingButton deleteButton;
        deleteButton.type     = BuildingType::DELETE;
        deleteButton.texture  = LoadTexture(assets::locate_asset("Assets/images/textures/delete.png").string());
        deleteButton.position = Math::fvec2(startX + (BUTTON_WIDTH + BUTTON_SPACING) * 2, startY);
        deleteButton.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
        deleteButton.name     = "Destroy";
        deleteButton.tint     = Dokev::ColorPalette::WHITE;
        mainButtons.push_back(deleteButton);

        BuildingButton cancelButton;
        cancelButton.type     = BuildingType::NONE;
        cancelButton.texture  = LoadTexture(assets::locate_asset("Assets/images/textures/delete.png").string());
        cancelButton.position = Math::fvec2(startX + (BUTTON_WIDTH + BUTTON_SPACING) * 3, startY);
        cancelButton.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
        cancelButton.name     = "Cancel";
        cancelButton.tint     = Dokev::ColorPalette::RED;
        mainButtons.push_back(cancelButton);
    }

    void UIManager::CreateCategoryButtons()
    {
        categoryButtons.clear();

        auto windowSize = Engine::GetWindow().GetSize();

        float totalWidth = BUTTON_WIDTH * 4 + BUTTON_SPACING * 3;
        float startX     = (static_cast<float>(windowSize.x) - totalWidth) / 2.0f;
        float startY     = UI_SPACING + BUTTON_HEIGHT + BUTTON_SPACING;

        BuildingButton homeButton;
        homeButton.type     = BuildingType::NONE;
        homeButton.texture  = LoadTexture(assets::locate_asset("Assets/images/building/house2.png").string());
        homeButton.position = Math::fvec2(startX, startY);
        homeButton.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
        homeButton.name     = "Home";
        homeButton.tint     = Dokev::ColorPalette::WHITE;
        categoryButtons.push_back(homeButton);

        BuildingButton workButton;
        workButton.type     = BuildingType::NONE;
        workButton.texture  = LoadTexture(assets::locate_asset("Assets/images/building/workplace2.png").string());
        workButton.position = Math::fvec2(startX + (BUTTON_WIDTH + BUTTON_SPACING), startY);
        workButton.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
        workButton.name     = "Work";
        workButton.tint     = Dokev::ColorPalette::WHITE;
        categoryButtons.push_back(workButton);

        BuildingButton roadButton;
        roadButton.type     = BuildingType::NONE;
        roadButton.texture  = LoadTexture(assets::locate_asset("Assets/images/road/roadUI_6432.png").string());
        roadButton.position = Math::fvec2(startX + (BUTTON_WIDTH + BUTTON_SPACING) * 2, startY);
        roadButton.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
        roadButton.name     = "Road";
        roadButton.tint     = Dokev::ColorPalette::WHITE;
        categoryButtons.push_back(roadButton);

        BuildingButton decorationButton;
        decorationButton.type     = BuildingType::NONE;
        decorationButton.texture  = LoadTexture(assets::locate_asset("Assets/images/textures/MissingTexture.png").string());
        decorationButton.position = Math::fvec2(startX + (BUTTON_WIDTH + BUTTON_SPACING) * 3, startY);
        decorationButton.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
        decorationButton.name     = "Decoration";
        decorationButton.tint     = Dokev::ColorPalette::WHITE;
        categoryButtons.push_back(decorationButton);
    }

    void UIManager::Update(Math::fvec2 mousePos, bool mouseClicked, float dt)
    {
        totalTime += dt;
        // Keyboard shortcuts
        const auto& input = Engine::GetInput();

        //Update HoveringState
        UpdateHoverStates(mousePos);

        //Hotkey use
        hotkeyHandled = false;

        // C key: Cancel (works in any state)
        if (input.KeyJustPressed(CS230::Input::Keys::C) && !hotkeyHandled)
        {

            if (currentMenuState == MenuState::MAIN)
            {
                CancelSelection();
                hotkeyHandled = true;
            }
            else
            {
                CloseButtonMenus();
                hotkeyHandled = true;
            }

        }

        // B key: Build (open category menu from MAIN)
        if (input.KeyJustPressed(CS230::Input::Keys::B) && currentMenuState == MenuState::MAIN && !hotkeyHandled)
        {
            currentMenuState = MenuState::CATEGORY;
            CreateCategoryButtons();
            hotkeyHandled = true;
        }

        // Category menu shortcuts (when in CATEGORY state)
        if (currentMenuState == MenuState::CATEGORY && !hotkeyHandled)
        {
            if (input.KeyJustPressed(CS230::Input::Keys::H))
            {
                currentMenuState = MenuState::BUILDING;
                currentCategory  = CategoryType::HOME_CATEGORY;
                CreateBuildingButtons(CategoryType::HOME_CATEGORY);
                hotkeyHandled = true;
            }
            else if (input.KeyJustPressed(CS230::Input::Keys::W))
            {
                currentMenuState = MenuState::BUILDING;
                currentCategory  = CategoryType::WORK_CATEGORY;
                CreateBuildingButtons(CategoryType::WORK_CATEGORY);
                hotkeyHandled = true;
            }
            else if (input.KeyJustPressed(CS230::Input::Keys::R))
            {
                currentMenuState = MenuState::BUILDING;
                currentCategory  = CategoryType::ROAD_CATEGORY;
                CreateBuildingButtons(CategoryType::ROAD_CATEGORY);
                hotkeyHandled = true;
            }
            else if (input.KeyJustPressed(CS230::Input::Keys::D))
            {
                currentMenuState = MenuState::BUILDING;
                currentCategory  = CategoryType::DECORATION_CATEGORY;
                CreateBuildingButtons(CategoryType::DECORATION_CATEGORY);
                hotkeyHandled = true;
            }
        }

        // Building selection shortcuts (when in BUILDING state)
        if (currentMenuState == MenuState::BUILDING && !hotkeyHandled)
        {
            if (currentCategory == CategoryType::HOME_CATEGORY)
            {
                if (input.KeyJustPressed(CS230::Input::Keys::H))
                {
                    selectedBuilding = BuildingType::HOME;
                    CloseButtonMenus();
                    hotkeyHandled = true;
                }
            }
            else if (currentCategory == CategoryType::WORK_CATEGORY)
            {
                if (input.KeyJustPressed(CS230::Input::Keys::W))
                {
                    selectedBuilding = BuildingType::WOOD_WORKSHOP;
                    CloseButtonMenus();
                    hotkeyHandled = true;
                }
                else if (input.KeyJustPressed(CS230::Input::Keys::R))
                {
                    selectedBuilding = BuildingType::ROCK_WORKSHOP;
                    CloseButtonMenus();
                    hotkeyHandled = true;
                }
                else if (input.KeyJustPressed(CS230::Input::Keys::S))
                {
                    selectedBuilding = BuildingType::STEEL_WORKSHOP;
                    CloseButtonMenus();
                    hotkeyHandled = true;
                }
                else if (input.KeyJustPressed(CS230::Input::Keys::G))
                {
                    selectedBuilding = BuildingType::GRANARY;
                    CloseButtonMenus();
                    hotkeyHandled = true;
                }
                else if (input.KeyJustPressed(CS230::Input::Keys::F))
                {
                    selectedBuilding = BuildingType::FARM;
                    CloseButtonMenus();
                    hotkeyHandled = true;
                }
            }
            else if (currentCategory == CategoryType::ROAD_CATEGORY)
            {
                if (input.KeyJustPressed(CS230::Input::Keys::R))
                {
                    selectedBuilding = BuildingType::ROAD;
                    CloseButtonMenus();
                    hotkeyHandled = true;
                }
            }
            else if (currentCategory == CategoryType::DECORATION_CATEGORY)
            {
                if (input.KeyJustPressed(CS230::Input::Keys::M))
                {
                    selectedBuilding = BuildingType::MISSING_TEXTURE;
                    CloseButtonMenus();
                    hotkeyHandled = true;
                }
                else if (input.KeyJustPressed(CS230::Input::Keys::S))
                {
                    selectedBuilding = BuildingType::STREET_LIGHT;
                    CloseButtonMenus();
                    hotkeyHandled = true;
                }
            }
        }

        switch (currentMenuState)
        {
            case MenuState::MAIN:
                {
                    if (!mouseClicked)
                    break;

                    for (auto& button : mainButtons)
                    {

                        if (!button.isHovered) continue;

                        if (button.name == "Build")
                        {
                            selectedBuilding = BuildingType::NONE;
                            currentMenuState = MenuState::CATEGORY;
                            CreateCategoryButtons();
                        }
                        else if (button.type == BuildingType::FLATTEN)
                        {
                            selectedBuilding = BuildingType::FLATTEN;
                            categoryButtons.clear();
                            buildingButtons.clear();
                        }
                        else if (button.type == BuildingType::DELETE)
                        {
                            selectedBuilding = BuildingType::DELETE;
                            categoryButtons.clear();
                            buildingButtons.clear();
                        }
                        else if (button.name == "Cancel")
                        {
                            //ResetToMainButtonState();
                            CancelSelection();

                        }

                        break;
                    }
                    break;
                }

            case MenuState::CATEGORY:
                {
                    if (!mouseClicked)
                    break;

                    for (auto& button : categoryButtons)
                    {

                        if (!button.isHovered) continue;

                        currentMenuState = MenuState::BUILDING;

                        if (button.name == "Home")
                            currentCategory = CategoryType::HOME_CATEGORY;
                        else if (button.name == "Work")
                            currentCategory = CategoryType::WORK_CATEGORY;
                        else if (button.name == "Road")
                            currentCategory = CategoryType::ROAD_CATEGORY;
                        else if (button.name == "Decoration")
                            currentCategory = CategoryType::DECORATION_CATEGORY;

                        CreateBuildingButtons(currentCategory);
                        break;
                    }
                    break;
                }

            case MenuState::BUILDING:
                {
                    if(!mouseClicked)
                    break;

                    for (auto& button : categoryButtons)
                    {

                        if (!button.isHovered) continue;

                        if (button.name == "Home")
                            currentCategory = CategoryType::HOME_CATEGORY;
                        else if (button.name == "Work")
                            currentCategory = CategoryType::WORK_CATEGORY;
                        else if (button.name == "Road")
                            currentCategory = CategoryType::ROAD_CATEGORY;
                        else if (button.name == "Decoration")
                            currentCategory = CategoryType::DECORATION_CATEGORY;

                        CreateBuildingButtons(currentCategory);
                        return;
                    }

                    for (auto& button : buildingButtons)
                    {

                        if (!button.isHovered) continue;

                        if (button.name == "Cancel")
                        {
                            // In buildingbuttons, 
                            // Cancel button maintains the previous selectedbuilding state
                            CloseButtonMenus();
                            return;

                            //selectedBuilding = BuildingType::NONE;
                        }
                        else
                        {
                            selectedBuilding = button.type;
                        }

                        currentMenuState = MenuState::MAIN;
                        currentCategory = CategoryType::NONE;
                        categoryButtons.clear();
                        buildingButtons.clear();

                        return;
                    }
                    break;
                }
        }
        UpdateSelectedStates(); 
        UpdateButtonScales(dt);
    }

    void UIManager::Draw(ResourceManager* resourceMgr, CityMetrics* cm, int humanCount, BuildingCostManager* costMgr, int hour, int minute) const
    {
        DrawResources(resourceMgr, humanCount, hour, minute);
        DrawHappiness(cm);

        switch (currentMenuState)
        {
            case MenuState::MAIN:
                for (const auto& button : mainButtons)
                {
                    DrawBuildingButton(button, resourceMgr, costMgr, totalTime);
                }
                break;

            case MenuState::CATEGORY:
                for (const auto& button : mainButtons)
                {
                    DrawBuildingButton(button, resourceMgr, costMgr, totalTime);
                }
                for (const auto& button : categoryButtons)
                {
                    DrawBuildingButton(button, resourceMgr, costMgr, totalTime);
                }
                break;

            case MenuState::BUILDING:
                for (const auto& button : mainButtons)
                {
                    DrawBuildingButton(button, resourceMgr, costMgr, totalTime);
                }
                for (const auto& button : categoryButtons)
                {
                    DrawBuildingButton(button, resourceMgr, costMgr, totalTime);
                }
                for (const auto& button : buildingButtons)
                {
                    DrawBuildingButton(button, resourceMgr, costMgr, totalTime);
                }
                break;
        }
    }

    void UIManager::DrawResources(ResourceManager* resourceMgr, int humanCount, int hour, int minute) const
    {
        if (!resourceMgr)
            return;

        auto  windowSize = Engine::GetWindow().GetSize();
        float startX     = static_cast<float>(windowSize.x) - 170.0f;
        float resourceY  = static_cast<float>(windowSize.y) - 100.0f;

        float height       = 35.0f;
        float paddingY     = -10;
        int   iconPaddingY = 15;
        float iconScale    = 0.8f;

        float currentX = startX;
        float currentY = resourceY;

        if (outlined)
        {
            std::string moneyText = "Money: " + FormatNumber(resourceMgr->totalMoney);
            if (auto textTexture = outlined->PrintToTexture(moneyText, Dokev::ColorPalette::WHITE))
            {
                textTexture->Draw(
                    Math::TranslationMatrix(Math::vec2{ currentX - ICON_SPACING, currentY - paddingY }) * Math::ScaleMatrix(Math::vec2{ FONT_SCALE, FONT_SCALE }),
                    Dokev::ColorPalette::WHITE);
            }
        }
        currentY -= height + 5.0f;

        // Draw food count
        if (outlined)
        {
            const int   foodAmount   = resourceMgr->GetFood();
            const int   foodCapacity = resourceMgr->GetFoodCapacity();
            std::string foodText     = "Food: " + std::to_string(foodAmount) + "/" + std::to_string(foodCapacity);
            if (auto textTexture = outlined->PrintToTexture(foodText, Dokev::ColorPalette::WHITE))
            {
                textTexture->Draw(
                    Math::TranslationMatrix(Math::vec2{ currentX - ICON_SPACING, currentY - paddingY }) * Math::ScaleMatrix(Math::vec2{ FONT_SCALE, FONT_SCALE }),
                    Dokev::ColorPalette::WHITE);
            }
            currentY -= height + 5.0f;

            std::string granaryHint;
            CS200::RGBA hintColor = Dokev::ColorPalette::YELLOW;

            if (foodCapacity <= 0)
            {
                granaryHint = "Build Granary to store Food";
                hintColor   = Dokev::ColorPalette::ORANGE;
            }
            else if (foodAmount * 100 >= foodCapacity * kGranaryWarningThresholdPercent)
            {
                granaryHint = "Food storage almost full";
            }

            if (!granaryHint.empty())
            {
                if (auto textTexture = outlined->PrintToTexture(granaryHint, hintColor))
                {
                    textTexture->Draw(
                        Math::TranslationMatrix(Math::vec2{ currentX - ICON_SPACING, currentY - paddingY }) * Math::ScaleMatrix(Math::vec2{ 0.35, 0.35 }),
                        hintColor);
                }
                currentY -= 22.0f;
            }
        }

        if (humanIcon.IsValid())
        {
            humanIcon.DrawEx(static_cast<int>(currentX) - ICON_SPACING, static_cast<int>(currentY) + iconPaddingY, 0.0f, iconScale, Dokev::ColorPalette::WHITE);
        }
        if (outlined)
        {
            std::string humanText = FormatNumber(humanCount);
            if (auto textTexture = outlined->PrintToTexture(humanText, Dokev::ColorPalette::WHITE))
            {
                textTexture->Draw(Math::TranslationMatrix(Math::vec2{ currentX, currentY - paddingY }) * Math::ScaleMatrix(Math::vec2{ FONT_SCALE, FONT_SCALE }), Dokev::ColorPalette::WHITE);
            }
        }
        currentY -= height + 5.0f;

        // Draw time (HH:MM)
        if (outlined)
        {
            char timeBuf[16];
            std::snprintf(timeBuf, sizeof(timeBuf), "Time : %02d : %02d", hour, minute);
            if (auto textTexture = outlined->PrintToTexture(timeBuf, Dokev::ColorPalette::WHITE))
            {
                textTexture->Draw(
                    Math::TranslationMatrix(Math::vec2{ currentX - ICON_SPACING, currentY - paddingY }) * Math::ScaleMatrix(Math::vec2{ FONT_SCALE, FONT_SCALE }),
                    Dokev::ColorPalette::WHITE);
            }
        }
    }

    void UIManager::DrawHappiness(CityMetrics* cm) const
    {
        auto  windowSize   = Engine::GetWindow().GetSize();
        float startX       = static_cast<float>(windowSize.x) - 170.0f;
        float startY       = static_cast<float>(windowSize.y) - 60.0f;
        int   iconPaddingY = 15;

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

    void UIManager::DrawBuildingButton(const BuildingButton& button, ResourceManager* resourceMgr, BuildingCostManager* costMgr, float time) const
    {
        // Compute scaled box from center 
        float scale   = button.hoverScale;
        float centerX = button.position.x + button.size.x * 0.5f;
        float centerY = button.position.y + button.size.y * 0.5f;
        float scaledW = button.size.x * scale;
        float scaledH = button.size.y * scale;
        Math::fvec2 scaledPos  = Math::fvec2(centerX - scaledW * 0.5f, centerY - scaledH * 0.5f);
        Math::fvec2 scaledSize = Math::fvec2(scaledW, scaledH);

        CS200::RGBA boxColor = Dokev::ColorPalette::BOX_INSIDE_DEFAULT;

        if (button.isSelected)
        {
            if(button.type == BuildingType::FLATTEN || button.type == BuildingType::DELETE)
            {
                boxColor = Dokev::ColorPalette::BOX_INSIDE_HOVERED; // gray
            }
            else if (button.type != BuildingType::NONE)
            {
                boxColor = Dokev::ColorPalette::BOX_INSIDE_SELECTED; // green
            }
            else
            {
                // main / category pressed
                boxColor = Dokev::ColorPalette::BOX_INSIDE_HOVERED; // gray
            }
        }
        else if (button.isHovered)
        {
            if ((button.type != BuildingType::NONE &&
                button.type != BuildingType::FLATTEN &&
                button.type != BuildingType::DELETE))
            {   
                //building
                boxColor = Dokev::ColorPalette::BOX_INSIDE_SELECTED; // green
            }
            else
            {
                // main/category 
                boxColor = Dokev::ColorPalette::BOX_INSIDE_HOVERED; // gray
            }
        }

        DrawBox(scaledPos, scaledSize, boxColor);

        CS200::RGBA borderColor = button.isSelected ? Dokev::ColorPalette::BOX_OUTLINE_SELECTED : Dokev::ColorPalette::BOX_OUTLINE_DEFAULT;

        DrawRectangleLinesEx(static_cast<int>(scaledPos.x), static_cast<int>(scaledPos.y), static_cast<int>(scaledSize.x), static_cast<int>(scaledSize.y), 3.0f, borderColor);

        // Draw Building Texture.
        if (button.texture.IsValid())
        {
            int textureWidth  = button.texture.GetWidth();
            int textureHeight = button.texture.GetHeight();

            float iconScale    = static_cast<float>(ICON_SCALE) * scale;
            float scaledWidth  = textureWidth * iconScale;
            float scaledHeight = textureHeight * iconScale;

            float bobOffset = button.isSelected ? std::sin(time * 3.0f) * 4.0f : 0.0f;

            Math::fvec2 imagePos = Math::fvec2(scaledPos.x + (scaledSize.x - scaledWidth) / 2.0f, scaledPos.y + (scaledSize.y - scaledHeight) / 2.0f + bobOffset);

            button.texture.DrawEx(static_cast<int>(imagePos.x), static_cast<int>(imagePos.y), 0.0f, iconScale, button.tint);
        }

        // Draw required resources
        if (button.isHovered && (button.type == BuildingType::ROAD || button.type == BuildingType::WOOD_WORKSHOP || button.type == BuildingType::ROCK_WORKSHOP ||
                                 button.type == BuildingType::STEEL_WORKSHOP || button.type == BuildingType::GRANARY || button.type == BuildingType::FARM ||
                                 button.type == BuildingType::HOME || button.type == BuildingType::CAFE || button.type == BuildingType::RESTAURANT2 ||
                                 button.type == BuildingType::MISSING_TEXTURE || button.type == BuildingType::STREET_LIGHT ||
                                 button.type == BuildingType::WORKPLACE3 || button.type == BuildingType::WORKPLACE4 ||
                                 button.type == BuildingType::CASINO || button.type == BuildingType::EDU_BUILDING ||
                                 button.type == BuildingType::PARK))
        {
            float tooltipWidth  = 180.0f;
            float tooltipHeight = 60.0f;

            // Add space for hotkey display
            std::string hotkey = GetHotkeyForButton(button);
            if (!hotkey.empty())
            {
                tooltipHeight += 15.0f;
            }

            Math::fvec2 tooltipPos = Math::fvec2(button.position.x - tooltipWidth - 10.0f, button.position.y);

            DrawBox(tooltipPos, Math::fvec2(tooltipWidth, tooltipHeight), Dokev::ColorPalette::TOOLTIP_BACKGROUND);

            DrawRectangleLinesEx(static_cast<int>(tooltipPos.x), static_cast<int>(tooltipPos.y), static_cast<int>(tooltipWidth), static_cast<int>(tooltipHeight), 2.0f, Dokev::ColorPalette::LIGHTGRAY);

            if (outlined && resourceMgr && costMgr)
            {
                // Get cost from JSON
                BuildingCost cost = costMgr->GetCost(button.type);

                if (auto nameTexture = outlined->PrintToTexture(button.name, Dokev::ColorPalette::WHITE))
                {
                    nameTexture->Draw(
                        Math::TranslationMatrix(Math::vec2{ tooltipPos.x + 10.0f, tooltipPos.y + 12.0f }) * Math::ScaleMatrix(Math::vec2{ 0.35, 0.35 }),
                        Dokev::ColorPalette::WHITE);
                }

                if (cost.money > 0)
                {
                    std::string moneyText = "Money: " + std::to_string(resourceMgr->totalMoney) + " / " + std::to_string(cost.money);
                    if (auto textTexture = outlined->PrintToTexture(moneyText, Dokev::ColorPalette::WHITE))
                    {
                        textTexture->Draw(
                            Math::TranslationMatrix(Math::vec2{ tooltipPos.x + 10.0f, tooltipPos.y + 30.0f }) * Math::ScaleMatrix(Math::vec2{ 0.3, 0.3 }),
                            Dokev::ColorPalette::WHITE);
                    }
                }

                // Draw hotkey at the bottom
                if (!hotkey.empty())
                {
                    float hotkeyY = tooltipPos.y + tooltipHeight - 15.0f;
                    float hotkeyX = tooltipPos.x + 10.0f;

                    // Draw Hotkey:  in white
                    if (auto labelTexture = outlined->PrintToTexture("Hotkey: ", Dokev::ColorPalette::WHITE))
                    {
                        labelTexture->Draw(Math::TranslationMatrix(Math::vec2{ hotkeyX, hotkeyY }) * Math::ScaleMatrix(Math::vec2{ 0.25, 0.25 }), Dokev::ColorPalette::WHITE);
                        hotkeyX += labelTexture->GetSize().x * 0.25f;
                    }

                    // Draw key in green
                    if (auto keyTexture = outlined->PrintToTexture(hotkey, Dokev::ColorPalette::GREEN))
                    {
                        keyTexture->Draw(Math::TranslationMatrix(Math::vec2{ hotkeyX, hotkeyY }) * Math::ScaleMatrix(Math::vec2{ 0.25, 0.25 }), Dokev::ColorPalette::GREEN);
                    }
                }
            }
        }
        else if (button.isHovered && !button.name.empty())
        {
            std::string tooltipText   = button.name;
            float       textWidth     = static_cast<float>(tooltipText.length()) * 8.0f;
            float       tooltipWidth  = textWidth + 20.0f;
            float       tooltipHeight = 40.0f;

            // Add space for hotkey display
            std::string hotkey = GetHotkeyForButton(button);
            if (!hotkey.empty())
            {
                tooltipHeight += 15.0f;
            }

            if (tooltipWidth < 140.0f)
            {
                tooltipWidth = 140.0f;
            }

            Math::fvec2 tooltipPos = Math::fvec2(button.position.x - tooltipWidth - 10.0f, button.position.y);

            DrawBox(tooltipPos, Math::fvec2(tooltipWidth, tooltipHeight), Dokev::ColorPalette::TOOLTIP_BACKGROUND);
            DrawRectangleLinesEx(static_cast<int>(tooltipPos.x), static_cast<int>(tooltipPos.y), static_cast<int>(tooltipWidth), static_cast<int>(tooltipHeight), 2.0f, Dokev::ColorPalette::LIGHTGRAY);

            if (outlined)
            {
                if (auto nameTexture = outlined->PrintToTexture(tooltipText, Dokev::ColorPalette::WHITE))
                {
                    nameTexture->Draw(Math::TranslationMatrix(Math::vec2{ tooltipPos.x + 10.0f, tooltipPos.y + 15.0f }) * Math::ScaleMatrix(Math::vec2{ 0.35, 0.35 }), Dokev::ColorPalette::WHITE);
                }

                // Draw hotkey at the bottom
                if (!hotkey.empty())
                {
                    float hotkeyY = tooltipPos.y + tooltipHeight - 15.0f;
                    float hotkeyX = tooltipPos.x + 10.0f;

                    // Draw Hotkey:  in white
                    if (auto labelTexture = outlined->PrintToTexture("Hotkey: ", Dokev::ColorPalette::WHITE))
                    {
                        labelTexture->Draw(Math::TranslationMatrix(Math::vec2{ hotkeyX, hotkeyY }) * Math::ScaleMatrix(Math::vec2{ 0.25, 0.25 }), Dokev::ColorPalette::WHITE);
                        hotkeyX += labelTexture->GetSize().x * 0.25f;
                    }

                    // Draw key in green
                    if (auto keyTexture = outlined->PrintToTexture(hotkey, Dokev::ColorPalette::GREEN))
                    {
                        keyTexture->Draw(Math::TranslationMatrix(Math::vec2{ hotkeyX, hotkeyY }) * Math::ScaleMatrix(Math::vec2{ 0.25, 0.25 }), Dokev::ColorPalette::GREEN);
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
            char  buffer[32];
            std::snprintf(buffer, sizeof(buffer), "%.1fk", displayValue);
            return std::string(buffer);
        }
        return std::to_string(value);
    }

    std::string UIManager::GetHotkeyForButton(const BuildingButton& button) const
    {
        // Category buttons
        if (currentMenuState == MenuState::CATEGORY)
        {
            if (button.name == "Home")
                return "H";
            if (button.name == "Work")
                return "W";
            if (button.name == "Road")
                return "R";
            if (button.name == "Decoration")
                return "D";
        }

        // Building buttons
        if (currentMenuState == MenuState::BUILDING)
        {
            if (currentCategory == CategoryType::HOME_CATEGORY)
            {
                if (button.type == BuildingType::HOME)
                    return "H";
            }
            else if (currentCategory == CategoryType::WORK_CATEGORY)
            {
                if (button.type == BuildingType::WOOD_WORKSHOP)
                    return "W";
                if (button.type == BuildingType::ROCK_WORKSHOP)
                    return "R";
                if (button.type == BuildingType::STEEL_WORKSHOP)
                    return "S";
                if (button.type == BuildingType::GRANARY)
                    return "G";
                if (button.type == BuildingType::FARM)
                    return "F";
            }
            else if (currentCategory == CategoryType::ROAD_CATEGORY)
            {
                if (button.type == BuildingType::ROAD)
                    return "R";
            }
            else if (currentCategory == CategoryType::DECORATION_CATEGORY)
            {
                if (button.type == BuildingType::MISSING_TEXTURE)
                    return "M";
                if (button.type == BuildingType::STREET_LIGHT)
                    return "S";
            }
            if (button.name == "Cancel")
                return "C";
        }

        // Main menu buttons
        if (currentMenuState == MenuState::MAIN)
        {
            if (button.name == "Build")
                return "B";
            if (button.name == "Cancel")
                return "C";
        }

        return ""; // No hotkey
    }

    // ---------------- New Helper --------------- // 
    void UIManager::CloseButtonMenus()
    {
        currentMenuState = MenuState::MAIN;
        currentCategory = CategoryType::NONE;
        categoryButtons.clear();
        buildingButtons.clear();
    }
    void UIManager::CancelSelection()
    {
        selectedBuilding = BuildingType::NONE;
        CloseButtonMenus();
    }
    void UIManager::UpdateHoverStates(Math::fvec2 mousePos)
    {
        for (auto& button : mainButtons)
        {
            button.isHovered = PointInRect(mousePos, button.position, button.size);
        }

        for (auto& button : categoryButtons)
        {
            button.isHovered = PointInRect(mousePos, button.position, button.size);
        }

        for (auto& button : buildingButtons)
        {
            button.isHovered = PointInRect(mousePos, button.position, button.size);
        }
    }
    void UIManager::UpdateButtonScales(float dt)
    {
        for (auto& button : mainButtons)
        {
            float targetScale = button.isHovered ? 1.3f : 1.0f;
            button.hoverScale +=
                (targetScale - button.hoverScale) * std::min(1.0f, 4.0f * dt);
        }

        for (auto& button : categoryButtons)
        {
            float targetScale = button.isHovered ? 1.3f : 1.0f;
            button.hoverScale +=
                (targetScale - button.hoverScale) * std::min(1.0f, 4.0f * dt);
        }

        for (auto& button : buildingButtons)
        {
            float targetScale = button.isHovered ? 1.3f : 1.0f;
            button.hoverScale +=
                (targetScale - button.hoverScale) * std::min(1.0f, 4.0f * dt);
        }
    }
    void UIManager::UpdateSelectedStates()
    {
        for (auto& button : mainButtons)
        {
            button.isSelected = IsMainButtonSelected(button);
        }

        for (auto& button : categoryButtons)
        {
            button.isSelected = IsCategoryButtonSelected(button);
        }

        for (auto& button : buildingButtons)
        {
            button.isSelected = IsBuildingButtonSelected(button);
        }
    }
    bool UIManager::IsMainButtonSelected(const BuildingButton& button) const
    {
        if (button.name == "Cancel")
        {
            return false;
        }

        if (button.name == "Build")
        {
            return currentMenuState == MenuState::CATEGORY ||
                currentMenuState == MenuState::BUILDING;
        }

        if (button.type == BuildingType::FLATTEN)
        {
            return selectedBuilding == BuildingType::FLATTEN;
        }

        if (button.type == BuildingType::DELETE)
        {
            return selectedBuilding == BuildingType::DELETE;
        }

        return false;
    }
    bool UIManager::IsCategoryButtonSelected(const BuildingButton& button) const
    {
        if (currentMenuState != MenuState::BUILDING)
        {
            return false;
        }

        if (button.name == "Home")
        {
            return currentCategory == CategoryType::HOME_CATEGORY;
        }

        if (button.name == "Work")
        {
            return currentCategory == CategoryType::WORK_CATEGORY;
        }

        if (button.name == "Road")
        {
            return currentCategory == CategoryType::ROAD_CATEGORY;
        }

        if (button.name == "Decoration")
        {
            return currentCategory == CategoryType::DECORATION_CATEGORY;
        }

        return false;
    }
    bool UIManager::IsBuildingButtonSelected(const BuildingButton& button) const
    {
        if (button.name == "Cancel")
        {
            return false;
        }

        return selectedBuilding != BuildingType::NONE &&
            button.type != BuildingType::NONE &&
            button.type == selectedBuilding;
    }
}
