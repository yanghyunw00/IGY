/**
 * \file UserInterfaceEditor.hpp
 * \brief
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once

#include <cstddef>

// Forward declarations
class HappinessManager;
class Human;
class EventManager;

struct SimulationControls
{
    bool        paused        = false;
    std::size_t speedIndex    = 1;
    bool        pauseClicked  = false;
    bool        backClicked   = false;
    int         speedClicked  = -1;

    const char* const* speedLabels = nullptr;
    std::size_t        speedCount  = 0;
};

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
    void DrawUI(HappinessManager* happinessManager, Dokev::ImmigrationManager* immigrationManager, Dokev::GameObjectManager* gameObjectManager, Dokev::CityMetrics* cityMetrics = nullptr, int hour = 0, int minute = 0, EventManager* eventManager = nullptr, SimulationControls* simControls = nullptr);

    void SetSelectedHuman(Human* human) { selectedHuman = human; }

private:
    void DrawHappinessSection(HappinessManager* happinessManager);
    void DrawImmigrationSection(Dokev::ImmigrationManager* immigrationManager, HappinessManager* happinessManager, int availableHomes, int totalResources, int humanCount);
    void DrawPopulationSection(Dokev::GameObjectManager* gameObjectManager, int homeCapacity, int totalHouses, int workplaceCount, int humanCount);
    void DrawWinConditionSection(Dokev::CityMetrics* cityMetrics, int humanCount);
    void DrawDisasterSection(EventManager* eventManager);
    void DrawGameControlsSection(SimulationControls* controls);

    /**
     * \brief Draw selected NPC info panel at bottom-right corner
     */
    void DrawNPCInfoPanel();

    Human* selectedHuman = nullptr;
};
