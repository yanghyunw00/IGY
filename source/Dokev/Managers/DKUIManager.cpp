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
#include "Game/Managers/DKCityMetrics.hpp"
#include "Game/Managers/HappinessManager.hpp"
#include "Game/Managers/ResourceManager.hpp"
#include <cmath>
#include <string>


namespace
{
    constexpr int kGranaryWarningThresholdPercent = 90;

    constexpr int kEscButtonCount = 6;

    struct EscMenuLayout
    {
        float panelX, panelY, panelW, panelH;
        float btnX,   btnW,   btnH;
        float titleY;
        float btnY[kEscButtonCount];
    };

    EscMenuLayout GetEscMenuLayout(int winW, int winH)
    {
        EscMenuLayout l;
        l.panelW  = 280.0f;
        l.btnH    = 46.0f;
        float start  = 82.0f;          // first button offset from panel top
        float stride = l.btnH + 11.0f;
        l.panelH  = start + stride * (kEscButtonCount - 1) + l.btnH + 22.0f;
        l.panelX  = winW * 0.5f - l.panelW * 0.5f;
        l.panelY  = winH * 0.5f - l.panelH * 0.5f;
        l.btnW    = 240.0f;
        l.btnX    = winW * 0.5f - l.btnW * 0.5f;
        l.titleY  = l.panelY + 28.0f;
        for (int i = 0; i < kEscButtonCount; ++i)
            l.btnY[i] = l.panelY + start + stride * i;
        return l;
    }
}

namespace Dokev
{

    UIManager::UIManager()
    {
    }

    void UIManager::Initialize()
    {
        CreateMainButtons();
        happyIcon     = LoadTexture(assets::locate_asset("Assets/images/textures/smile.png").string());
        humanIcon     = LoadTexture(assets::locate_asset("Assets/images/textures/Human.png").string());
        iconTime      = LoadTexture(assets::locate_asset("Assets/images/textures/top_icon_time.png").string());
        iconMoney     = LoadTexture(assets::locate_asset("Assets/images/textures/top_icon_money.png").string());
        iconFood      = LoadTexture(assets::locate_asset("Assets/images/textures/top_icon_food.png").string());
        iconHuman     = LoadTexture(assets::locate_asset("Assets/images/textures/top_icon_human.png").string());
        iconHappiness = LoadTexture(assets::locate_asset("Assets/images/textures/top_icon_happiness.png").string());
        outlined      = std::make_unique<CS230::Font>(assets::locate_asset("Assets/fonts/DungGeunMo.ttf"), 40);
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
                    casinoButton.texture  = LoadTexture(assets::locate_asset("Assets/images/building/edu_building.png").string());
                    casinoButton.position = Math::fvec2(offsetX, startY + (BUTTON_HEIGHT + BUTTON_SPACING) * 2);
                    casinoButton.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
                    casinoButton.name     = "Casino";
                    casinoButton.tint     = Dokev::ColorPalette::WHITE;
                    buildingButtons.push_back(casinoButton);

                    BuildingButton eduButton;
                    eduButton.type     = BuildingType::PENTHOUSE;
                    eduButton.texture  = LoadTexture(assets::locate_asset("Assets/images/building/casino2.png").string());
                    eduButton.position = Math::fvec2(offsetX, startY + (BUTTON_HEIGHT + BUTTON_SPACING) * 3);
                    eduButton.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
                    eduButton.name     = "Penthouse";
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
                    wp4Button.type     = BuildingType::DISASTER_OBSERVATORY;
                    wp4Button.texture  = LoadTexture(assets::locate_asset("Assets/images/building/disaster_observatory.png").string());
                    wp4Button.position = Math::fvec2(offsetX2, startY + (BUTTON_HEIGHT + BUTTON_SPACING) * 4);
                    wp4Button.size     = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
                    wp4Button.name     = "Disaster Observatory";
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

    void UIManager::Update(Math::fvec2 mousePos, bool mouseClicked, float dt, bool mouseDown)
    {
        totalTime += dt;

        // Reset ESC menu click flags every frame
        escResumeClicked   = false;
        escSaveClicked     = false;
        escLoadClicked     = false;
        escSettingsClicked = false;
        escMainMenuClicked = false;
        escQuitClicked     = false;

        if (escMenuOpen)
        {
            auto ws = Engine::GetWindow().GetSize();
            auto  l = GetEscMenuLayout(ws.x, ws.y);
            escHoveredButton = -1;
            for (int i = 0; i < kEscButtonCount; ++i)
            {
                if (mousePos.x >= l.btnX && mousePos.x <= l.btnX + l.btnW &&
                    mousePos.y >= l.btnY[i] && mousePos.y <= l.btnY[i] + l.btnH)
                {
                    escHoveredButton = i;
                    break;
                }
            }

            escPressedButton = (mouseDown && escHoveredButton >= 0) ? escHoveredButton : -1;

            // Ease hover/press factors toward their targets so the buttons feel springy
            float hoverSpeed = std::min(1.0f, dt * 16.0f);
            float pressSpeed = std::min(1.0f, dt * 28.0f);
            for (int i = 0; i < kEscButtonCount; ++i)
            {
                float hoverTarget = (escHoveredButton == i) ? 1.0f : 0.0f;
                float pressTarget = (escPressedButton == i) ? 1.0f : 0.0f;
                escHoverAnim[i] += (hoverTarget - escHoverAnim[i]) * hoverSpeed;
                escPressAnim[i] += (pressTarget - escPressAnim[i]) * pressSpeed;
            }

            if (mouseClicked && escHoveredButton >= 0)
            {
                switch (escHoveredButton)
                {
                    case 0: escResumeClicked   = true; escMenuOpen = false; break;
                    case 1: escSaveClicked     = true; break;
                    case 2: escLoadClicked     = true; break;
                    case 3: escSettingsClicked = true; break;
                    case 4: escMainMenuClicked = true; break;
                    case 5: escQuitClicked     = true; break;
                }
            }
            return; // block all other UI input while ESC menu is open
        }
        else
        {
            // Decay animations back to rest while the menu is closed
            for (int i = 0; i < kEscButtonCount; ++i)
            {
                escHoverAnim[i] = 0.0f;
                escPressAnim[i] = 0.0f;
            }
            escPressedButton = -1;
        }

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

        // Top bar icon column hover / click detection
        {
            auto  ws   = Engine::GetWindow().GetSize();
            float colW = static_cast<float>(ws.x) / 5.0f;
            float barY = static_cast<float>(ws.y) - 50.0f;
            bool  inBar = (mousePos.y >= barY && mousePos.y <= static_cast<float>(ws.y));

            hoveredIconColumn    = -1;
            timeHoveredThisFrame  = false;
            moneyHoveredThisFrame = false;

            if (inBar)
            {
                int col = static_cast<int>(mousePos.x / colW);
                if (col >= 0 && col < 5)
                {
                    hoveredIconColumn = col;
                    if (col == 0) timeHoveredThisFrame  = true;
                    if (col == 1) moneyHoveredThisFrame = true;
                }
            }

            foodHoveredThisFrame = inBar && hoveredIconColumn == 2;
            popHoveredThisFrame  = inBar && hoveredIconColumn == 3;
            happHoveredThisFrame = inBar && hoveredIconColumn == 4;

            timeClickedThisFrame  = timeHoveredThisFrame  && mouseClicked;
            moneyClickedThisFrame = moneyHoveredThisFrame && mouseClicked;
            foodClickedThisFrame  = foodHoveredThisFrame  && mouseClicked;
            popClickedThisFrame   = popHoveredThisFrame   && mouseClicked;

            if (timeClickedThisFrame)
                timeClickAnimTimer = 0.12f;
            if (moneyClickedThisFrame)
                moneyPopupOpen = !moneyPopupOpen;
            if (mouseClicked && !moneyHoveredThisFrame)
                moneyPopupOpen = false;
            if (foodClickedThisFrame)
                foodPopupOpen = !foodPopupOpen;
            if (mouseClicked && !foodHoveredThisFrame)
                foodPopupOpen = false;
            if (popClickedThisFrame)
                popPopupOpen = !popPopupOpen;
            if (mouseClicked && !popHoveredThisFrame)
                popPopupOpen = false;
            happClickedThisFrame = happHoveredThisFrame && mouseClicked;
            if (happClickedThisFrame)
                happPopupOpen = !happPopupOpen;
            if (mouseClicked && !happHoveredThisFrame)
                happPopupOpen = false;
        }

        if (timeClickAnimTimer > 0.0f)
            timeClickAnimTimer -= dt;
    }

    void UIManager::Draw(ResourceManager* resourceMgr, CityMetrics* cm, int humanCount, const PopulationStats& popStats, BuildingCostManager* costMgr, int hour, int minute, int currentDay, int speedIndex, bool simPaused) const
    {
        DrawResources(resourceMgr, cm, humanCount, popStats, hour, minute, currentDay, speedIndex, simPaused);
        DrawHappiness(cm);

        switch (currentMenuState)
        {
            case MenuState::MAIN:
                for (const auto& button : mainButtons)
                {
                    DrawBuildingButton(button, resourceMgr, costMgr, totalTime, cm, humanCount);
                }
                break;

            case MenuState::CATEGORY:
                for (const auto& button : mainButtons)
                {
                    DrawBuildingButton(button, resourceMgr, costMgr, totalTime, cm, humanCount);
                }
                for (const auto& button : categoryButtons)
                {
                    DrawBuildingButton(button, resourceMgr, costMgr, totalTime, cm, humanCount);
                }
                break;

            case MenuState::BUILDING:
                for (const auto& button : mainButtons)
                {
                    DrawBuildingButton(button, resourceMgr, costMgr, totalTime, cm, humanCount);
                }
                for (const auto& button : categoryButtons)
                {
                    DrawBuildingButton(button, resourceMgr, costMgr, totalTime, cm, humanCount);
                }
                for (const auto& button : buildingButtons)
                {
                    DrawBuildingButton(button, resourceMgr, costMgr, totalTime, cm, humanCount);
                }
                break;
        }

        if (escMenuOpen)
            DrawEscMenu();
    }

    void UIManager::DrawResources(ResourceManager* resourceMgr, CityMetrics* cm, int humanCount, const PopulationStats& popStats, int hour, int minute, int currentDay, int speedIndex, bool simPaused) const
    {
        if (!resourceMgr || !outlined)
            return;

        auto winSize = Engine::GetWindow().GetSize();

        constexpr float barHeight = 50.0f;
        const float     barY      = static_cast<float>(winSize.y) - barHeight;

        // Black background bar
        Dokev::DrawRectangle(0, static_cast<int>(barY), winSize.x, static_cast<int>(barHeight), 0x000000CC);

        const float y        = barY + 11.0f; // vertically center text in bar
        const float colWidth = static_cast<float>(winSize.x) / 5.0f;

        constexpr float iconSize = 2.0f;
        constexpr float iconGap  = 36.0f;
        constexpr float iconYOff = 4.0f;

        // Pulsing icon draw 
        auto drawIcon = [&](const Texture2D& tex, float bx, float by, int col)
        {
            if (!tex.IsValid()) return;
            float pulse = (hoveredIconColumn == col) ? 1.0f + 0.18f * std::sinf(totalTime * 9.0f) : 1.0f;
            float s     = iconSize * pulse;
            float adjX  = static_cast<float>(tex.GetWidth())  * iconSize * (1.0f - pulse) * 0.5f;
            float adjY  = static_cast<float>(tex.GetHeight()) * iconSize * (1.0f - pulse) * 0.5f;
            tex.DrawEx(static_cast<int>(bx + adjX), static_cast<int>(by + adjY), 0.0f, s, Dokev::ColorPalette::WHITE);
        };

        // Time  (column 0)
        {
            float x = colWidth * 0.0f + 20.0f;
            drawIcon(iconTime, x, y + iconYOff, 0);
            char buf[16];
            std::snprintf(buf, sizeof(buf), "%02d:%02d", hour, minute);
            if (auto tex = outlined->PrintToTexture(buf, Dokev::ColorPalette::WHITE))
                tex->Draw(Math::TranslationMatrix(Math::vec2{ x + iconGap, y }) * Math::ScaleMatrix(Math::vec2{ FONT_SCALE, FONT_SCALE }), Dokev::ColorPalette::WHITE);

            // Speed badge — small text right after time
            {
                const char* labels[]     = { "1x", "2x", "5x", "10x" };
                constexpr CS200::RGBA speedColors[] = { 0xFFFFFFFF, 0xFFFF00FF, 0xFF8800FF, 0xFF2222FF };
                std::string  speedLabel  = simPaused ? "||" : (speedIndex < 4 ? labels[speedIndex] : "1x");
                CS200::RGBA  badgeColor  = simPaused ? 0x88AAFFFF : speedColors[std::min(speedIndex, 3)];
                constexpr float badgeScale = 0.40f;
                if (auto badge = outlined->PrintToTexture(speedLabel, badgeColor))
                    badge->Draw(Math::TranslationMatrix(Math::vec2{ x + iconGap + 80.0f, y + 4.0f }) * Math::ScaleMatrix(Math::vec2{ badgeScale, badgeScale }), badgeColor);
            }

            // Click pressed effect (sunken overlay)
            if (timeClickAnimTimer > 0.0f)
            {
                float alpha  = timeClickAnimTimer / 0.12f; // 1.0 → 0.0
                auto  ov     = static_cast<uint8_t>(static_cast<int>(alpha * 80));
                CS200::RGBA pressColor = (static_cast<CS200::RGBA>(0) << 24) | (static_cast<CS200::RGBA>(0) << 16) | (static_cast<CS200::RGBA>(0) << 8) | ov;
                Dokev::DrawRectangle(0, static_cast<int>(barY), static_cast<int>(colWidth), 50, pressColor);
                // inset border (top/left dark, bottom/right white) for sunken look
                Dokev::DrawRectangleLinesEx(2, static_cast<int>(barY) + 2, static_cast<int>(colWidth) - 4, 46, 2.0f, 0x00000088);
            }

            // Day tooltip (when hovering)
            if (timeHoveredThisFrame)
            {
                std::string dayStr = "Day " + std::to_string(currentDay);
                float       tipW   = 120.0f;
                float       tipH   = 34.0f;
                float       tipX   = x;
                float       tipY   = barY - tipH - 4.0f;
                Dokev::DrawRectangle(static_cast<int>(tipX), static_cast<int>(tipY), static_cast<int>(tipW), static_cast<int>(tipH), 0x000000E6);
                Dokev::DrawRectangleLinesEx(static_cast<int>(tipX), static_cast<int>(tipY), static_cast<int>(tipW), static_cast<int>(tipH), 2.0f, 0xFFFFFFFF);
                if (auto tip = outlined->PrintToTexture(dayStr, Dokev::ColorPalette::WHITE))
                    tip->Draw(Math::TranslationMatrix(Math::vec2{ tipX + 8.0f, tipY + 5.0f }) * Math::ScaleMatrix(Math::vec2{ 0.42, 0.42 }), Dokev::ColorPalette::WHITE);
            }
        }

        // Money (column 1)
        {
            float x = colWidth * 1.0f + 20.0f;
            drawIcon(iconMoney, x, y + iconYOff, 1);
            std::string txt = std::to_string(resourceMgr->totalMoney);
            if (auto tex = outlined->PrintToTexture(txt, Dokev::ColorPalette::WHITE))
                tex->Draw(Math::TranslationMatrix(Math::vec2{ x + iconGap, y }) * Math::ScaleMatrix(Math::vec2{ FONT_SCALE, FONT_SCALE }), Dokev::ColorPalette::WHITE);

            // Finance popup — per-building breakdown
            if (moneyPopupOpen)
            {
                constexpr float popW  = 280.0f;
                constexpr float ts    = 0.35f;
                constexpr float lineH = 21.0f;
                constexpr float indentX = 18.0f;
                float popX = x - 10.0f;

                // Measure height based on actual entries
                auto sectionHeight = [&](const ResourceManager::DayStats& s) -> float
                {
                    float h = lineH; // section title
                    if (!s.income.empty())
                    {
                        h += lineH * 0.6f; // "Income:" sub-header
                        h += lineH * static_cast<float>(s.income.size());
                    }
                    if (!s.upkeep.empty())
                    {
                        h += 6.0f + lineH * 0.6f; // divider + "Upkeep:" sub-header
                        h += lineH * static_cast<float>(s.upkeep.size());
                    }
                    h += 8.0f + lineH; // divider + net
                    return h;
                };

                const auto& ld  = resourceMgr->lastDay;
                const auto& tod = resourceMgr->GetCurrentDayStats();
                float popH = sectionHeight(ld) + 10.0f + sectionHeight(tod) + 12.0f;
                float popY = barY - popH - 6.0f;

                Dokev::DrawRectangle(static_cast<int>(popX), static_cast<int>(popY),
                                     static_cast<int>(popW), static_cast<int>(popH), 0x000000EE);
                Dokev::DrawRectangleLinesEx(static_cast<int>(popX), static_cast<int>(popY),
                                            static_cast<int>(popW), static_cast<int>(popH), 2.0f, 0xFFD700FF);

                // NDC is y-up; mirror each row so the breakdown reads top-to-bottom.
                auto flipY = [&](float v) { return 2.0f * popY + popH - lineH - v; };

                auto printLine = [&](const std::string& label, int val, float ly, CS200::RGBA col, bool indent)
                {
                    std::string sign = (val >= 0) ? "+" : "";
                    std::string txt  = label + "  " + sign + std::to_string(val);
                    float       lx   = popX + (indent ? indentX : 8.0f);
                    if (auto t = outlined->PrintToTexture(txt, col))
                        t->Draw(Math::TranslationMatrix(Math::vec2{ lx, flipY(ly) }) * Math::ScaleMatrix(Math::vec2{ ts, ts }), col);
                };

                auto printHeader = [&](const std::string& txt, float ly, CS200::RGBA col)
                {
                    if (auto t = outlined->PrintToTexture(txt, col))
                        t->Draw(Math::TranslationMatrix(Math::vec2{ popX + 8.0f, flipY(ly) }) * Math::ScaleMatrix(Math::vec2{ ts * 0.9f, ts * 0.9f }), col);
                };

                auto drawSection = [&](const ResourceManager::DayStats& s, float startY, CS200::RGBA titleColor,
                                       const std::string& title) -> float
                {
                    float ly = startY;
                    if (auto t = outlined->PrintToTexture(title, titleColor))
                        t->Draw(Math::TranslationMatrix(Math::vec2{ popX + 8.0f, flipY(ly) }) * Math::ScaleMatrix(Math::vec2{ ts, ts }), titleColor);
                    ly += lineH;

                    if (!s.income.empty())
                    {
                        printHeader("Income:", ly, 0xAAFFAAFF);
                        ly += lineH * 0.6f;
                        for (const auto& [name, amt] : s.income)
                        {
                            printLine(name, amt, ly, 0x88FF88FF, true);
                            ly += lineH;
                        }
                    }

                    if (!s.upkeep.empty())
                    {
                        Dokev::DrawRectangle(static_cast<int>(popX + 8), static_cast<int>(flipY(ly + 2) + lineH),
                                             static_cast<int>(popW - 16), 1, 0xFFFFFF33);
                        ly += 6.0f;
                        printHeader("Upkeep:", ly, 0xFFAAAAFF);
                        ly += lineH * 0.6f;
                        for (const auto& [name, amt] : s.upkeep)
                        {
                            printLine(name, -amt, ly, 0xFF6666FF, true);
                            ly += lineH;
                        }
                    }

                    Dokev::DrawRectangle(static_cast<int>(popX + 8), static_cast<int>(flipY(ly + 2) + lineH),
                                         static_cast<int>(popW - 16), 1, 0xFFFFFF55);
                    ly += 8.0f;
                    int         net      = s.TotalIncome() - s.TotalUpkeep();
                    CS200::RGBA netColor = (net >= 0) ? 0x44FF44FF : 0xFF4444FF;
                    printLine("Net", net, ly, netColor, false);
                    ly += lineH;
                    return ly;
                };

                float ly = popY + 6.0f;
                ly = drawSection(ld,  ly, Dokev::ColorPalette::WHITE, "[ Yesterday ]");

                // Separator between sections
                Dokev::DrawRectangle(static_cast<int>(popX + 4), static_cast<int>(flipY(ly + 2) + lineH),
                                     static_cast<int>(popW - 8), 2, 0xFFD70066);
                ly += 10.0f;

                drawSection(tod, ly, 0xFFFF88FF, "[ Today ]");
            }
        }

        // Food   (column 2)
        {
            float x = colWidth * 2.0f + 20.0f;

            const int   foodAmount   = resourceMgr->GetFood();
            const int   foodCapacity = resourceMgr->GetFoodCapacity();
            const bool  foodLow      = foodCapacity > 0 && foodAmount * 5 < foodCapacity; // < 20%
            const bool  foodCritical = foodCapacity > 0 && foodAmount == 0;

            // Blink icon when food is low: alternate white↔red
            if (!foodLow || (std::fmod(totalTime, 0.5f) < 0.25f))
            {
                CS200::RGBA tint = foodCritical ? 0xFF2222FF : (foodLow ? 0xFF8800FF : Dokev::ColorPalette::WHITE);
                // apply pulse same as drawIcon but with custom tint
                float pulse = (hoveredIconColumn == 2) ? 1.0f + 0.18f * std::sinf(totalTime * 9.0f) : 1.0f;
                if (foodLow) pulse = 1.0f + 0.25f * std::abs(std::sinf(totalTime * 6.0f)); // stronger pulse when low
                float s    = iconSize * pulse;
                float adjX = static_cast<float>(iconFood.GetWidth())  * iconSize * (1.0f - pulse) * 0.5f;
                float adjY = static_cast<float>(iconFood.GetHeight()) * iconSize * (1.0f - pulse) * 0.5f;
                if (iconFood.IsValid())
                    iconFood.DrawEx(static_cast<int>(x + adjX), static_cast<int>(y + iconYOff + adjY), 0.0f, s, tint);
            }

            std::string txt   = std::to_string(foodAmount) + "/" + std::to_string(foodCapacity);
            CS200::RGBA color = foodCritical ? 0xFF2222FF : (foodLow ? 0xFF8800FF : Dokev::ColorPalette::WHITE);
            if (foodCapacity > 0 && foodAmount * 100 >= foodCapacity * kGranaryWarningThresholdPercent)
                color = Dokev::ColorPalette::ORANGE;
            if (auto tex = outlined->PrintToTexture(txt, color))
                tex->Draw(Math::TranslationMatrix(Math::vec2{ x + iconGap, y }) * Math::ScaleMatrix(Math::vec2{ FONT_SCALE, FONT_SCALE }), color);

            // Food popup
            if (foodPopupOpen)
            {
                constexpr float popW  = 260.0f;
                constexpr float ts    = 0.35f;
                constexpr float lineH = 21.0f;
                float popX = x - 10.0f;

                auto fSectionH = [&](const ResourceManager::FoodDayStats& s) -> float
                {
                    float h = lineH;
                    if (!s.production.empty())  { h += lineH * 0.6f + lineH * static_cast<float>(s.production.size()); }
                    if (!s.consumption.empty()) { h += 6.0f + lineH * 0.6f + lineH * static_cast<float>(s.consumption.size()); }
                    h += 8.0f + lineH;
                    return h;
                };

                const auto& lf  = resourceMgr->lastFoodDay;
                const auto& tfd = resourceMgr->GetCurrentFoodDayStats();
                float popH = fSectionH(lf) + 10.0f + fSectionH(tfd) + 12.0f;
                float popY = barY - popH - 6.0f;

                Dokev::DrawRectangle(static_cast<int>(popX), static_cast<int>(popY),
                                     static_cast<int>(popW), static_cast<int>(popH), 0x000000EE);
                Dokev::DrawRectangleLinesEx(static_cast<int>(popX), static_cast<int>(popY),
                                            static_cast<int>(popW), static_cast<int>(popH), 2.0f, 0x44CC44FF);

                auto flipY = [&](float v) { return 2.0f * popY + popH - lineH - v; };

                auto printLine = [&](const std::string& label, int val, float ly, CS200::RGBA col, bool indent)
                {
                    std::string sign = (val >= 0) ? "+" : "";
                    std::string txt2 = label + "  " + sign + std::to_string(val);
                    float       lx   = popX + (indent ? 18.0f : 8.0f);
                    if (auto t = outlined->PrintToTexture(txt2, col))
                        t->Draw(Math::TranslationMatrix(Math::vec2{ lx, flipY(ly) }) * Math::ScaleMatrix(Math::vec2{ ts, ts }), col);
                };
                auto printSubHeader = [&](const std::string& txt2, float ly, CS200::RGBA col)
                {
                    if (auto t = outlined->PrintToTexture(txt2, col))
                        t->Draw(Math::TranslationMatrix(Math::vec2{ popX + 8.0f, flipY(ly) }) * Math::ScaleMatrix(Math::vec2{ ts * 0.9f, ts * 0.9f }), col);
                };

                auto drawFoodSection = [&](const ResourceManager::FoodDayStats& s, float startY,
                                           CS200::RGBA titleColor, const std::string& title) -> float
                {
                    float ly = startY;
                    if (auto t = outlined->PrintToTexture(title, titleColor))
                        t->Draw(Math::TranslationMatrix(Math::vec2{ popX + 8.0f, flipY(ly) }) * Math::ScaleMatrix(Math::vec2{ ts, ts }), titleColor);
                    ly += lineH;

                    if (!s.production.empty())
                    {
                        printSubHeader("Production:", ly, 0xAAFFAAFF);
                        ly += lineH * 0.6f;
                        for (const auto& [name, amt] : s.production)
                        {
                            printLine(name, amt, ly, 0x88FF88FF, true);
                            ly += lineH;
                        }
                    }
                    if (!s.consumption.empty())
                    {
                        Dokev::DrawRectangle(static_cast<int>(popX + 8), static_cast<int>(flipY(ly + 2) + lineH),
                                             static_cast<int>(popW - 16), 1, 0xFFFFFF33);
                        ly += 6.0f;
                        printSubHeader("Consumed:", ly, 0xFFAAAAFF);
                        ly += lineH * 0.6f;
                        for (const auto& [name, amt] : s.consumption)
                        {
                            printLine(name, -amt, ly, 0xFF6666FF, true);
                            ly += lineH;
                        }
                    }
                    Dokev::DrawRectangle(static_cast<int>(popX + 8), static_cast<int>(flipY(ly + 2) + lineH),
                                         static_cast<int>(popW - 16), 1, 0xFFFFFF55);
                    ly += 8.0f;
                    int         net      = s.TotalProduction() - s.TotalConsumption();
                    CS200::RGBA netColor = (net >= 0) ? 0x44FF44FF : 0xFF4444FF;
                    printLine("Net", net, ly, netColor, false);
                    ly += lineH;
                    return ly;
                };

                float ly = popY + 6.0f;
                ly = drawFoodSection(lf,  ly, Dokev::ColorPalette::WHITE, "[ Yesterday ]");
                Dokev::DrawRectangle(static_cast<int>(popX + 4), static_cast<int>(flipY(ly + 2) + lineH),
                                     static_cast<int>(popW - 8), 2, 0x44CC4466);
                ly += 10.0f;
                drawFoodSection(tfd, ly, 0xAAFFAAFF, "[ Today ]");
            }
        }

        // Citizens (column 3)
        {
            float x = colWidth * 3.0f + 20.0f;
            drawIcon(iconHuman, x, y + iconYOff, 3);
            std::string txt = std::to_string(humanCount);
            if (auto tex = outlined->PrintToTexture(txt, Dokev::ColorPalette::WHITE))
                tex->Draw(Math::TranslationMatrix(Math::vec2{ x + iconGap, y }) * Math::ScaleMatrix(Math::vec2{ FONT_SCALE, FONT_SCALE }), Dokev::ColorPalette::WHITE);

            // Population popup
            if (popPopupOpen)
            {
                constexpr float popW  = 250.0f;
                constexpr float ts    = 0.37f;
                constexpr float lineH = 23.0f;
                float popX = x - 10.0f;
                // title + 8 stat rows + 3 dividers, then (if cm) immigration (hdr+3) + casino (hdr+1) + 2 dividers
                float popH = cm ? lineH * 14.0f + 5 * 8.0f + 18.0f
                                : lineH * 9.0f + 32.0f;
                float popY = barY - popH - 6.0f;

                Dokev::DrawRectangle(static_cast<int>(popX), static_cast<int>(popY),
                                     static_cast<int>(popW), static_cast<int>(popH), 0x000000EE);
                Dokev::DrawRectangleLinesEx(static_cast<int>(popX), static_cast<int>(popY),
                                            static_cast<int>(popW), static_cast<int>(popH), 2.0f, 0x88AAFFFF);
                                            
                auto flipY = [&](float v) { return 2.0f * popY + popH - lineH - v; };
                auto pline = [&](const std::string& label, const std::string& val, float ly, CS200::RGBA col)
                {
                    if (auto t = outlined->PrintToTexture(label + "  " + val, col))
                        t->Draw(Math::TranslationMatrix(Math::vec2{ popX + 14.0f, flipY(ly) }) * Math::ScaleMatrix(Math::vec2{ ts, ts }), col);
                };
                auto divider = [&](float ly)
                {
                    Dokev::DrawRectangle(static_cast<int>(popX + 8), static_cast<int>(flipY(ly) + lineH),
                                         static_cast<int>(popW - 16), 1, 0xFFFFFF33);
                };

                float ly = popY + 6.0f;

                if (auto t = outlined->PrintToTexture("[ Population ]", Dokev::ColorPalette::WHITE))
                    t->Draw(Math::TranslationMatrix(Math::vec2{ popX + 14.0f, flipY(ly) }) * Math::ScaleMatrix(Math::vec2{ ts, ts }), Dokev::ColorPalette::WHITE);
                ly += lineH;

                pline("Total:", std::to_string(popStats.total), ly, Dokev::ColorPalette::WHITE);
                ly += lineH;

                divider(ly); ly += 8.0f;

                CS200::RGBA homelessColor = (popStats.homeless > 0) ? 0xFF8800FF : Dokev::ColorPalette::WHITE;
                pline("Housed:", std::to_string(popStats.housed) + " / " + std::to_string(popStats.housingCap), ly, Dokev::ColorPalette::WHITE);
                ly += lineH;
                pline("Homeless:", std::to_string(popStats.homeless), ly, homelessColor);
                ly += lineH;

                divider(ly); ly += 8.0f;

                CS200::RGBA joblessColor = (popStats.unemployed > 0) ? 0xFFDD44FF : Dokev::ColorPalette::WHITE;
                pline("Employed:", std::to_string(popStats.employed), ly, Dokev::ColorPalette::WHITE);
                ly += lineH;
                pline("Jobless:", std::to_string(popStats.unemployed), ly, joblessColor);
                ly += lineH;

                divider(ly); ly += 8.0f;

                char happyBuf[16], energyBuf[16];
                std::snprintf(happyBuf, sizeof(happyBuf), "%.0f%%", popStats.avgHappiness);
                std::snprintf(energyBuf, sizeof(energyBuf), "%.0f%%", popStats.avgEnergy);

                CS200::RGBA happyColor = (popStats.avgHappiness >= 70.f) ? 0x44FF44FF :
                                         (popStats.avgHappiness >= 40.f) ? 0xFFDD44FF : 0xFF4444FF;
                CS200::RGBA energyColor = (popStats.avgEnergy >= 50.f) ? Dokev::ColorPalette::WHITE : 0xFF8800FF;

                pline("Avg Happy:", happyBuf, ly, happyColor);
                ly += lineH;
                pline("Avg Energy:", energyBuf, ly, energyColor);
                ly += lineH;

                // ---- Immigration & Casino conditions (need city metrics) ----
                if (cm)
                {
                    auto header = [&](const std::string& label, float hy)
                    {
                        if (auto t = outlined->PrintToTexture(label, 0x88CCFFFF))
                            t->Draw(Math::TranslationMatrix(Math::vec2{ popX + 14.0f, flipY(hy) }) * Math::ScaleMatrix(Math::vec2{ ts, ts }), 0x88CCFFFF);
                    };

                    const float happiness     = cm->GetHappiness();
                    const int   availableHomes = popStats.homeCapacity - popStats.total;
                    const int   totalResources = resourceMgr->GetTotalResources();
                    auto*       immi           = cm->GetImmigrationManager();
                    const float chance         = immi->CalculateImmigrationChance(availableHomes, happiness, humanCount, totalResources);
                    const float nextCheck      = immi->GetNextImmigrationTime();

                    divider(ly); ly += 8.0f;
                    header("[ Immigration ]", ly); ly += lineH;

                    char chanceBuf[40];
                    std::snprintf(chanceBuf, sizeof(chanceBuf), "%.0f%%  (next %.0fs)", chance * 100.0f, nextCheck);
                    CS200::RGBA chanceColor = (chance >= 0.6f) ? 0x44FF44FF :
                                              (chance >= 0.3f) ? 0xFFDD44FF : 0xFF4444FF;
                    pline("Chance:", chanceBuf, ly, chanceColor); ly += lineH;

                    if (availableHomes > 0)
                        pline("Housing:", std::to_string(availableHomes) + " free", ly, 0x44FF44FF);
                    else
                        pline("Housing:", "Full!", ly, 0xFF4444FF);
                    ly += lineH;

                    const char* happLabel = (happiness >= 60.f) ? "Good" : (happiness >= 40.f) ? "Low" : "Critical";
                    CS200::RGBA happLblColor = (happiness >= 60.f) ? 0x44FF44FF : (happiness >= 40.f) ? 0xFFDD44FF : 0xFF4444FF;
                    pline("Happiness:", happLabel, ly, happLblColor); ly += lineH;

                    // ---- Casino ----
                    divider(ly); ly += 8.0f;
                    header("[ Casino ]", ly); ly += lineH;

                    if (auto t = outlined->PrintToTexture("Needs night safety >= 70%", 0xFFDD44FF))
                        t->Draw(Math::TranslationMatrix(Math::vec2{ popX + 14.0f, flipY(ly) }) * Math::ScaleMatrix(Math::vec2{ ts, ts }), 0xFFDD44FF);
                }
            }
        }
    }

    void UIManager::DrawHappiness(CityMetrics* cm) const
    {
        if (!cm || !outlined)
            return;

        auto winSize = Engine::GetWindow().GetSize();

        constexpr float barHeight = 50.0f;
        const float colWidth = static_cast<float>(winSize.x) / 5.0f;
        const float barY = static_cast<float>(winSize.y) - barHeight;
        const float x    = colWidth * 4.0f + 20.0f;
        const float y    = barY + 11.0f;

        constexpr float iconSize = 2.0f;
        constexpr float iconGap  = 36.0f;
        constexpr float iconYOff = 4.0f;

        float happiness = cm->GetHappiness();
        CS200::RGBA happColor = (happiness >= 70.f) ? Dokev::ColorPalette::WHITE :
                                (happiness >= 40.f) ? 0xFFDD44FF : 0xFF4444FF;

        if (iconHappiness.IsValid())
        {
            float pulse = (hoveredIconColumn == 4) ? 1.0f + 0.18f * std::sinf(totalTime * 9.0f) : 1.0f;
            float s     = iconSize * pulse;
            float adjX  = static_cast<float>(iconHappiness.GetWidth())  * iconSize * (1.0f - pulse) * 0.5f;
            float adjY  = static_cast<float>(iconHappiness.GetHeight()) * iconSize * (1.0f - pulse) * 0.5f;
            iconHappiness.DrawEx(static_cast<int>(x + adjX), static_cast<int>(y + iconYOff + adjY), 0.0f, s, happColor);
        }

        std::string txt = std::to_string(static_cast<int>(happiness)) + "%";
        if (auto tex = outlined->PrintToTexture(txt, happColor))
            tex->Draw(Math::TranslationMatrix(Math::vec2{ x + iconGap, y }) * Math::ScaleMatrix(Math::vec2{ FONT_SCALE, FONT_SCALE }), happColor);

        // Happiness popup
        if (happPopupOpen)
        {
            const auto  factors = cm->GetHappinessManager()->GetFactors();
            constexpr float popW  = 270.0f;
            constexpr float popH  = 210.0f;
            constexpr float ts    = 0.37f;
            constexpr float lineH = 23.0f;
            float popX = x - 30.0f;
            float popY = barY - popH - 6.0f;

            Dokev::DrawRectangle(static_cast<int>(popX), static_cast<int>(popY),
                                 static_cast<int>(popW), static_cast<int>(popH), 0x000000EE);
            Dokev::DrawRectangleLinesEx(static_cast<int>(popX), static_cast<int>(popY),
                                        static_cast<int>(popW), static_cast<int>(popH), 2.0f, 0xFFDD44FF);

            auto flipY = [&](float v) { return 2.0f * popY + popH - lineH - v; };

            auto scoreColor = [](float s) -> CS200::RGBA {
                return (s >= 80.f) ? 0x44FF44FF : (s >= 50.f) ? 0xFFDD44FF : 0xFF4444FF;
            };
            auto hint = [](float s) -> const char* {
                return (s >= 80.f) ? "Good" : (s >= 50.f) ? "Fair" : "Poor";
            };

            auto pline = [&](const std::string& label, float score, float weight, float ly)
            {
                char buf[64];
                std::snprintf(buf, sizeof(buf), "%.0f%%  [%s]", score, hint(score));
                CS200::RGBA col = scoreColor(score);

                std::string weightStr = "x" + std::to_string(static_cast<int>(weight * 100)) + "%  ";
                if (auto t = outlined->PrintToTexture(label + "  " + buf, col))
                    t->Draw(Math::TranslationMatrix(Math::vec2{ popX + 14.0f, flipY(ly) }) * Math::ScaleMatrix(Math::vec2{ ts, ts }), col);
                if (auto t = outlined->PrintToTexture(weightStr, 0xAAAAAAFF))
                    t->Draw(Math::TranslationMatrix(Math::vec2{ popX + popW - 70.0f, flipY(ly) }) * Math::ScaleMatrix(Math::vec2{ ts * 0.8f, ts * 0.8f }), 0xAAAAAAFF);
            };

            float ly = popY + 6.0f;

            if (auto t = outlined->PrintToTexture("[ Happiness ]", Dokev::ColorPalette::WHITE))
                t->Draw(Math::TranslationMatrix(Math::vec2{ popX + 14.0f, flipY(ly) }) * Math::ScaleMatrix(Math::vec2{ ts, ts }), Dokev::ColorPalette::WHITE);
            ly += lineH;

            char overallBuf[16];
            std::snprintf(overallBuf, sizeof(overallBuf), "%.0f%%", happiness);
            if (auto t = outlined->PrintToTexture(std::string("Overall:  ") + overallBuf, happColor))
                t->Draw(Math::TranslationMatrix(Math::vec2{ popX + 14.0f, flipY(ly) }) * Math::ScaleMatrix(Math::vec2{ ts, ts }), happColor);
            ly += lineH;

            Dokev::DrawRectangle(static_cast<int>(popX + 8), static_cast<int>(flipY(ly + 2) + lineH),
                                 static_cast<int>(popW - 16), 1, 0xFFFFFF33);
            ly += 10.0f;

            pline("Housing    ", factors.housingScore,     0.40f, ly); ly += lineH;
            pline("Employment ", factors.employmentScore,  0.40f, ly); ly += lineH;
            pline("Night Safe ", factors.nightSafetyScore, 0.20f, ly); ly += lineH;

            Dokev::DrawRectangle(static_cast<int>(popX + 8), static_cast<int>(flipY(ly + 2) + lineH),
                                 static_cast<int>(popW - 16), 1, 0xFFFFFF33);
            ly += 10.0f;

            // Tip: worst factor
            float worst = std::min({ factors.housingScore, factors.employmentScore, factors.nightSafetyScore });
            const char* tip = (worst == factors.housingScore)     ? "! Build more homes"      :
                              (worst == factors.employmentScore)   ? "! Build more workshops"  :
                                                                     "! Add street lights";
            if (auto t = outlined->PrintToTexture(tip, 0xFF8800FF))
                t->Draw(Math::TranslationMatrix(Math::vec2{ popX + 14.0f, flipY(ly) }) * Math::ScaleMatrix(Math::vec2{ ts * 0.85f, ts * 0.85f }), 0xFF8800FF);
        }
    }

    void UIManager::DrawEscMenu() const
    {
        if (!outlined)
            return;

        auto ws = Engine::GetWindow().GetSize();
        auto  l = GetEscMenuLayout(ws.x, ws.y);

        // Fullscreen dim overlay
        Dokev::DrawRectangle(0, 0, ws.x, ws.y, 0x00000099);

        // Panel background + border
        Dokev::DrawRectangle(static_cast<int>(l.panelX), static_cast<int>(l.panelY),
                             static_cast<int>(l.panelW), static_cast<int>(l.panelH), 0x12122AEE);
        Dokev::DrawRectangleLinesEx(static_cast<int>(l.panelX), static_cast<int>(l.panelY),
                                    static_cast<int>(l.panelW), static_cast<int>(l.panelH), 2.0f, 0xFFFFFFCC);

        // Title
        constexpr double ts = 1.0;
        float cx = ws.x * 0.5f;
        if (auto t = outlined->PrintToTexture("PAUSED", 0xFFFFFFFF))
            t->Draw(Math::TranslationMatrix(Math::vec2{ cx - t->GetSize().x * ts * 0.5, l.titleY })
                  * Math::ScaleMatrix(Math::vec2{ ts, ts }), 0xFFFFFFFF);

        // Separator
        float sepY = l.panelY + 68.0f;
        Dokev::DrawRectangle(static_cast<int>(l.panelX + 12), static_cast<int>(sepY),
                             static_cast<int>(l.panelW - 24), 1, 0xFFFFFF55);

        // Buttons
        static const char* labels[kEscButtonCount] = { "Resume", "Save", "Load", "Settings", "Main Menu", "Quit" };

        auto lerpColor = [](CS200::RGBA a, CS200::RGBA b, float t) -> CS200::RGBA
        {
            t = std::clamp(t, 0.0f, 1.0f);
            auto ch = [&](int shift) -> unsigned
            {
                float av = static_cast<float>((a >> shift) & 0xFF);
                float bv = static_cast<float>((b >> shift) & 0xFF);
                return static_cast<unsigned>(av + (bv - av) * t) & 0xFFu;
            };
            return (ch(24) << 24) | (ch(16) << 16) | (ch(8) << 8) | ch(0);
        };

        for (int i = 0; i < kEscButtonCount; ++i)
        {
            float hov   = escHoverAnim[i]; // 0..1 eased
            float press = escPressAnim[i]; // 0..1 eased

            // Grow a touch on hover, then push/shrink in on press
            float scale = 1.0f + 0.05f * hov - 0.06f * press;
            float pushY = 3.0f * press; // sink down when pressed

            float bw = l.btnW * scale;
            float bh = l.btnH * scale;
            float bx = l.btnX + (l.btnW - bw) * 0.5f;
            float by = l.btnY[i] + (l.btnH - bh) * 0.5f + pushY;

            // Background: idle -> hover -> pressed (accent)
            CS200::RGBA bg = lerpColor(0x22223ACC, 0x3E3E66FF, hov);
            bg             = lerpColor(bg, 0x5A5AA0FF, press);
            Dokev::DrawRectangle(static_cast<int>(bx), static_cast<int>(by),
                                 static_cast<int>(bw), static_cast<int>(bh), bg);

            // Brighter left accent bar that widens on press for extra punch
            if (hov > 0.01f || press > 0.01f)
            {
                int         accentW = static_cast<int>(3.0f + 3.0f * press);
                CS200::RGBA accent  = lerpColor(0x6E8BFFCC, 0xBFD0FFFF, press);
                Dokev::DrawRectangle(static_cast<int>(bx), static_cast<int>(by),
                                     accentW, static_cast<int>(bh), accent);
            }

            // Border brightens with hover, full white + thicker on press
            CS200::RGBA border      = lerpColor(0xFFFFFF66, 0xFFFFFFFF, std::max(hov, press));
            float       borderThick = 1.5f + 1.0f * hov + 0.5f * press;
            Dokev::DrawRectangleLinesEx(static_cast<int>(bx), static_cast<int>(by),
                                        static_cast<int>(bw), static_cast<int>(bh), borderThick, border);

            if (auto t = outlined->PrintToTexture(labels[i], 0xFFFFFFFF))
            {
                float lblScale = static_cast<float>(ts) * scale;
                float tx       = cx - static_cast<float>(t->GetSize().x) * lblScale * 0.5f;
                float ty       = by + (bh - static_cast<float>(t->GetSize().y) * lblScale) * 0.5f;
                t->Draw(Math::TranslationMatrix(Math::vec2{ tx, ty })
                      * Math::ScaleMatrix(Math::vec2{ lblScale, lblScale }), 0xFFFFFFFF);
            }
        }
    }

    void UIManager::DrawBox(Math::fvec2 position, Math::fvec2 size, CS200::RGBA color) const
    {
        DrawRectangle(static_cast<int>(position.x), static_cast<int>(position.y), static_cast<int>(size.x), static_cast<int>(size.y), color);
    }

    void UIManager::DrawBuildingButton(const BuildingButton& button, ResourceManager* resourceMgr, BuildingCostManager* costMgr, float time, CityMetrics* cm, int humanCount) const
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

        // Draw Building Texture
        if (button.texture.IsValid())
        {
            int textureWidth  = button.texture.GetWidth();
            int textureHeight = button.texture.GetHeight();

            float iconScaleMult = (button.type == BuildingType::FARM || button.type == BuildingType::GRANARY || button.type == BuildingType::PARK ||
                                   button.type == BuildingType::WORKPLACE3 || button.type == BuildingType::DISASTER_OBSERVATORY) ? 0.75f : 1.0f;
            float iconScale    = static_cast<float>(ICON_SCALE) * scale * iconScaleMult;
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
                                 button.type == BuildingType::WORKPLACE3 || button.type == BuildingType::DISASTER_OBSERVATORY ||
                                 button.type == BuildingType::CASINO || button.type == BuildingType::PENTHOUSE ||
                                 button.type == BuildingType::PARK))
        {
            bool  isPenthouse   = (button.type == BuildingType::PENTHOUSE);
            float tooltipWidth  = isPenthouse ? 260.0f : 220.0f;
            float tooltipHeight = isPenthouse ? 110.0f : 80.0f;

            // Add space for hotkey display
            std::string hotkey = GetHotkeyForButton(button);
            if (!hotkey.empty() && !isPenthouse)
            {
                tooltipHeight += 20.0f;
            }

            Math::fvec2 tooltipPos = Math::fvec2(button.position.x - tooltipWidth - 10.0f, button.position.y);

            DrawBox(tooltipPos, Math::fvec2(tooltipWidth, tooltipHeight), Dokev::ColorPalette::BLACK);

            DrawRectangleLinesEx(static_cast<int>(tooltipPos.x), static_cast<int>(tooltipPos.y), static_cast<int>(tooltipWidth), static_cast<int>(tooltipHeight), 3.0f, Dokev::ColorPalette::WHITE);

            if (outlined && resourceMgr && costMgr)
            {
                BuildingCost cost      = costMgr->GetCost(button.type);
                float        lineScale = 0.40f;

                if (auto nameTexture = outlined->PrintToTexture(button.name, Dokev::ColorPalette::WHITE))
                {
                    float nameScale = 0.48f;
                    float centeredX = tooltipPos.x + (tooltipWidth - nameTexture->GetSize().x * nameScale) / 2.0f;
                    nameTexture->Draw(
                        Math::TranslationMatrix(Math::vec2{ centeredX, tooltipPos.y + 12.0f }) * Math::ScaleMatrix(Math::vec2{ nameScale, nameScale }),
                        Dokev::ColorPalette::WHITE);
                }

                if (isPenthouse)
                {
                    float happiness     = cm ? cm->GetHappiness() : 0.0f;
                    auto  moneyColor    = (resourceMgr->totalMoney >= 1000) ? Dokev::ColorPalette::GREEN : Dokev::ColorPalette::RED;
                    auto  popColor      = (humanCount >= 30) ? Dokev::ColorPalette::GREEN : Dokev::ColorPalette::RED;
                    auto  happyColor    = (happiness >= 80.0f) ? Dokev::ColorPalette::GREEN : Dokev::ColorPalette::RED;

                    auto drawCond = [&](const std::string& text, CS200::RGBA color, float y)
                    {
                        if (auto tex = outlined->PrintToTexture(text, color))
                        {
                            float cx = tooltipPos.x + (tooltipWidth - tex->GetSize().x * lineScale) / 2.0f;
                            tex->Draw(Math::TranslationMatrix(Math::vec2{ cx, y }) * Math::ScaleMatrix(Math::vec2{ lineScale, lineScale }), color);
                        }
                    };

                    drawCond("Money: " + std::to_string(resourceMgr->totalMoney) + " / 1000", moneyColor, tooltipPos.y + 42.0f);
                    drawCond("Pop: " + std::to_string(humanCount) + " / 30",                  popColor,   tooltipPos.y + 64.0f);
                    drawCond("Happiness: " + std::to_string(static_cast<int>(happiness)) + " / 80", happyColor, tooltipPos.y + 86.0f);
                }
                else
                {
                    if (cost.money > 0)
                    {
                        std::string moneyText = "Money: " + std::to_string(resourceMgr->totalMoney) + " / " + std::to_string(cost.money);
                        if (auto textTexture = outlined->PrintToTexture(moneyText, Dokev::ColorPalette::WHITE))
                        {
                            float centeredX = tooltipPos.x + (tooltipWidth - textTexture->GetSize().x * lineScale) / 2.0f;
                            textTexture->Draw(
                                Math::TranslationMatrix(Math::vec2{ centeredX, tooltipPos.y + 42.0f }) * Math::ScaleMatrix(Math::vec2{ lineScale, lineScale }),
                                Dokev::ColorPalette::WHITE);
                        }
                    }

                    if (!hotkey.empty())
                    {
                        float hotkeyScale = 0.35f;
                        float hotkeyY     = tooltipPos.y + tooltipHeight - 22.0f;

                        auto labelTexture = outlined->PrintToTexture("Hotkey: ", Dokev::ColorPalette::WHITE);
                        auto keyTexture   = outlined->PrintToTexture(hotkey, Dokev::ColorPalette::GREEN);

                        if (labelTexture && keyTexture)
                        {
                            float totalW  = (labelTexture->GetSize().x + keyTexture->GetSize().x) * hotkeyScale;
                            float hotkeyX = tooltipPos.x + (tooltipWidth - totalW) / 2.0f;
                            labelTexture->Draw(Math::TranslationMatrix(Math::vec2{ hotkeyX, hotkeyY }) * Math::ScaleMatrix(Math::vec2{ hotkeyScale, hotkeyScale }), Dokev::ColorPalette::WHITE);
                            hotkeyX += labelTexture->GetSize().x * hotkeyScale;
                            keyTexture->Draw(Math::TranslationMatrix(Math::vec2{ hotkeyX, hotkeyY }) * Math::ScaleMatrix(Math::vec2{ hotkeyScale, hotkeyScale }), Dokev::ColorPalette::GREEN);
                        }
                    }
                }
            }
        }
        else if (button.isHovered && !button.name.empty())
        {
            std::string tooltipText   = button.name;
            float       textWidth     = static_cast<float>(tooltipText.length()) * 9.0f;
            float       tooltipWidth  = textWidth + 20.0f;
            float       tooltipHeight = 38.0f;

            // Add space for hotkey display
            std::string hotkey = GetHotkeyForButton(button);
            if (!hotkey.empty())
            {
                tooltipHeight += 18.0f;
            }

            if (tooltipWidth < 130.0f)
            {
                tooltipWidth = 130.0f;
            }

            Math::fvec2 tooltipPos = Math::fvec2(button.position.x - tooltipWidth - 10.0f, button.position.y);

            DrawBox(tooltipPos, Math::fvec2(tooltipWidth, tooltipHeight), Dokev::ColorPalette::BLACK);
            DrawRectangleLinesEx(static_cast<int>(tooltipPos.x), static_cast<int>(tooltipPos.y), static_cast<int>(tooltipWidth), static_cast<int>(tooltipHeight), 3.0f, Dokev::ColorPalette::WHITE);

            if (outlined)
            {
                if (auto nameTexture = outlined->PrintToTexture(tooltipText, Dokev::ColorPalette::WHITE))
                {
                    float nameScale = 0.48f;
                    float centeredX = tooltipPos.x + (tooltipWidth - nameTexture->GetSize().x * nameScale) / 2.0f;
                    nameTexture->Draw(Math::TranslationMatrix(Math::vec2{ centeredX, tooltipPos.y + 8.0f }) * Math::ScaleMatrix(Math::vec2{ nameScale, nameScale }), Dokev::ColorPalette::WHITE);
                }

                // Draw hotkey at the bottom (centered)
                if (!hotkey.empty())
                {
                    float hotkeyScale = 0.35f;
                    float hotkeyY     = tooltipPos.y + tooltipHeight - 20.0f;

                    auto labelTexture = outlined->PrintToTexture("Hotkey: ", Dokev::ColorPalette::WHITE);
                    auto keyTexture   = outlined->PrintToTexture(hotkey, Dokev::ColorPalette::GREEN);

                    if (labelTexture && keyTexture)
                    {
                        float totalW  = (labelTexture->GetSize().x + keyTexture->GetSize().x) * hotkeyScale;
                        float hotkeyX = tooltipPos.x + (tooltipWidth - totalW) / 2.0f;
                        labelTexture->Draw(Math::TranslationMatrix(Math::vec2{ hotkeyX, hotkeyY }) * Math::ScaleMatrix(Math::vec2{ hotkeyScale, hotkeyScale }), Dokev::ColorPalette::WHITE);
                        hotkeyX += labelTexture->GetSize().x * hotkeyScale;
                        keyTexture->Draw(Math::TranslationMatrix(Math::vec2{ hotkeyX, hotkeyY }) * Math::ScaleMatrix(Math::vec2{ hotkeyScale, hotkeyScale }), Dokev::ColorPalette::GREEN);
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
