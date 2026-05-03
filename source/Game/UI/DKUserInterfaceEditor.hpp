/**
 * \file UserInterfaceEditor.hpp
 * \brief
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once

// Forward declarations
class HappinessManager;
class Human;

namespace Dokev
{
    class GameObjectManager;
    class ImmigrationManager;
}

namespace Dokev
{
    class CityMetrics;
}

class UserInterfaceEditor
{
public:
    UserInterfaceEditor() = default;
    void DrawUI(HappinessManager* happinessManager, Dokev::ImmigrationManager* immigrationManager, Dokev::GameObjectManager* gameObjectManager, Dokev::CityMetrics* cityMetrics = nullptr, int hour = 0, int minute = 0);

    void SetSelectedHuman(Human* human) { selectedHuman = human; }

private:
    void DrawHappinessSection(HappinessManager* happinessManager);
    void DrawImmigrationSection(Dokev::ImmigrationManager* immigrationManager, HappinessManager* happinessManager, int availableHomes, int totalResources, int humanCount);
    void DrawPopulationSection(Dokev::GameObjectManager* gameObjectManager, int homeCapacity, int totalHouses, int workplaceCount, int humanCount);
    void DrawWinConditionSection(Dokev::CityMetrics* cityMetrics, int humanCount);

    /**
     * \brief Draw selected NPC info panel at bottom-right corner
     */
    void DrawNPCInfoPanel();

    Human* selectedHuman = nullptr;
};
