/**
 * \file DKGameWorldEditor.cpp
 * \brief
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "DKGameWorldEditor.hpp"
#include "DKCamera.hpp"
#include "../Game/GridMap.hpp"
#include "../Game/Human.hpp"
#include "../Game/Road.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Window.hpp"
#include "Engine/Input.hpp"
#include <imgui.h>
#include <SDL.h>

namespace Dokev
{
    void GameWorldEditor::Initialize(GridMap* mapPtr, Camera* cameraPtr, std::vector<Human>* humanPtr, Road* roadPtr,
                                     int* indexPtr, float* immigrationTimerPtr, float* immigrationIntervalPtr)
    {
        map = mapPtr;
        camera = cameraPtr;
        humans = humanPtr;
        road = roadPtr;
        rotationIndex = indexPtr;
        immigrationTimer = immigrationTimerPtr;
        immigrationInterval = immigrationIntervalPtr;
    }

    void GameWorldEditor::UpdateInput()
    {
        const auto& input = Engine::GetInput();
        const auto& environment = Engine::GetWindowEnvironment();

        // Regenerate map
        if (input.KeyJustPressed(CS230::Input::Keys::Space))
        {
            if (map)
            {
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
        ImGui::Text("PERFORMANCE");
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

        ImGui::Spacing();
        ImGui::Spacing();

        // CAMERA CONTROLS Section
        if (camera)
        {
            ImGui::Text("CAMERA CONTROLS");
            ImGui::Separator();

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

        // HUMAN CONTROLS Section
        if (humans && map)
        {
            ImGui::Text("HUMAN CONTROLS");
            ImGui::Separator();

            ImGui::Text("Total Humans: %zu", humans->size());

            if (ImGui::Button("Spawn Immigrant"))
            {
                humans->push_back(Human::CreateImmigrant());
            }

            ImGui::SameLine();

            if (ImGui::Button("Make All Humans Explore"))
            {
                for (auto& h : *humans)
                {
                    h.DecideNextAction(map);
                }
            }

            if (immigrationInterval && immigrationTimer)
            {
                ImGui::SliderFloat("Immigration Interval (sec)", immigrationInterval, 1.0f, 30.0f);
                ImGui::Text("Next immigrant in: %.1f sec", *immigrationInterval - *immigrationTimer);
            }

            ImGui::Separator();
            ImGui::Text("Custom Path:");

            static int startX = 5;
            static int startY = 5;
            static int endX = 50;
            static int endY = 50;

            ImGui::SliderInt("Start X", &startX, 0, GRID_SIZE_X - 1);
            ImGui::SliderInt("Start Y", &startY, 0, GRID_SIZE_Y - 1);
            ImGui::SliderInt("End X", &endX, 0, GRID_SIZE_X - 1);
            ImGui::SliderInt("End Y", &endY, 0, GRID_SIZE_Y - 1);
        }

        // ROAD CONTROLS Section
        if (road && map)
        {
            ImGui::Separator();
            ImGui::Text("Road Building:");

            ImGui::Text("Total roads: %zu", road->getRoadCount());

            static int roadStartX = 10;
            static int roadStartY = 10;
            static int roadEndX = 30;
            static int roadEndY = 30;

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
                road->addRoad(roadStartX, roadStartY);
                printf("Added road at (%d,%d)\n", roadStartX, roadStartY);
            }

            ImGui::SameLine();

            if (ImGui::Button("Remove Road"))
            {
                road->removeRoad(roadStartX, roadStartY);
                printf("Removed road at (%d,%d)\n", roadStartX, roadStartY);
            }
        }

        ImGui::End();
    }
}
