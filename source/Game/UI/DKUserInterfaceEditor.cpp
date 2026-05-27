/**
 * \file UserInterfaceEditor.cpp
 * \brief
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "DKUserInterfaceEditor.hpp"
#include "../Common/BuildingType.hpp"
#include "../Entities/Building.hpp"
#include "../Entities/Human.hpp"
#include "../Managers/DKCityMetrics.hpp"
#include "../Managers/DKImmigrationManager.hpp"
#include "../Managers/EventManager.hpp"
#include "../Managers/HappinessManager.hpp"
#include "Dokev/Core/DKMetaProgramming.hpp"
#include "Dokev/Managers/DKGameObjectManager.hpp"
#include <imgui.h>


void UserInterfaceEditor::DrawUI(HappinessManager* happinessManager, Dokev::ImmigrationManager* immigrationManager, Dokev::GameObjectManager* gameObjectManager, Dokev::CityMetrics* cityMetrics, int hour, int minute, int currentDay, EventManager* eventManager, SimulationControls* simControls)
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
    ImGui::SetNextWindowSize(ImVec2(300, 600), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("City Status"))
    {
        ImGui::End();
        return;
    }

    if (simControls)
    {
        DrawGameControlsSection(simControls);
        ImGui::Separator();
    }

    DrawTimeSection(hour, minute, currentDay);
    ImGui::Separator();

    if (ImGui::TreeNode("CITY HAPPINESS"))
    {
        DrawHappinessSection(happinessManager);
        ImGui::TreePop();
    }

    ImGui::Separator();

    if (ImGui::TreeNode("IMMIGRATION"))
    {
        DrawImmigrationSection(immigrationManager, happinessManager, availableHomes, totalResources, humanCount);
        ImGui::TreePop();
    }

    ImGui::Separator();

    if (ImGui::TreeNode("POPULATION"))
    {
        DrawPopulationSection(gameObjectManager, homeCapacity, totalHouses, workplaceCount, humanCount);
        ImGui::TreePop();
    }

    if (cityMetrics)
    {
        ImGui::Separator();
        if (ImGui::TreeNode("WIN CONDITION"))
        {
            DrawWinConditionSection(cityMetrics, humanCount);
            ImGui::TreePop();
        }
    }

    if (eventManager)
    {
        ImGui::Separator();
        if (ImGui::TreeNode("DISASTERS"))
        {
            DrawDisasterSection(eventManager);
            ImGui::TreePop();
        }
    }

    ImGui::End();

    DrawNPCInfoPanel();
}

void UserInterfaceEditor::DrawTimeSection(int hour, int minute, int currentDay)
{
    // Day 6-20, Night 22-4
    const char* phaseName;
    ImVec4      phaseColor;
    ImVec4      barColor;
    const char* phaseIcon;

    if (hour >= 4 && hour < 6)
    {
        phaseName  = "Dawn";
        phaseColor = { 1.0f, 0.6f, 0.2f, 1.0f };
        barColor   = { 1.0f, 0.55f, 0.1f, 1.0f };
        phaseIcon  = "~";
    }
    else if (hour >= 6 && hour < 12)
    {
        phaseName  = "Morning";
        phaseColor = { 1.0f, 0.95f, 0.3f, 1.0f };
        barColor   = { 1.0f, 0.88f, 0.15f, 1.0f };
        phaseIcon  = "*";
    }
    else if (hour >= 12 && hour < 18)
    {
        phaseName  = "Afternoon";
        phaseColor = { 1.0f, 0.85f, 0.1f, 1.0f };
        barColor   = { 1.0f, 0.82f, 0.0f, 1.0f };
        phaseIcon  = "*";
    }
    else if (hour >= 18 && hour < 20)
    {
        phaseName  = "Evening";
        phaseColor = { 1.0f, 0.5f, 0.1f, 1.0f };
        barColor   = { 1.0f, 0.45f, 0.05f, 1.0f };
        phaseIcon  = "~";
    }
    else if (hour >= 20 && hour < 22)
    {
        phaseName  = "Dusk";
        phaseColor = { 0.8f, 0.35f, 0.15f, 1.0f };
        barColor   = { 0.5f, 0.3f, 0.5f, 1.0f };
        phaseIcon  = "o";
    }
    else
    {
        phaseName  = "Night";
        phaseColor = { 0.55f, 0.55f, 0.95f, 1.0f };
        barColor   = { 0.25f, 0.25f, 0.65f, 1.0f };
        phaseIcon  = "o";
    }

    ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 1.0f), "Day %d", currentDay);
    ImGui::SameLine();
    ImGui::TextColored(phaseColor, "  [%s] %s", phaseIcon, phaseName);

    ImGui::Text("%02d:%02d", hour, minute);
    ImGui::SameLine();
    bool isWorking = (hour >= 9 && hour < 17);
    ImGui::TextColored(
        isWorking ? ImVec4(0.4f, 1.0f, 0.4f, 1.0f) : ImVec4(0.6f, 0.6f, 0.6f, 1.0f),
        " %s", isWorking ? "(Working)" : "(Off)");

    float progress = static_cast<float>(hour * 60 + minute) / 1440.0f;
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, barColor);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.12f, 0.12f, 0.28f, 0.85f));
    ImGui::ProgressBar(progress, ImVec2(-1.0f, 10.0f), "");
    ImGui::PopStyleColor(2);

    int totalMinutes = hour * 60 + minute;
    int remainMinutes;
    const char* nextPhase;
    if (totalMinutes < 6 * 60)          { remainMinutes = 6 * 60 - totalMinutes;  nextPhase = "Sunrise"; }
    else if (totalMinutes < 20 * 60)    { remainMinutes = 20 * 60 - totalMinutes; nextPhase = "Sunset"; }
    else                                { remainMinutes = (24 * 60 - totalMinutes) + 6 * 60; nextPhase = "Sunrise"; }

    ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "%s in %dh %02dm",
        nextPhase, remainMinutes / 60, remainMinutes % 60);
}

void UserInterfaceEditor::DrawHappinessSection(HappinessManager* happinessManager)
{
    float totalHappiness = happinessManager->CalculateTotalHappiness();
    auto  factors        = happinessManager->GetFactors();

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

    // Night Safety
    ImGui::Text("  Night Safety:");
    ImGui::SameLine();
    if (factors.nightSafetyScore >= 80.0f)
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "%.0f%%", factors.nightSafetyScore);
    else if (factors.nightSafetyScore >= 50.0f)
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "%.0f%%", factors.nightSafetyScore);
    else
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "%.0f%%", factors.nightSafetyScore);

}

void UserInterfaceEditor::DrawImmigrationSection(Dokev::ImmigrationManager* immigrationManager, HappinessManager* happinessManager, int availableHomes, int totalResources, int humanCount)
{
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

void UserInterfaceEditor::DrawPopulationSection(Dokev::GameObjectManager* gameObjectManager, int homeCapacity, int totalHouses, int workplaceCount, int humanCount)
{
    (void)gameObjectManager;

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

void UserInterfaceEditor::DrawWinConditionSection(Dokev::CityMetrics* cityMetrics, int humanCount)
{
    if (!cityMetrics)
        return;

    int  winPopulation = cityMetrics->GetWinConditionPopulation();
    bool isWinning     = cityMetrics->IsWinConditionMet(humanCount);

    //  Progress
    ImGui::Text("Goal: Reach %d citizens", winPopulation);
    ImGui::Text("Progress:");
    ImGui::SameLine();

    if (isWinning)
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "%d / %d (ACHIEVED!)", humanCount, winPopulation);
    else
    {
        float progress = static_cast<float>(humanCount) / static_cast<float>(winPopulation);
        if (progress >= 0.75f)
            ImGui::TextColored(ImVec4(0, 1, 1, 1), "%d / %d", humanCount, winPopulation);
        else if (progress >= 0.5f)
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d / %d", humanCount, winPopulation);
        else
            ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "%d / %d", humanCount, winPopulation);
    }

    //  Progress Bar
    float progressRatio = static_cast<float>(humanCount) / static_cast<float>(winPopulation);
    ImGui::ProgressBar(progressRatio, ImVec2(-1, 0));
}

void UserInterfaceEditor::DrawGameControlsSection(SimulationControls* controls)
{
    ImGui::Text("Simulation: %s", controls->paused ? "Paused" : "Running");
    if (controls->speedLabels && controls->speedIndex < controls->speedCount)
        ImGui::Text("Speed: %s", controls->speedLabels[controls->speedIndex]);

    if (ImGui::Button(controls->paused ? "Resume" : "Pause"))
        controls->pauseClicked = true;

    if (controls->speedLabels)
    {
        for (std::size_t i = 0; i < controls->speedCount; ++i)
        {
            ImGui::SameLine();
            if (ImGui::RadioButton(controls->speedLabels[i], controls->speedIndex == i))
                controls->speedClicked = static_cast<int>(i);
        }
    }

    ImGui::Spacing();
    if (ImGui::Button("Back to Main Menu"))
        controls->backClicked = true;
}

void UserInterfaceEditor::DrawDisasterSection(EventManager* eventManager)
{
    const auto& events = eventManager->GetActiveEvents();

    if (events.empty())
    {
        ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "No active disasters");

        double timeLeft = eventManager->GetMaxTime() - eventManager->GetDuration();
        ImGui::Text("Next event in: %.1f sec", timeLeft);
        return;
    }

    for (const auto& e : events)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "[!] %s", e->GetName());
        float progress = e->GetProgress();
        ImGui::ProgressBar(progress, ImVec2(-1, 0));
    }
}

void UserInterfaceEditor::DrawNPCInfoPanel()
{
    if (!selectedHuman)
        return;

    const float    panelW  = 260.0f;
    const float    panelH  = 160.0f;
    const float    padding = 12.0f;

    ImGuiViewport* vp      = ImGui::GetMainViewport();
    ImVec2         anchor  = ImVec2(vp->WorkPos.x + vp->WorkSize.x - padding,
                                    vp->WorkPos.y + vp->WorkSize.y - padding);

    ImGui::SetNextWindowPos(anchor, ImGuiCond_Always, ImVec2(1.0f, 1.0f));
    ImGui::SetNextWindowSize(ImVec2(panelW, panelH), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.85f);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove   |
                             ImGuiWindowFlags_NoCollapse;

    if (!ImGui::Begin("Citizen Info", nullptr, flags))
    {
        ImGui::End();
        return;
    }

    // Name
    ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.3f, 1.0f), "%s", selectedHuman->GetName().c_str());

    ImGui::Separator();

    // State
    const char* stateStr = "Unknown";
    switch (selectedHuman->GetState())
    {
        case HumanState::Idle:               stateStr = "Idle";              break;
        case HumanState::Walking:            stateStr = "Walking";           break;
        case HumanState::FindingDestination: stateStr = "Looking around";    break;
        case HumanState::Working:            stateStr = "Working";           break;
        case HumanState::Resting:            stateStr = "Resting";           break;
    }
    ImGui::Text("State:     %s", stateStr);

    // Happiness
    float happiness = selectedHuman->GetStats().happiness;
    ImGui::Text("Happiness: ");
    ImGui::SameLine();
    ImVec4 happyColor;
    if (happiness >= 70.0f)      happyColor = { 0.0f, 1.0f, 0.0f, 1.0f };
    else if (happiness >= 40.0f) happyColor = { 1.0f, 1.0f, 0.0f, 1.0f };
    else                         happyColor = { 1.0f, 0.2f, 0.2f, 1.0f };
    ImGui::TextColored(happyColor, "%.0f%%", happiness);
    ImGui::ProgressBar(happiness / 100.0f, ImVec2(-1, 6));

    // Position
    Math::fvec2 pos = selectedHuman->GetGridPosition();
    ImGui::Text("Pos:       (%.0f, %.0f)", pos.x, pos.y);

    ImGui::End();
}
