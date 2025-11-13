/**
 * \file DKGameWorldEditor.cpp
 * \brief
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "DKGameWorldEditor.hpp"
#include "DKCamera.hpp"
#include "DKGameObjectManager.hpp"
#include "DKMapManager.hpp"

#include "../Game/Building.hpp"
#include "../Game/GridMap.hpp"
#include "../Game/Human.hpp"
#include "../Game/ResidentBuilding.hpp"
#include "../Game/Road.hpp"
#include "../Game/WorkBuilding.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Input.hpp"
#include "Engine/Window.hpp"
#include "Game/ResourceManager.hpp"
#include <SDL.h>
#include <imgui.h>

namespace Dokev
{
    void GameWorldEditor::Initialize(
        MapManager* mapManagerPtr, Camera* cameraPtr, int* indexPtr, float* immigrationTimerPtr, float* immigrationIntervalPtr, GameObjectManager* gameObjectManager,
        ResourceManager* resourceManagerPtr)
    {
        mapManager          = mapManagerPtr;
        camera              = cameraPtr;
        rotationIndex       = indexPtr;
        immigrationTimer    = immigrationTimerPtr;
        immigrationInterval = immigrationIntervalPtr;
        goManager           = gameObjectManager;
        resourceManager     = resourceManagerPtr;
    }

    void GameWorldEditor::UpdateInput()
    {
        const auto& input       = Engine::GetInput();
        const auto& environment = Engine::GetWindowEnvironment();

        // Regenerate map
        if (input.KeyJustPressed(CS230::Input::Keys::Space))
        {
            if (mapManager)
            {
                auto map = mapManager->GetMap();
                map->ReGenerateMap();
                map->InitializePathFinding();
                map->ClearPath();
                printf("Map regenerated and pathfinding reinitialized\n");
            }
        }

        // Camera zoom
        if (camera)
        {
            if (input.KeyDown(CS230::Input::Keys::Q))
                camera->AdjustZoom(0.95f);
            if (input.KeyDown(CS230::Input::Keys::E))
                camera->AdjustZoom(1.05f);

            camera->ClampZoom(0.1f, 5.0f);

            constexpr double camera_speed = 1.0;

            // Camera movement
            if (input.KeyDown(CS230::Input::Keys::A))
            {
                camera->Move({ camera_speed * environment.DeltaTime, 0 });
            }
            if (input.KeyDown(CS230::Input::Keys::D))
            {
                camera->Move({ -camera_speed * environment.DeltaTime, 0 });
            }
            if (input.KeyDown(CS230::Input::Keys::W))
            {
                camera->Move({ 0, -camera_speed * environment.DeltaTime });
            }
            if (input.KeyDown(CS230::Input::Keys::S))
            {
                camera->Move({ 0, camera_speed * environment.DeltaTime });
            }

            // Reset camera
            if (input.KeyDown(CS230::Input::Keys::R))
            {
                camera->Reset();
            }
        }

        // Map rotation
        if (rotationIndex)
        {
            if (input.KeyJustPressed(CS230::Input::Keys::Z))
            {
                *rotationIndex = (*rotationIndex - 1 + 4) % 4;
            }
            else if (input.KeyJustPressed(CS230::Input::Keys::C))
            {
                *rotationIndex = (*rotationIndex + 1) % 4;
            }
        }
    }

    void GameWorldEditor::DrawImGui()
    {
        if (!ImGui::Begin("Controller"))
        {
            ImGui::End();
            return;
        }

        // PERFORMANCE Section
        if (ImGui::TreeNode("PERFORMANCE"))
        {
            ImGui::Separator();

            const auto& env = Engine::GetWindowEnvironment();
            ImGui::Text("FPS: %d", env.FPS);
            ImGui::Text("Frame time: %.2f ms", static_cast<float>(env.DeltaTime * 1000.0));

            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::Text("DISPLAY SETTINGS");
            ImGui::Separator();

            if (ImGui::Checkbox("VSync Enabled", &vsyncEnabled))
            {
                if (vsyncEnabled)
                {
                    if (SDL_GL_SetSwapInterval(-1) != 0)
                        SDL_GL_SetSwapInterval(1);
                }
                else
                {
                    SDL_GL_SetSwapInterval(0);
                }
            }
            ImGui::TreePop();
        }

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Separator();

        // CAMERA CONTROLS Section
        if (ImGui::TreeNode("CAMERA CONTROLS"))
        {
            if (camera)
            {
                float zoom = camera->GetZoom();
                if (ImGui::SliderFloat("Zoom", &zoom, 0.1f, 1.0f))
                {
                    camera->SetZoom(zoom);
                }

                ImGui::Spacing();
            }

            if (rotationIndex)
            {
                if (ImGui::Button("Rotate to Map right"))
                {
                    *rotationIndex = (*rotationIndex - 1 + 4) % 4;
                }

                if (ImGui::Button("Roatate Map to Left"))
                {
                    *rotationIndex = (*rotationIndex + 1) % 4;
                }

                ImGui::Text("Rotate Index: %d", *rotationIndex);

                ImGui::Spacing();
                ImGui::Spacing();
            }
            ImGui::TreePop();
        }
        ImGui::Separator();

        if (resourceManager)
        {
            ImGui::Text("PLAYER RESOURCES");
            ImGui::Separator();
            ImGui::Text("Wood: %d", resourceManager->totalWood);
            ImGui::Text("Stone: %d", resourceManager->totalStone);
            ImGui::Text("Steel: %d", resourceManager->totalSteel);
            ImGui::Spacing();
            ImGui::Spacing();
        }
        ImGui::Separator();

        ImGui::Spacing();

        // ROAD CONTROLS Section
        if (ImGui::TreeNode("Road Building"))
        {
            if (mapManager)
            {
                auto road = mapManager->GetRoad();
                auto map  = mapManager->GetMap();

                ImGui::Text("Total roads: %zu", road->getRoadCount());

                static int roadStartX = 10;
                static int roadStartY = 10;
                static int roadEndX   = 30;
                static int roadEndY   = 30;

                ImGui::SliderInt("Road Start X", &roadStartX, 0, GRID_SIZE_X - 1);
                ImGui::SliderInt("Road Start Y", &roadStartY, 0, GRID_SIZE_Y - 1);
                ImGui::SliderInt("Road End X", &roadEndX, 0, GRID_SIZE_X - 1);
                ImGui::SliderInt("Road End Y", &roadEndY, 0, GRID_SIZE_Y - 1);

                if (ImGui::Button("Build Road Path"))
                {
                    road->buildRoadPath(*map, roadStartX, roadStartY, roadEndX, roadEndY);
                }

                ImGui::SameLine();

                if (ImGui::Button("Clear All Roads"))
                {
                    road->removeAllRoad(*map);
                    printf("All roads cleared\n");
                }

                if (ImGui::Button("Add Single Road"))
                {
                    road->addRoad(roadStartX, roadStartY, *map);
                    printf("Added road at (%d,%d)\n", roadStartX, roadStartY);
                }

                ImGui::SameLine();

                if (ImGui::Button("Remove Road"))
                {
                    road->removeRoad(roadStartX, roadStartY, *map);
                    printf("Removed road at (%d,%d)\n", roadStartX, roadStartY);
                }
            }
            ImGui::TreePop();
        }
        // BUILDING CONTROLS=============================================================
        ImGui::Separator();
       
        if (ImGui::TreeNode("BUILDING CONTROLS"))
        {
            if (goManager && mapManager)
            {
                auto& buildings = goManager->GetBuildings();
                ImGui::Text("Total Buildings : %zu", buildings.size());

                ImGui::Spacing();

                ImGui::Text("Resident building: ");
                if (ImGui::Button("Add Small House"))
                {
                    auto map = mapManager->GetMap();

                    vec2 pos = vec2(rand() % (GRID_SIZE_X - 1), rand() % (GRID_SIZE_Y - 1));

                    const int maxAttemps = 100;
                    bool      isfound    = false;

                    for (int attempt = 0; attempt < maxAttemps && !isfound; ++attempt)
                    {
                        int gridX = rand() % GRID_SIZE_X;
                        int gridY = rand() % GRID_SIZE_Y;

                        GridCell** viewArray = map->GetViewArray(1);
                        GridCell*  cell      = viewArray[gridY * GRID_SIZE_X + gridX];
                        double     height    = cell->getHeight();

                        if (height >= 75.0f)
                        {
                            pos     = vec2(static_cast<float>(gridX), static_cast<float>(gridY));
                            isfound = true;
                        }
                    }
                    goManager->AddBuilding(new ResidentBuilding(pos, 1, 1, BuildingType::HOME));
                }

                ImGui::Spacing();

                ImGui::Text("Work Buildings: ");
                if (ImGui::Button("Add Wood Workshop"))
                {
                    auto map = mapManager->GetMap();

                    vec2 pos = vec2(rand() % (GRID_SIZE_X - 1), rand() % (GRID_SIZE_Y - 1));

                    const int maxAttemps = 100;
                    bool      isfound    = false;

                    for (int attempt = 0; attempt < maxAttemps && !isfound; ++attempt)
                    {
                        int gridX = rand() % GRID_SIZE_X;
                        int gridY = rand() % GRID_SIZE_Y;

                        GridCell** viewArray = map->GetViewArray(1);
                        GridCell*  cell      = viewArray[gridY * GRID_SIZE_X + gridX];
                        double     height    = cell->getHeight();

                        if (height >= 75.0f)
                        {
                            pos     = vec2(static_cast<float>(gridX), static_cast<float>(gridY));
                            isfound = true;
                        }
                    }
                    goManager->AddBuilding(new WorkBuilding(pos, 1, 1, BuildingType::WOOD_WORKSHOP));
                }

                if (ImGui::Button("Add Rock Workshop"))
                {
                    auto map = mapManager->GetMap();

                    vec2 pos = vec2(rand() % (GRID_SIZE_X - 1), rand() % (GRID_SIZE_Y - 1));

                    const int maxAttemps = 100;
                    bool      isfound    = false;

                    for (int attempt = 0; attempt < maxAttemps && !isfound; ++attempt)
                    {
                        int gridX = rand() % GRID_SIZE_X;
                        int gridY = rand() % GRID_SIZE_Y;

                        GridCell** viewArray = map->GetViewArray(1);
                        GridCell*  cell      = viewArray[gridY * GRID_SIZE_X + gridX];
                        double     height    = cell->getHeight();

                        if (height >= 75.0f)
                        {
                            pos     = vec2(static_cast<float>(gridX), static_cast<float>(gridY));
                            isfound = true;
                        }
                    }
                    goManager->AddBuilding(new WorkBuilding(pos, 1, 1, BuildingType::ROCK_WORKSHOP));
                }

                if (ImGui::Button("Add Steel Workshop"))
                {
                    auto map = mapManager->GetMap();

                    vec2 pos = vec2(rand() % (GRID_SIZE_X - 1), rand() % (GRID_SIZE_Y - 1));

                    const int maxAttemps = 100;
                    bool      isfound    = false;

                    for (int attempt = 0; attempt < maxAttemps && !isfound; ++attempt)
                    {
                        int gridX = rand() % GRID_SIZE_X;
                        int gridY = rand() % GRID_SIZE_Y;

                        GridCell** viewArray = map->GetViewArray(1);
                        GridCell*  cell      = viewArray[gridY * GRID_SIZE_X + gridX];
                        double     height    = cell->getHeight();

                        if (height >= 75.0f)
                        {
                            pos     = vec2(static_cast<float>(gridX), static_cast<float>(gridY));
                            isfound = true;
                        }
                    }
                    goManager->AddBuilding(new WorkBuilding(pos, 1, 1, BuildingType::STEEL_WORKSHOP));
                }
                ImGui::TreePop();
            }
        }
        // =======================================================================================================
        ImGui::Separator();

        // HUMAN CONTROLS Section
        if (ImGui::TreeNode("Human CONTORLS"))
        {
            if (goManager)
            {
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::Separator();

                auto& humans = goManager->GetHumans();
                ImGui::Text("Total Humans: %zu", humans.size());

                // IMMIGRATION CONTROLS Section
                if (immigrationInterval && immigrationTimer)
                {
                    ImGui::Text("IMMIGRATION CONTROLS");
                    ImGui::Separator();

                    ImGui::SliderFloat("Immigration Interval (sec)", immigrationInterval, 1.0f, 30.0f);
                    ImGui::Text("Next immigrant in: %.1f sec", *immigrationInterval - *immigrationTimer);

                    ImGui::Spacing();
                    ImGui::Spacing();
                }

                ImGui::Spacing();

                if (ImGui::Button("Add Human at Center"))
                {
                    goManager->AddHuman(new Human(Math::fvec2(GRID_SIZE_X / 2.0f, GRID_SIZE_Y / 2.0f)));
                }

                ImGui::SameLine();

                if (ImGui::Button("Add Immigrant"))
                {
                    goManager->AddHuman(new Human(Human::CreateImmigrant()));
                }

                ImGui::Spacing();

                // Individual human controls
                if (ImGui::TreeNode("Individual Humans"))
                {
                    int index = 0;
                    for (auto& human : humans)
                    {
                        ImGui::PushID(index);

                        std::string label = "Human #" + std::to_string(index);
                        if (ImGui::TreeNode(label.c_str()))
                        {
                            auto pos = human->GetGridPosition();
                            ImGui::Text("Position: (%.1f, %.1f)", pos.x, pos.y);
                            ImGui::Text(
                                "State: %s", human->GetState() == HumanState::Idle                 ? "Idle"
                                             : human->GetState() == HumanState::Walking            ? "Walking"
                                             : human->GetState() == HumanState::FindingDestination ? "Finding Destination"
                                             : human->GetState() == HumanState::Working            ? "Working"
                                                                                                   : "Resting");
                            ImGui::Text("Happiness: %.1f", human->GetHappiness());

                            if (ImGui::Button("Toggle Debug"))
                            {
                                human->ToggleDebug();
                            }

                            ImGui::SameLine();

                            if (ImGui::Button("Remove"))
                            {
                                human->Destroy();
                            }

                            ImGui::TreePop();
                        }

                        ImGui::PopID();
                        index++;
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }
        ImGui::Separator();


        ImGui::Text("SAVE / LOAD");
        ImGui::Separator();
        if (ImGui::Button("Save Game"))
        {
            saveGameRequested = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Load Game"))
        {
            loadGameRequested = true;
        }
        ImGui::Separator();

        ImGui::End();
    }
}
