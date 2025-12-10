/**
 * \file UserInterfaceEditor.hpp
 * \brief
 * \author
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once

// Forward declarations
class HappinessManager;

namespace Dokev
{
    class GameObjectManager;
    class ImmigrationManager;
}

class UserInterfaceEditor
{
public:
    UserInterfaceEditor() = default;
    void DrawUI(HappinessManager* happinessManager, Dokev::ImmigrationManager* immigrationManager, Dokev::GameObjectManager* gameObjectManager);

private:
    /**
     * \brief Draw happiness section (without Begin/End)
     */
    void DrawHappinessSection(HappinessManager* happinessManager);

    /**
     * \brief Draw immigration section (without Begin/End)
     */
    void DrawImmigrationSection(Dokev::ImmigrationManager* immigrationManager, HappinessManager* happinessManager, int availableHomes, int totalResources, int humanCount);

    /**
     * \brief Draw population section (without Begin/End)
     */
    void DrawPopulationSection(Dokev::GameObjectManager* gameObjectManager, int homeCapacity, int totalHouses, int workplaceCount, int humanCount);
};
