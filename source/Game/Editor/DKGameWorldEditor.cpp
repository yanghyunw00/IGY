/**
 * \file DKGameWorldEditor.cpp
 * \brief
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "DKGameWorldEditor.hpp"
#include "Dokev/Core/DKSystemInfo.hpp"
#include "Dokev/Graphics/DKCamera.hpp"
#include "Dokev/Managers/DKGameObjectManager.hpp"
#include "Dokev/Managers/DKMapManager.hpp"


#include "../Entities/Building.hpp"
#include "../Entities/Human.hpp"
#include "../Entities/ResidentBuilding.hpp"
#include "../Entities/Road.hpp"
#include "../Entities/WorkBuilding.hpp"
#include "../Managers/DKCityMetrics.hpp"
#include "../Managers/ResourceManager.hpp"
#include "../Managers/WorkManager.hpp"
#include "../World/GridMap.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Input.hpp"
#include "Engine/Window.hpp"
#include <SDL.h>
#include <imgui.h>


namespace Dokev
{
    void GameWorldEditor::Initialize(
        MapManager* mapManagerPtr, Camera* cameraPtr, int* indexPtr, float* immigrationTimerPtr, float* immigrationIntervalPtr, GameObjectManager* gameObjectManager,
        ResourceManager* resourceManagerPtr, CityMetrics* cityMetricsPtr)
    {
        mapManager          = mapManagerPtr;
        camera              = cameraPtr;
        rotationIndex       = indexPtr;
        immigrationTimer    = immigrationTimerPtr;
        immigrationInterval = immigrationIntervalPtr;
        goManager           = gameObjectManager;
        resourceManager     = resourceManagerPtr;
        cityMetrics         = cityMetricsPtr;
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
            if (input.KeyJustPressed(CS230::Input::Keys::Q))
                camera->ZoomOut();
            if (input.KeyJustPressed(CS230::Input::Keys::E))
                camera->ZoomIn();

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
        }

        // Rotation
        if (rotationIndex)
        {
            if (input.KeyJustPressed(CS230::Input::Keys::R))
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

            // Memory usage
            auto memInfo = Dokev::SystemInfo::GetMemoryInfo();
            ImGui::Text("Memory: %.1f / %.1f MB (%.1f%%)", memInfo.GetUsedPhysicalMB(), memInfo.GetTotalPhysicalMB(), memInfo.GetUsagePercent());
            ImGui::Text("Process: %.1f MB", memInfo.GetProcessUsageMB());

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

                if (ImGui::Button("Rotate Map to Left"))
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
            if (ImGui::TreeNode("PLAYER RESOURCES"))
            {
                ImGui::Separator();

                int wood = resourceManager->totalWood;
                if (ImGui::SliderInt("Wood", &wood, 0, 1000))
                {
                    resourceManager->totalWood = wood;
                }

                int stone = resourceManager->totalStone;
                if (ImGui::SliderInt("Stone", &stone, 0, 1000))
                {
                    resourceManager->totalStone = stone;
                }

                int steel = resourceManager->totalSteel;
                if (ImGui::SliderInt("Steel", &steel, 0, 1000))
                {
                    resourceManager->totalSteel = steel;
                }

                ImGui::Spacing();

                if (ImGui::Button("Set All to 100"))
                {
                    resourceManager->totalWood  = 100;
                    resourceManager->totalStone = 100;
                    resourceManager->totalSteel = 100;
                }

                ImGui::SameLine();

                if (ImGui::Button("Set All to 0"))
                {
                    resourceManager->totalWood  = 0;
                    resourceManager->totalStone = 0;
                    resourceManager->totalSteel = 0;
                }

                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::TreePop();
            }
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

                    vec2 pos = vec2(static_cast<float>(rand() % (GRID_SIZE_X - 1)), static_cast<float>(rand() % (GRID_SIZE_Y - 1)));

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

                    vec2 pos = vec2(static_cast<float>(rand() % (GRID_SIZE_X - 1)), static_cast<float>(rand() % (GRID_SIZE_Y - 1)));

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
                    auto* workshop = new WorkBuilding(pos, 1, 1, BuildingType::WOOD_WORKSHOP);
                    workshop->SetResourceManager(resourceManager);
                    goManager->AddBuilding(workshop);
                }

                if (ImGui::Button("Add Rock Workshop"))
                {
                    auto map = mapManager->GetMap();

                    vec2 pos = vec2(static_cast<float>(rand() % (GRID_SIZE_X - 1)), static_cast<float>(rand() % (GRID_SIZE_Y - 1)));

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
                    auto* workshop = new WorkBuilding(pos, 1, 1, BuildingType::ROCK_WORKSHOP);
                    workshop->SetResourceManager(resourceManager);
                    goManager->AddBuilding(workshop);
                }

                if (ImGui::Button("Add Steel Workshop"))
                {
                    auto map = mapManager->GetMap();

                    vec2 pos = vec2(static_cast<float>(rand() % (GRID_SIZE_X - 1)), static_cast<float>(rand() % (GRID_SIZE_Y - 1)));

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
                    auto* workshop = new WorkBuilding(pos, 1, 1, BuildingType::STEEL_WORKSHOP);
                    workshop->SetResourceManager(resourceManager);
                    goManager->AddBuilding(workshop);
                }
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Text("Building Status:");
                ImGui::Spacing();

                int bidx = 0;
                for (auto& building : buildings)
                {
                    ImGui::PushID(bidx);

                    BuildingType bt       = building->GetBuildingType();
                    const char*  btName   = bt == BuildingType::HOME           ? "Home"
                                         : bt == BuildingType::WOOD_WORKSHOP   ? "Wood Workshop"
                                         : bt == BuildingType::ROCK_WORKSHOP   ? "Rock Workshop"
                                         : bt == BuildingType::STEEL_WORKSHOP  ? "Steel Workshop"
                                                                               : "Unknown";
                    auto         gpos     = building->GetGrid();
                    bool         isBuilt  = building->GetIsBuilt();
                    unsigned int cur      = building->GetCurrentCapacity();
                    unsigned int maxCap   = building->GetMaxCapacity();

                    std::string blabel = std::string(btName) + " #" + std::to_string(bidx)
                                       + "  [" + std::to_string(cur) + "/" + std::to_string(maxCap) + "]"
                                       + (isBuilt ? "" : " (building...)");

                    if (ImGui::TreeNode(blabel.c_str()))
                    {
                        ImGui::Text("Position : (%.0f, %.0f)", gpos.x, gpos.y);
                        ImGui::Text("Built    : %s", isBuilt ? "Yes" : "No");
                        if (!isBuilt)
                            ImGui::ProgressBar(building->GetProgress(), ImVec2(-1, 0), "Construction");
                        ImGui::Text("Workers  : %u / %u", cur, maxCap);

                        if (!isBuilt)
                        {
                            if (ImGui::Button("Force Complete"))
                            {
                                ImGui::SameLine();
                                ImGui::TextDisabled("(Update loop will complete it)");
                            }
                        }

                        ImGui::TreePop();
                    }

                    ImGui::PopID();
                    bidx++;
                }

                ImGui::TreePop();
            }
        }
        ImGui::Separator();

        // WORK MANAGER DEBUG Section
        if (ImGui::TreeNode("WORK MANAGER"))
        {
            const auto& jobs = WorkManager::GetInstance().GetJobs();
            ImGui::Text("Active Jobs: %d", (int)jobs.size());

            ImGui::Spacing();

            if (ImGui::BeginTable("JobTable", 5,
                ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY,
                ImVec2(0, 160)))
            {
                ImGui::TableSetupScrollFreeze(0, 1);
                ImGui::TableSetupColumn("ID",       ImGuiTableColumnFlags_WidthFixed, 30);
                ImGui::TableSetupColumn("Type",     ImGuiTableColumnFlags_WidthFixed, 70);
                ImGui::TableSetupColumn("Pos",      ImGuiTableColumnFlags_WidthFixed, 80);
                ImGui::TableSetupColumn("Assigned", ImGuiTableColumnFlags_WidthFixed, 60);
                ImGui::TableSetupColumn("Worker",   ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();

                for (const auto& job : jobs)
                {
                    ImGui::TableNextRow();

                    ImGui::TableNextColumn();
                    ImGui::Text("%d", job.jobId);

                    ImGui::TableNextColumn();
                    const char* typeName = job.type == WorkType::Gathering ? "Gathering"
                                        : job.type == WorkType::Building   ? "Building"
                                        : job.type == WorkType::Farming    ? "Farming"
                                                                           : "None";
                    ImGui::Text("%s", typeName);

                    ImGui::TableNextColumn();
                    ImGui::Text("(%.0f,%.0f)", job.position.x, job.position.y);

                    ImGui::TableNextColumn();
                    if (job.isAssigned)
                        ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "YES");
                    else
                        ImGui::TextDisabled("no");

                    ImGui::TableNextColumn();
                    if (job.isAssigned && job.assignedWorker)
                    {
                        auto wp = job.assignedWorker->GetGridPosition();
                        ImGui::Text("(%.0f,%.0f) %s", wp.x, wp.y,
                            job.assignedWorker->GetState() == HumanState::Working ? "[work]" : "[walk]");
                    }
                    else
                    {
                        ImGui::TextDisabled("-");
                    }
                }
                ImGui::EndTable();
            }

            ImGui::Spacing();

            if (ImGui::Button("Clear All Jobs"))
            {
                auto& allJobs = WorkManager::GetInstance().GetJobs();
                std::vector<int> ids;
                for (const auto& j : allJobs) ids.push_back(j.jobId);
                for (int id : ids) WorkManager::GetInstance().RemoveJob(id);
            }

            ImGui::TreePop();
        }
        ImGui::Separator();

        // HUMAN CONTROLS Section
        if (ImGui::TreeNode("Human CONTROLS"))
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

                // Global work session time control
                {
                    static float globalWorkTime = 5.0f;
                    if (ImGui::SliderFloat("Work Session Time (sec)", &globalWorkTime, 1.0f, 60.0f))
                    {
                        for (auto& h : humans)
                            h->SetMaxWorkSessionTime(globalWorkTime);
                    }
                    ImGui::Spacing();
                }

                {
                    static float globalJobRadius = 20.0f;
                    if (ImGui::SliderFloat("Job Search Radius (tiles)", &globalJobRadius, 5.0f, 80.0f))
                    {
                        for (auto& h : humans)
                            h->SetJobSearchRadius(globalJobRadius);
                    }
                    ImGui::Spacing();
                }

                // Individual human controls
                if (ImGui::TreeNode("Individual Humans"))
                {
                    int index = 0;
                    for (auto& human : humans)
                    {
                        ImGui::PushID(index);

                        const char* stateStr = human->GetState() == HumanState::Idle                 ? "Idle"
                                             : human->GetState() == HumanState::Walking            ? "Walking"
                                             : human->GetState() == HumanState::FindingDestination ? "Finding"
                                             : human->GetState() == HumanState::Working            ? "Working"
                                                                                                   : "?";

                        std::string label = "Human #" + std::to_string(index) + "  [" + stateStr + "]";
                        if (ImGui::TreeNode(label.c_str()))
                        {
                            auto pos = human->GetGridPosition();
                            ImGui::Text("Grid Pos  : (%.1f, %.1f)", pos.x, pos.y);
                            ImGui::Text("State     : %s", stateStr);
                            ImGui::Text("Happiness : %.1f", human->GetHappiness());

                            ImGui::Spacing();
                            ImGui::Separator();
                            ImGui::Text("--- Work Info ---");

                            int jobId = human->GetAssignedJobId();
                            if (jobId != -1)
                            {
                                auto wp = human->GetWorkplacePosition();
                                ImGui::Text("Job ID    : %d", jobId);
                                ImGui::Text("Workplace : (%.1f, %.1f)", wp.x, wp.y);

                                if (human->GetState() == HumanState::Working)
                                {
                                    float t    = human->GetWorkSessionTimer();
                                    float tMax = human->GetMaxWorkSessionTime();
                                    char  buf[32];
                                    snprintf(buf, sizeof(buf), "%.1f / %.1fs", t, tMax);
                                    ImGui::ProgressBar(t / tMax, ImVec2(-1, 0), buf);
                                }

                                if (ImGui::Button("Cancel Work"))
                                    human->CancelWork();
                            }
                            else
                            {
                                ImGui::TextDisabled("No job assigned");
                            }

                            ImGui::Spacing();

                            if (ImGui::Button("Toggle Debug"))
                                human->ToggleDebug();

                            ImGui::SameLine();

                            if (ImGui::Button("Remove"))
                                human->Destroy();

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

        // WIN CONDITION CONTROLS
        if (cityMetrics)
        {
            ImGui::Spacing();
            ImGui::Text("WIN CONDITION SETTINGS");
            ImGui::Separator();

            int winPopulation = cityMetrics->GetWinConditionPopulation();
            if (ImGui::SliderInt("Win Population", &winPopulation, 5, 100))
            {
                cityMetrics->SetWinConditionPopulation(winPopulation);
            }

            if (goManager)
            {
                int  currentPopulation = static_cast<int>(goManager->GetHumans().size());
                bool isWinning         = cityMetrics->IsWinConditionMet(currentPopulation);

                ImGui::Text("Current: %d / %d", currentPopulation, winPopulation);

                if (isWinning)
                {
                    ImGui::TextColored(ImVec4(0, 1, 0, 1), "WINNER!");
                }
                else
                {
                    float progress = static_cast<float>(currentPopulation) / static_cast<float>(winPopulation);
                    ImGui::ProgressBar(progress, ImVec2(-1, 0));
                }
            }

            ImGui::Spacing();
            ImGui::Separator();
        }

        ImGui::End();
    }
}
