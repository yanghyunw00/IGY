/**
 * \file DKUIManager.cpp
 * \brief UI Manager implementation for building selection interface
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "DKUIManager.hpp"
#include "DKShapes.hpp"
#include "DKCollision.hpp"
#include "Engine/Window.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Path.hpp"
#include "Game/ResourceManager.hpp"
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

        try
        {
            woodIcon = LoadTexture(assets::locate_asset("Assets/resources/tree_1.png").string());
        }
        catch (...)
        {
            printf("Failed to load wood icon\n");
        }

        try
        {
            rockIcon = LoadTexture(assets::locate_asset("Assets/resources/rock_1.png").string());
        }
        catch (...)
        {
            printf("Failed to load rock icon\n");
        }

        try
        {
            steelIcon = LoadTexture(assets::locate_asset("Assets/resources/steel_1.png").string());
        }
        catch (...)
        {
            printf("Failed to load steel icon\n");
        }
    }

    void UIManager::CreateBuildingButtons()
    {
        buildingButtons.clear();

        auto windowSize = Engine::GetWindow().GetSize();

        float startX = static_cast<float>(windowSize.x) - BUTTON_WIDTH - UI_MARGIN;
        float startY = UI_MARGIN;
        BuildingButton homeButton;
        homeButton.type = BuildingType::HOME;
        homeButton.texture = LoadTexture(assets::locate_asset("Assets/images/textures/home.png").string());
        homeButton.position = Math::fvec2(startX, startY);
        homeButton.size = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
        homeButton.name = "Home";
        homeButton.tint = CS200::WHITE;
        buildingButtons.push_back(homeButton);
        BuildingButton woodButton;
        woodButton.type = BuildingType::WOOD_WORKSHOP;
        woodButton.texture = LoadTexture(assets::locate_asset("Assets/images/textures/work.png").string());
        woodButton.position = Math::fvec2(startX, startY + (BUTTON_HEIGHT + BUTTON_SPACING));
        woodButton.size = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
        woodButton.name = "Wood Workshop";
        woodButton.tint = CS200::pack_color({0.6f, 0.4f, 0.2f, 1.0f});
        buildingButtons.push_back(woodButton);
        BuildingButton rockButton;
        rockButton.type = BuildingType::ROCK_WORKSHOP;
        rockButton.texture = LoadTexture(assets::locate_asset("Assets/images/textures/work.png").string());
        rockButton.position = Math::fvec2(startX, startY + (BUTTON_HEIGHT + BUTTON_SPACING) * 2);
        rockButton.size = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
        rockButton.name = "Rock Workshop";
        rockButton.tint = CS200::pack_color({0.5f, 0.5f, 0.5f, 1.0f});
        buildingButtons.push_back(rockButton);
        BuildingButton steelButton;
        steelButton.type = BuildingType::STEEL_WORKSHOP;
        steelButton.texture = LoadTexture(assets::locate_asset("Assets/images/textures/work.png").string());
        steelButton.position = Math::fvec2(startX, startY + (BUTTON_HEIGHT + BUTTON_SPACING) * 3);
        steelButton.size = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
        steelButton.name = "Steel Workshop";
        steelButton.tint = CS200::pack_color({0.7f, 0.7f, 0.8f, 1.0f});
        buildingButtons.push_back(steelButton);
        BuildingButton roadButton;
        roadButton.type = BuildingType::ROAD;
        roadButton.texture = LoadTexture(assets::locate_asset("Assets/images/textures/road.png").string());
        roadButton.position = Math::fvec2(startX, startY + (BUTTON_HEIGHT + BUTTON_SPACING) * 4);
        roadButton.size = Math::fvec2(BUTTON_WIDTH, BUTTON_HEIGHT);
        roadButton.name = "Road";
        roadButton.tint = CS200::WHITE;
        buildingButtons.push_back(roadButton);
    }

    void UIManager::Update(Math::fvec2 mousePos, bool mouseClicked)
    {
        for (auto& button : buildingButtons)
        {
            button.isHovered = PointInRect(mousePos, button.position, button.size);

            if (button.isHovered && mouseClicked)
            {
                if (selectedBuilding == button.type)
                {
                    selectedBuilding = BuildingType::NONE;
                }
                else
                {
                    selectedBuilding = button.type;
                }
            }

            button.isSelected = (button.type == selectedBuilding);
        }
    }

    void UIManager::Draw(ResourceManager* resourceMgr) const
    {
        DrawResources(resourceMgr);

        for (const auto& button : buildingButtons)
        {
            DrawBuildingButton(button);
        }
    }

    void UIManager::DrawResources(ResourceManager* resourceMgr) const
    {
        if (!resourceMgr)
            return;

        auto windowSize = Engine::GetWindow().GetSize();
        float startX = static_cast<float>(windowSize.x) - BUTTON_WIDTH - UI_MARGIN;
        float resourceY = static_cast<float>(windowSize.y) - 120.0f;

        constexpr float ICON_SIZE = 28.0f;
        constexpr float SPACING = 5.0f;
        constexpr float LINE_HEIGHT = ICON_SIZE + SPACING;

        float currentX = startX;
        float currentY = resourceY;

        if (woodIcon.IsValid())
        {
            woodIcon.DrawEx(static_cast<int>(currentX), static_cast<int>(currentY),
                           0.0f, ICON_SIZE / woodIcon.GetWidth(), CS200::WHITE);
        }
        currentY -= LINE_HEIGHT;

        if (rockIcon.IsValid())
        {
            rockIcon.DrawEx(static_cast<int>(currentX), static_cast<int>(currentY),
                           0.0f, ICON_SIZE / rockIcon.GetWidth(), CS200::WHITE);
        }
        currentY -= LINE_HEIGHT;

        if (steelIcon.IsValid())
        {
            steelIcon.DrawEx(static_cast<int>(currentX), static_cast<int>(currentY),
                           0.0f, ICON_SIZE / steelIcon.GetWidth(), CS200::WHITE);
        }
    }

    void UIManager::DrawBox(Math::fvec2 position, Math::fvec2 size, CS200::RGBA color) const
    {
        DrawRectangle(
            static_cast<int>(position.x),
            static_cast<int>(position.y),
            static_cast<int>(size.x),
            static_cast<int>(size.y),
            color
        );
    }

    void UIManager::DrawBuildingButton(const BuildingButton& button) const
    {
        CS200::RGBA boxColor = CS200::pack_color({0.3f, 0.3f, 0.3f, 0.8f});

        if (button.isSelected)
        {
            boxColor = CS200::pack_color({0.2f, 0.6f, 0.2f, 0.9f});
        }
        else if (button.isHovered)
        {
            boxColor = CS200::pack_color({0.5f, 0.5f, 0.5f, 0.9f});
        }

        DrawBox(button.position, button.size, boxColor);

        CS200::RGBA borderColor = button.isSelected ?
            CS200::pack_color({0.3f, 0.8f, 0.3f, 1.0f}) :
            CS200::pack_color({0.6f, 0.6f, 0.6f, 1.0f});

        DrawRectangleLinesEx(
            static_cast<int>(button.position.x),
            static_cast<int>(button.position.y),
            static_cast<int>(button.size.x),
            static_cast<int>(button.size.y),
            3.0f,
            borderColor
        );

        if (button.texture.IsValid())
        {
            constexpr float PADDINGX = 30.0f;
            constexpr float PADDINGY = 20.0f;

            float imageScale = 0.7f;
            int textureWidth = button.texture.GetWidth();
            int textureHeight = button.texture.GetHeight();

            float scaledWidth = textureWidth * imageScale;
            float scaledHeight = textureHeight * imageScale;

            Math::fvec2 imagePos = Math::fvec2(
                button.position.x - PADDINGX + (button.size.x - scaledWidth) / 2.0f ,
                button.position.y - PADDINGY + (button.size.y - scaledHeight) / 2.0f
            );

            button.texture.DrawEx(
                static_cast<int>(imagePos.x),
                static_cast<int>(imagePos.y),
                0.0f,
                imageScale,
                button.tint
            );
        }
    }
}
