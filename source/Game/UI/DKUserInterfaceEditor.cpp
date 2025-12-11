/**
 * \file UserInterfaceEditor.cpp
 * \brief
 * \author
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "DKUserInterfaceEditor.hpp"
#include "../Managers/HappinessManager.hpp"
#include "../Managers/DKImmigrationManager.hpp"
#include "Dokev/Managers/DKGameObjectManager.hpp"
#include "../Entities/Building.hpp"
#include "../Common/BuildingType.hpp"
#include "Dokev/Core/DKMetaProgramming.hpp"
#include <imgui.h>

void UserInterfaceEditor::DrawUI(HappinessManager* happinessManager, Dokev::ImmigrationManager* immigrationManager,
                                 Dokev::GameObjectManager* gameObjectManager)
{
    if (!happinessManager || !immigrationManager || !gameObjectManager)
        return;

    int humanCount     = static_cast<int>(gameObjectManager->GetHumans().size());
    int homeCapacity   = 0;
    int totalHouses    = 0;
    int workplaceCount = 0;

    for (const auto& building : gameObjectManager->GetBuildings())
    {
        BuildingType type = building->GetBuildingType();

        if (Dokev::Meta::is_residential_type(type))
        {
            totalHouses++;
            homeCapacity += building->GetMaxCapacity();
        }

        if (Dokev::Meta::is_workshop_type(type))
        {
            workplaceCount++;
        }
    }

    int availableHomes = homeCapacity - humanCount;
    int totalResources = 0; 

    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_FirstUseEver);
    ImGui::Begin("City Status", nullptr, ImGuiWindowFlags_NoCollapse);

    DrawHappinessSection(happinessManager);
    ImGui::Separator();
    DrawImmigrationSection(immigrationManager, happinessManager, availableHomes, totalResources, humanCount);
    ImGui::Separator();
    DrawPopulationSection(gameObjectManager, homeCapacity, totalHouses, workplaceCount, humanCount);

    ImGui::End();
}

void UserInterfaceEditor::DrawHappinessSection(HappinessManager* happinessManager)
{
    float totalHappiness = happinessManager->CalculateTotalHappiness();
    auto  factors        = happinessManager->GetFactors();

    //  Section Title 
    ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "CITY HAPPINESS");

    //  Overall Happiness 
    ImGui::Text("Overall:");
    ImGui::SameLine();

    ImVec4 color;
    if (totalHappiness >= 80.0f)
        color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // Green
    else if (totalHappiness >= 60.0f)
        color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
    else if (totalHappiness >= 40.0f)
        color = ImVec4(1.0f, 0.5f, 0.0f, 1.0f); // Orange
    else
        color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red

    ImGui::TextColored(color, "%.0f%%", totalHappiness);

    ImGui::Spacing();

    ImGui::Text("Factors:");

    // Housing
    ImGui::Text("  Housing:    ");
    ImGui::SameLine();
    if (factors.housingScore >= 80.0f)
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "%.0f%%", factors.housingScore);
    else if (factors.housingScore >= 50.0f)
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "%.0f%%", factors.housingScore);
    else
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "%.0f%%", factors.housingScore);

    // Employment
    ImGui::Text("  Employment: ");
    ImGui::SameLine();
    if (factors.employmentScore >= 80.0f)
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "%.0f%%", factors.employmentScore);
    else if (factors.employmentScore >= 50.0f)
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "%.0f%%", factors.employmentScore);
    else
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "%.0f%%", factors.employmentScore);

    // Water
    ImGui::Text("  Water:      ");
    ImGui::SameLine();
    if (factors.waterScore >= 80.0f)
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "%.0f%%", factors.waterScore);
    else if (factors.waterScore >= 50.0f)
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "%.0f%%", factors.waterScore);
    else
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "%.0f%%", factors.waterScore);
}

void UserInterfaceEditor::DrawImmigrationSection(Dokev::ImmigrationManager* immigrationManager, HappinessManager* happinessManager,
                                                  int availableHomes, int totalResources, int humanCount)
{
    //  Section Title 
    ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "IMMIGRATION");

    //  Next check timer 
    float timeLeft = immigrationManager->GetNextImmigrationTime();
    ImGui::Text("Next check: %.1f sec", timeLeft);

    //  Current chance 
    float happiness = happinessManager->CalculateTotalHappiness();
    float chance    = immigrationManager->CalculateImmigrationChance(availableHomes, happiness, humanCount, totalResources);

    ImGui::Text("Chance:");
    ImGui::SameLine();

    // Color code the chance
    if (chance >= 0.6f)
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "%.0f%%", chance * 100.0f);
    else if (chance >= 0.3f)
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "%.0f%%", chance * 100.0f);
    else
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "%.0f%%", chance * 100.0f);

    // Progress bar
    ImGui::ProgressBar(chance, ImVec2(-1, 0));

    ImGui::Spacing();

    //  Requirements 
    ImGui::Text("Requirements:");

    // Housing check
    ImGui::Text("  Housing:");
    ImGui::SameLine();
    if (availableHomes > 0)
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "%d slots", availableHomes);
    else
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Full!");

    // Happiness check
    ImGui::Text("  Happiness:");
    ImGui::SameLine();
    if (happiness >= 60.0f)
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "Good");
    else if (happiness >= 40.0f)
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Low");
    else
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Critical");
}

void UserInterfaceEditor::DrawPopulationSection(Dokev::GameObjectManager* gameObjectManager, int homeCapacity,
                                                 int totalHouses, int workplaceCount, int humanCount)
{
    (void)gameObjectManager; //unused

    //  Section Title 
    ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "POPULATION");

    //  Population 
    ImGui::Text("Citizens: %d", humanCount);

    //  Housing 
    ImGui::Text("Housing:");
    ImGui::SameLine();
    if (homeCapacity >= humanCount)
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "%d / %d", humanCount, homeCapacity);
    else
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "%d / %d", humanCount, homeCapacity);

    //  Buildings 
    ImGui::Text("Homes: %d", totalHouses);
    ImGui::Text("Workplaces: %d", workplaceCount);
}
