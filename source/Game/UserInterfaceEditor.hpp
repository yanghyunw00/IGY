/**
 * \file UserInterfaceEditor.hpp
 * \brief 
 * \author 
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once

// Forward declarations
class HappinessSystem;
class ImmigrationSystem;

namespace Dokev
{
    class GameObjectManager;
}

class UserInterfaceEditor
{
public:
    UserInterfaceEditor() = default;
    void DrawUI(HappinessSystem* happinessSystem, ImmigrationSystem* immigrationSystem,
                Dokev::GameObjectManager* gameObjectManager);

private:
    /**
     * \brief Draw happiness section (without Begin/End)
     */
    void DrawHappinessSection(HappinessSystem* happinessSystem);

    /**
     * \brief Draw immigration section (without Begin/End)
     */
    void DrawImmigrationSection(ImmigrationSystem* immigrationSystem, HappinessSystem* happinessSystem,
                                int availableHomes, int totalResources, int humanCount);

    /**
     * \brief Draw population section (without Begin/End)
     */
    void DrawPopulationSection(Dokev::GameObjectManager* gameObjectManager, int homeCapacity, int totalHouses,
                               int workplaceCount, int humanCount);
};
