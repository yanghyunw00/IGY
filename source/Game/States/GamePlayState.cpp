/**
 * \file GamePlayState.cpp
 * \author Hyunwoo Yang
 * \author
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "GamePlayState.hpp"
#include "../Entities/Building.hpp"
#include "../Entities/Human.hpp"
#include "../Entities/ResidentBuilding.hpp"
#include "../Entities/WorkBuilding.hpp"
#include "../Managers/DKCityMetrics.hpp"
#include "../World/GridMap.hpp"
#include "CS200/IRenderer2D.hpp"
#include "CS200/NDC.hpp"
#include "CS200/RGBA.hpp"
#include "CS200/RenderingAPI.hpp"
#include "Dokev/Core/DKColor.hpp"
#include "Dokev/Core/DKMetaProgramming.hpp"
#include "Dokev/Graphics/DKImage.hpp"
#include "Dokev/Graphics/DKParticles.hpp"
#include "Dokev/Graphics/DKShapes.hpp"
#include "Dokev/Graphics/DKTexture.hpp"
#include "Dokev/Managers/DKGameObjectManager.hpp"


#include "Engine/AudioManager.hpp"
#include "Engine/Engine.hpp"
#include "Engine/FontManager.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Input.hpp"
#include "Engine/Logger.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Path.hpp"
#include "Engine/Window.hpp"


#include "../Entities/Road.hpp"
#include "../Managers/AchievementManager.hpp"
#include "../Managers/FloatingTextManager.hpp"
#include "../Managers/ResourceManager.hpp"
#include "../Resources/Resource.hpp"
#include "../Resources/Steel.hpp"
#include "../Resources/Stone.hpp"
#include "../Resources/Wood.hpp"
#include "MainMenu.hpp"


#include "OpenGL/GL.hpp"

#include <GL/glew.h>
#include <SDL.h>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <imgui.h>
#include <iomanip>
#include <nlohmann/json.hpp>
#include <numbers>
#include <sstream>


using json = nlohmann::json;

namespace
{
    constexpr std::array<double, 4>      kSimulationSpeedPresets = { 0.5, 1.0, 2.0, 4.0 };
    constexpr std::array<const char*, 4> kSimulationSpeedLabels  = { "0.5x", "1.0x", "2.0x", "4.0x" };

    Math::fvec2 ConvertViewTileToGrid(Math::fvec2 tileCoord, int rotationIndex, const GridMap* map)
    {
        if (!map)
        {
            return tileCoord;
        }

        int rotation = ((rotationIndex % 4) + 4) % 4;
        if (rotation == 0)
        {
            return tileCoord;
        }

        int inverseRotation = (4 - rotation) % 4;
        return map->TransformPathPointForView(tileCoord, inverseRotation);
    }

    Math::ivec2 lastPlacedRoadPos              = { -1, -1 };
    bool        blockRoadPlacementUntilRelease = false; // avoid placing road with the same click used to select it
}

NLOHMANN_JSON_SERIALIZE_ENUM(
    ResourceType, {
                      {  ResourceType::Wood,  "Wood" },
                      { ResourceType::Stone, "Stone" },
                      { ResourceType::Steel, "Steel" }
})

void GamePlayState::Load()
{
    CS200::RenderingAPI::SetClearColor(0x252525FF);

    auto perlin   = Dokev::GenImagePerlinNoise(256, 256, 0, 0, 0.01f);
    auto perlin2  = Dokev::GenImagePerlinNoise(256, 256, 256, 256, 0.01f);
    noiseTexture  = Dokev::LoadTextureFromImage(perlin);
    noiseTexture2 = Dokev::LoadTextureFromImage(perlin2);

    CS230::FontManager::Instance().Load("MainFont", assets::locate_asset("Assets/fonts/Font_Simple.png"));
    CS230::AudioManager::Instance().PlayMusic("Assets/sounds/BGM.mp3", true);
    roadBuildSoundCount = 0;

    // Initialize MapManager component
    AddGSComponent<MapManager>();
    GetGSComponent<MapManager>()->Initialize();

    // Initialize ResourceManager
    AddGSComponent<ResourceManager>();

    // Initialize CityMetrics
    AddGSComponent<CityMetrics>();

    // Initialize GameObjectManager component
    AddGSComponent<Dokev::GameObjectManager>();
    {
        const Road* road     = GetGSComponent<MapManager>()->GetRoad();
        Math::fvec2 spawnPos = { GRID_SIZE_X / 2.0f, GRID_SIZE_Y / 2.0f };
        if (road && !road->GetRoadPositions().empty())
        {
            const auto& positions = road->GetRoadPositions();
            spawnPos              = positions[positions.size() / 2]; // middle of the road
        }
        GetGSComponent<Dokev::GameObjectManager>()->AddHuman(new Human(spawnPos));
        GetGSComponent<Dokev::GameObjectManager>()->AddHuman(new Human(spawnPos));
    }


    camera.SetZoom(0.3f);

    // smokeSprite = Dokev::AnimatedSprite(assets::locate_asset("Assets/images/Smoke.spt"));

    worldEditor.Initialize(
        GetGSComponent<MapManager>().get(), &camera, &index, &immigrationTimer, &immigrationInterval, GetGSComponent<Dokev::GameObjectManager>().get(), GetGSComponent<ResourceManager>().get(),
        GetGSComponent<CityMetrics>().get());

    // Initialize UI Manager
    uiManager.Initialize();

    // Initialize Tutorial Manager
    tutorialManager.Initialize(&uiManager);

    // Load building costs from JSON
    buildingCostManager.LoadFromFile("Assets/resources/building_costs.json");

    mouseSelection.SetGridMap(GetGSComponent<MapManager>()->GetMap());
    mouseSelection.Load(assets::locate_asset("Assets/images/textures/select.png"), assets::locate_asset("Assets/images/textures/selectRed.png"));

    GL::Enable(GL_BLEND);
    GL::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    achievementManager.Init();
}

void GamePlayState::UpdateTimeControls()
{
    const auto& input = Engine::GetInput();

    if (input.KeyJustPressed(CS230::Input::Keys::P))
    {
        simulationPaused = !simulationPaused;
    }

    if (input.KeyJustPressed(CS230::Input::Keys::Left) && simulationSpeedPresetIndex > 0)
    {
        --simulationSpeedPresetIndex;
    }

    if (input.KeyJustPressed(CS230::Input::Keys::Right) && simulationSpeedPresetIndex + 1 < kSimulationSpeedPresets.size())
    {
        ++simulationSpeedPresetIndex;
    }
}

double GamePlayState::GetSimulationDeltaTime(double dt) const
{
    if (simulationPaused)
    {
        return 0.0;
    }

    return dt * kSimulationSpeedPresets[simulationSpeedPresetIndex];
}

void GamePlayState::SetSimulationSpeedPreset(std::size_t presetIndex)
{
    simulationSpeedPresetIndex = std::min(presetIndex, kSimulationSpeedPresets.size() - 1);
}

void GamePlayState::Update(double dt)
{
    // Engine::GetLogger().LogEvent("GamePlayState::Update start");
    [[maybe_unused]] const auto& environment = Engine::GetWindowEnvironment();
    UpdateTimeControls();

    const double simulationDt = GetSimulationDeltaTime(dt);

    if (worldEditor.IsSaveRequested())
    {
        SaveGame("savegame.json");
        worldEditor.ClearSaveRequest();
    }
    if (worldEditor.IsLoadRequested())
    {
        LoadGame("savegame.json");
        worldEditor.ClearLoadRequest();
    }

    mouse.Update();
    bool ismouseheld = mouse.MouseDown(Dokev::Mouse::Button::Left);
    if (!ismouseheld)
    {
        blockRoadPlacementUntilRelease = false;
        lastPlacedRoadPos              = { -1, -1 };
    }

    if (simulationPaused)
    {
        mouseSelection.SetHasValidPosition(false);
        mouseSelection.SetViewIndex(index);
        Engine::GetLogger().LogEvent("GamePlayState::Update paused");
        return;
    }

    camera.UpdateZoomInterpolation(simulationDt);

    // Only process game input if UI menu is not active
    if (!uiManager.IsMenuActive())
    {
        worldEditor.UpdateInput(simulationDt);
        // Engine::GetLogger().LogEvent("GamePlayState::Update after worldEditor");
    }


    UpdateGSComponents(simulationDt);
    // Engine::GetLogger().LogEvent("GamePlayState::Update after UpdateGSComponents");

    auto cityMetrics = GetGSComponent<CityMetrics>();
    if (cityMetrics)
    {
        cityMetrics->Update(simulationDt);
    }
    // Engine::GetLogger().LogEvent("GamePlayState::Update after cityMetrics Update");

    // Update water tile animation
    auto mapMgr = GetGSComponent<MapManager>();

    if (mapMgr && mapMgr->GetMap())
    {
        mapMgr->GetMap()->UpdateWaterAnimation(simulationDt);
    }

    // Update all humans
    auto& humans     = GetGSComponent<Dokev::GameObjectManager>()->GetHumans();
    auto  gameObjMgr = GetGSComponent<Dokev::GameObjectManager>();

    // Engine::GetLogger().LogEvent("GamePlayState::Update humans count " + std::to_string(humans.size()));

    if (!humans.empty())
    {
        for (size_t i = 0; i < humans.size(); ++i)
        {
            // Engine::GetLogger().LogEvent("GamePlayState::Update human " + std::to_string(i) + " begin");
            humans[i]->Update(static_cast<float>(simulationDt), mapMgr->GetMap(), mapMgr->GetRoad());
            // Engine::GetLogger().LogEvent("GamePlayState::Update human " + std::to_string(i) + " end");
        }
    }
    // Engine::GetLogger().LogEvent("GamePlayState::Update after humans");

    GetGSComponent<Dokev::GameObjectManager>()->Refresh();

    auto& buildings = GetGSComponent<Dokev::GameObjectManager>()->GetBuildings();

    if (!buildings.empty())
    {
        for (auto& building : buildings)
        {
            building->Update(static_cast<float>(simulationDt));
        }
    }
    // Engine::GetLogger().LogEvent("GamePlayState::Update after buildings");
    FloatingTextManager::Instance().Update(static_cast<float>(simulationDt));

    // smokeEmitter.Update(environment.DeltaTime);

    // const auto& input = Engine::GetInput();
    //  if (input.KeyJustPressed(CS230::Input::Keys::P))
    //  {
    //      Math::vec2 emit_pos = camera.GetPosition();
    //      Math::vec2 emit_vel(0.0, 0.0);
    //      Math::vec2 direction(0.0, 200.0);
    //      smokeEmitter.Emit(5, emit_pos, emit_vel, direction, 3.14159 / 4, 0.5);
    //  }

    // Happiness & Immigration System
    auto resourceMgr = GetGSComponent<ResourceManager>();

    // Update city metrics with new systems
    bool shouldSpawnImmigrant = cityMetrics->UpdateSystems(simulationDt, gameObjMgr.get(), resourceMgr.get());
    // Engine::GetLogger().LogEvent("GamePlayState::Update after UpdateSystems");

    if (shouldSpawnImmigrant)
    {
        gameObjMgr->AddHuman(new Human(Human::CreateImmigrant(mapMgr->GetRoad())));
    }
    // Engine::GetLogger().LogEvent("GamePlayState::Update after immigration check");

    // End condition (Lose)
    if (cityMetrics->GetHappiness() <= 10.0f)
    {
        SDL_Event e{};
        e.type         = SDL_WINDOWEVENT;
        e.window.event = SDL_WINDOWEVENT_CLOSE;
        SDL_PushEvent(&e);
        return;
    }

    // Win condition
    int currentPopulation = static_cast<int>(gameObjMgr->GetHumans().size());
    if (cityMetrics->IsWinConditionMet(currentPopulation))
    {
        // std::cout << "Congratulations!" << std::endl;
        //  Game won !
    }

    // Achievement system
    {
        int buildingCount     = static_cast<int>(gameObjMgr->GetBuildings().size());
        int maxSingleResource = std::max({ resourceMgr->totalWood, resourceMgr->totalStone, resourceMgr->totalSteel });
        achievementManager.Update(static_cast<float>(dt), currentPopulation, buildingCount, cityMetrics->GetHappiness(), maxSingleResource);
    }

    // mouse hovering & building instructure
    auto        windowSize     = Engine::GetWindow().GetSize();
    Math::fvec2 mouseScreenPos = mouse.GetPositionFloat();
    mouseScreenPos.y           = static_cast<float>(windowSize.y) - mouseScreenPos.y;
    bool mouseClicked          = mouse.MouseJustPressed(Dokev::Mouse::Button::Left);

    auto previousUISelection = uiManager.GetSelectedBuilding();
    uiManager.Update(mouseScreenPos, mouseClicked, static_cast<float>(dt));

    // Check if button was clicked
    auto currentSelection = uiManager.GetSelectedBuilding();
    bool selectionChanged = previousUISelection != currentSelection;
    bool buttonClicked    = selectionChanged && (currentSelection != BuildingType::NONE);

    // Update tutorial
    const auto& input       = Engine::GetInput();
    bool        skipPressed = input.KeyJustPressed(CS230::Input::Keys::F);
    tutorialManager.Update(static_cast<float>(simulationDt), mouseClicked, skipPressed);
    // Engine::GetLogger().LogEvent("GamePlayState::Update after tutorial");

    if (buttonClicked)
    {
        tutorialManager.OnButtonClicked();
    }

    if (selectionChanged && currentSelection == BuildingType::ROAD && ismouseheld)
    {
        blockRoadPlacementUntilRelease = true;
    }
    ///////////////////////////////////////////////////////////
    // Update mouse picking for isometric, building placement//
    ///////////////////////////////////////////////////////////

    auto selectBuildingType = uiManager.GetSelectedBuilding();
    bool shouldplace        = mouseClicked || (ismouseheld && (selectBuildingType == BuildingType::ROAD || selectBuildingType == BuildingType::FLATTEN) && !blockRoadPlacementUntilRelease);

    if (shouldplace)
    {
        if (mapMgr && mapMgr->GetMap())
        {
            Math::ivec2 mouseScreen = mouse.GetPosition();

            mapMgr->GetMap()->IsometricDrawMouseAtlas(index, selected_renderer);

            mapMgr->GetMap()->EnsureMouseAtlasInitialized();
            if (mapMgr->GetMap()->GetMouseAtlasFBO() == 0)
            {
                // Engine::GetLogger().LogError("Mouse atlas FBO not initialized; skipping placement read.");
                return;
            }

            GLubyte pixel[4] = { 0 };

            GL::BindFramebuffer(GL_FRAMEBUFFER, mapMgr->GetMap()->mouseAtlasFBO);

            mouseScreen.y = windowSize.y - mouseScreen.y;

            GL::ReadPixels(static_cast<int>(mouseScreen.x), static_cast<int>(mouseScreen.y), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

            GL::BindFramebuffer(GL_FRAMEBUFFER, 0);


            float tileX = pixel[0] * static_cast<float>(GRID_SIZE_X - 1) / 255.0f;
            float tileY = pixel[1] * static_cast<float>(GRID_SIZE_Y - 1) / 255.0f;

            Math::fvec2 gridCoord = ConvertViewTileToGrid({ tileX, tileY }, index, mapMgr->GetMap());

            mouseSelection.Update(gridCoord.x, gridCoord.y);
            mouseSelection.SetHasValidPosition(true);

            auto selectedBuild = uiManager.GetSelectedBuilding();

            if (selectedBuild != BuildingType::NONE && previousUISelection == selectedBuild && mapMgr)
            {
                auto attemptPlacement = [&]()
                {
                    auto* map = mapMgr->GetMap();
                    if (!map)
                    {
                        return;
                    }

                    Math::fvec2 gridCoord = ConvertViewTileToGrid({ tileX, tileY }, index, map);
                    int         gridX     = std::clamp(static_cast<int>(std::round(gridCoord.x)), 0, GRID_SIZE_X - 1);
                    int         gridY     = std::clamp(static_cast<int>(std::round(gridCoord.y)), 0, GRID_SIZE_Y - 1);

                    auto* tileMgr = map->GetTileManager();
                    if (!tileMgr)
                    {
                        return;
                    }

                    GridCell** viewArray = map->GetViewArray(0);
                    if (!viewArray)
                    {
                        return;
                    }

                    GridCell* clickedCell = viewArray[gridY * GRID_SIZE_X + gridX];

                    if (selectedBuild == BuildingType::FLATTEN)
                    {
                        if (!clickedCell || clickedCell->getWater())
                        {
                            mouseSelection.SetHasValidPosition(false);
                            return;
                        }
                    }
                    else if (selectedBuild != BuildingType::DELETE)
                    {
                        if (!clickedCell || clickedCell->getWater() || !tileMgr->CanPlaceTile(gridX, gridY))
                        {
                            mouseSelection.SetHasValidPosition(false);
                            return;
                        }

                        if (selectedBuild != BuildingType::ROAD)
                        {
                            auto* road = mapMgr->GetRoad();
                            if (!road || !road->IsAdjacentToRoad(gridX, gridY))
                            {
                                mouseSelection.SetHasValidPosition(false);
                                return;
                            }
                        }
                    }
                    else
                    {
                        // For DELETE mode, only check for water
                        if (!clickedCell || clickedCell->getWater())
                        {
                            mouseSelection.SetHasValidPosition(false);
                            return;
                        }
                    }

                    if (!resourceMgr || !gameObjMgr)
                    {
                        return;
                    }

                    auto consumeResources = [&](BuildingType type) -> bool
                    {
                        BuildingCost cost = buildingCostManager.GetCost(type);
                        if (resourceMgr->totalWood < cost.wood || resourceMgr->totalStone < cost.stone || resourceMgr->totalSteel < cost.steel)
                            return false;

                        resourceMgr->totalWood -= cost.wood;
                        resourceMgr->totalStone -= cost.stone;
                        resourceMgr->totalSteel -= cost.steel;
                        return true;
                    };

                    bool placed = false;

                    switch (selectedBuild)
                    {
                        case BuildingType::ROAD:
                            {
                                if (lastPlacedRoadPos.x == gridX && lastPlacedRoadPos.y == gridY)
                                    break;

                                auto* road = mapMgr->GetRoad();
                                if (!road)
                                    break;
                                if (!consumeResources(BuildingType::ROAD))
                                    break;
                                road->addRoad(gridX, gridY, *map);
                                placed            = true;
                                lastPlacedRoadPos = { gridX, gridY };
                                break;
                            }
                        case BuildingType::HOME:
                            {
                                if (!consumeResources(BuildingType::HOME))
                                    break;

                                auto* building = new ResidentBuilding(Math::fvec2(static_cast<float>(gridX), static_cast<float>(gridY)), 1, 1, BuildingType::HOME);
                                gameObjMgr->AddBuilding(building);
                                tileMgr->AllocateTile(gridX, gridY, Dokev::TileType::RESIDENTIAL);
                                placed = true;
                                break;
                            }
                        case BuildingType::WOOD_WORKSHOP:
                            {
                                if (!consumeResources(BuildingType::WOOD_WORKSHOP))
                                    break;

                                auto* workshop = new WorkBuilding(Math::fvec2(static_cast<float>(gridX), static_cast<float>(gridY)), 1, 1, BuildingType::WOOD_WORKSHOP);
                                workshop->SetResourceManager(resourceMgr.get());
                                gameObjMgr->AddBuilding(workshop);
                                tileMgr->AllocateTile(gridX, gridY, Dokev::TileType::INDUSTRIAL);
                                placed = true;
                                break;
                            }
                        case BuildingType::ROCK_WORKSHOP:
                            {
                                if (!consumeResources(BuildingType::ROCK_WORKSHOP))
                                    break;

                                auto* workshop = new WorkBuilding(Math::fvec2(static_cast<float>(gridX), static_cast<float>(gridY)), 1, 1, BuildingType::ROCK_WORKSHOP);
                                workshop->SetResourceManager(resourceMgr.get());
                                gameObjMgr->AddBuilding(workshop);
                                tileMgr->AllocateTile(gridX, gridY, Dokev::TileType::INDUSTRIAL);
                                placed = true;
                                break;
                            }
                        case BuildingType::STEEL_WORKSHOP:
                            {
                                if (!consumeResources(BuildingType::STEEL_WORKSHOP))
                                    break;

                                auto* workshop = new WorkBuilding(Math::fvec2(static_cast<float>(gridX), static_cast<float>(gridY)), 1, 1, BuildingType::STEEL_WORKSHOP);
                                workshop->SetResourceManager(resourceMgr.get());
                                gameObjMgr->AddBuilding(workshop);
                                tileMgr->AllocateTile(gridX, gridY, Dokev::TileType::INDUSTRIAL);
                                placed = true;
                                break;
                            }
                        case BuildingType::DELETE:
                            {
                                mouseSelection.SetHasValidPosition(true);
                                gameObjMgr->RemoveBuilding(gridX, gridY);

                                auto* road = mapMgr->GetRoad();
                                if (road)
                                {
                                    road->removeRoad(gridX, gridY, *map);
                                }
                                else
                                {
                                    tileMgr->DeallocateTile(gridX, gridY);
                                }

                                placed = true;
                                break;
                            }
                        case BuildingType::FLATTEN:
                            {
                                mouseSelection.SetHasValidPosition(true);
                                if (map->FlattenTile(gridX, gridY))
                                {
                                    placed = true;
                                    break;
                                }

                                // static Math::ivec2 startPos;
                                // if (mouse.MouseJustPressed(Mouse::Button::Left))
                                // {
                                //     startPos = mouse.GetPosition();
                                //     std::cout << startPos.x << std::endl;
                                // }

                                // if (mouse.MouseJustReleased(Mouse::Button::Left))
                                // {
                                //     std::cout << "Released " << std::endl; // didnt work

                                //     Math::ivec2 endPos = mouse.GetPosition();

                                //     int minx = std::min(startPos.x, endPos.x);
                                //     int maxx = std::max(startPos.x, endPos.x);
                                //     int miny = std::min(startPos.y, endPos.y);
                                //     int maxy = std::max(startPos.y, endPos.y);

                                //     for (int y = miny; y <= maxy; y++)
                                //     {
                                //         for (int x = minx; x <= maxx; x++)
                                //         {
                                //             mouseSelection.SetHasValidPosition(true);

                                //             if (map->FlattenTile(x, y))
                                //             {
                                //                 placed = true;
                                //             }
                                //         }
                                //     }
                                // }
                            }

                        default: break;
                    }

                    if (placed)
                    {
                        if (selectedBuild == BuildingType::ROAD)
                        {
                            roadBuildSoundCount++;
                            if (roadBuildSoundCount == 1 || ((roadBuildSoundCount - 1) % 8 == 0))
                            {
                                CS230::AudioManager::Instance().PlaySFX("Assets/sounds/road.mp3");
                            }
                        }
                        else if (selectedBuild != BuildingType::DELETE && selectedBuild != BuildingType::FLATTEN)
                        {
                        }

                        if (input.KeyJustPressed(CS230::Input::Keys::Escape))
                        {
                            uiManager.SetSelectedBuilding(BuildingType::NONE);
                        }
                        tutorialManager.OnBuildingPlaced();
                    }
                };

                attemptPlacement();
            }

            if (mouseClicked && selectBuildingType == BuildingType::NONE)
            {
                Math::fvec2 grid = ConvertViewTileToGrid({ tileX, tileY }, index, mapMgr->GetMap());

                Human* closest     = nullptr;
                float  closestDist = 1.5f;
                for (const auto& h : gameObjMgr->GetHumans())
                {
                    Math::fvec2 hPos = h->GetGridPosition();
                    float       dx   = hPos.x - grid.x;
                    float       dy   = hPos.y - grid.y;
                    float       dist = std::sqrt(dx * dx + dy * dy);

                    if (dist < closestDist)
                    {
                        closestDist = dist;
                        closest     = h.get();
                    }
                }

                // Toggle off
                if (closest == selectedNPC)
                    selectedNPC = nullptr;
                else
                    selectedNPC = closest;

                playerUI.SetSelectedHuman(selectedNPC);
            }
        }
    }
    mouseSelection.SetViewIndex(index);
    // Engine::GetLogger().LogEvent("GamePlayState::Update end");
}

void GamePlayState::Draw() const
{
    // Engine::GetLogger().LogEvent("GamePlayState::Draw start");
    auto windowSize = Engine::GetWindow().GetSize();
    CS200::RenderingAPI::Clear();

    auto& renderer2d           = Engine::GetRenderer2D(selected_renderer);
    auto& renderer2d_IMMEDIATE = Engine::GetRenderer2D(Dokev::RenderState::IMMEDIATE);

    auto ndcMatrix = CS200::build_ndc_matrix(windowSize);

    auto cameraTransform = camera.GetViewMatrix();

    renderer2d.BeginScene(cameraTransform * ndcMatrix);
    renderer2d_IMMEDIATE.BeginScene(cameraTransform * ndcMatrix);

    GL::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // BackGround Color
    DrawRectangle(-5000, -5000, 10000, 10000, Dokev::ColorPalette::SKYBLUE);

    auto mapMgr = GetGSComponent<MapManager>();
    mapMgr->GetMap()->IsometricDrawMap(index, selected_renderer, &mouseSelection, GetGSComponent<Dokev::GameObjectManager>().get());

    // auto& buildings = GetGSComponent<Dokev::GameObjectManager>()->GetBuildings();
    // for (const auto& building : buildings)
    // {
    //     building->Draw(index, mapMgr->GetMap());
    // }

    FloatingTextManager::Instance().Draw(cameraTransform);

    const auto& resources = mapMgr->GetResources();

    // resource
    for (const auto& resource : resources)
    {
        resource->Draw(index, mapMgr->GetMap());
    }
    testTexture.DrawEx(0, 0, 90.0f, 1.0f);

    // Draw all humans
    auto& humans = GetGSComponent<Dokev::GameObjectManager>()->GetHumans();
    for (const auto& human : humans)
    {
        human->Draw(index, mapMgr->GetMap());
    }

    renderer2d.EndScene();
    renderer2d_IMMEDIATE.EndScene();

    auto ndcMatrixUI = CS200::build_ndc_matrix(windowSize);
    renderer2d.BeginScene(ndcMatrixUI);

    int humanCount = static_cast<int>(GetGSComponent<Dokev::GameObjectManager>()->GetHumans().size());
    uiManager.Draw(GetGSComponent<ResourceManager>().get(), GetGSComponent<CityMetrics>().get(), humanCount, const_cast<BuildingCostManager*>(&buildingCostManager));

    tutorialManager.Draw();
    achievementManager.Draw();

    renderer2d.EndScene();
    // Engine::GetLogger().LogEvent("GamePlayState::Draw end");

    // Draw smoke animation at position (200, 200)
    // Math::TransformationMatrix smokeTransform;
    // smokeTransform = Math::TranslationMatrix(Math::vec2(200.0, 200.0));
    // smokeSprite.Draw(smokeTransform);

    // Draw particles
    // smokeEmitter.Draw(cameraTransform * ndcMatrix);
}

void GamePlayState::DrawImGui()
{
    // Developer UI
    worldEditor.DrawImGui();

    if (ImGui::Begin("Game Menu"))
    {
        ImGui::Text("Simulation: %s", simulationPaused ? "Paused" : "Running");
        ImGui::Text("Current Speed: %s", kSimulationSpeedLabels[simulationSpeedPresetIndex]);
        ImGui::Text("Shortcuts: P pause, Left/Right speed");

        if (ImGui::Button(simulationPaused ? "Resume" : "Pause"))
        {
            simulationPaused = !simulationPaused;
        }

        for (std::size_t presetIndex = 0; presetIndex < kSimulationSpeedPresets.size(); ++presetIndex)
        {
            if (presetIndex != 0)
            {
                ImGui::SameLine();
            }

            if (ImGui::RadioButton(kSimulationSpeedLabels[presetIndex], simulationSpeedPresetIndex == presetIndex))
            {
                SetSimulationSpeedPreset(presetIndex);
            }
        }

        ImGui::Separator();

        if (ImGui::Button("Back to Main Menu"))
        {
            auto& gsm = Engine::GetGameStateManager();
            gsm.PopState();
            gsm.PushState<MainMenu>();
            ImGui::End();
            return;
        }
    }
    ImGui::End();

    // Player UI
    auto cityMetrics = GetGSComponent<CityMetrics>();
    auto gameObjMgr  = GetGSComponent<Dokev::GameObjectManager>();

    if (cityMetrics && gameObjMgr)
    {
        if (selectedNPC)
        {
            bool found = false;
            for (const auto& h : gameObjMgr->GetHumans())
            {
                if (h.get() == selectedNPC)
                {
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                selectedNPC = nullptr;
                playerUI.SetSelectedHuman(nullptr);
            }
        }

        playerUI.DrawUI(cityMetrics->GetHappinessManager(), cityMetrics->GetImmigrationManager(), gameObjMgr.get(), cityMetrics.get());
    }
}

void GamePlayState::Unload()
{
    Engine::GetLogger().LogEvent("GamePlayState: Unloaded");
}

gsl::czstring GamePlayState::GetName() const
{
    return "IGY";
}

void GamePlayState::SaveGame(const std::string& filename)
{
    json j;
    Engine::GetLogger().LogEvent("Saving game to " + filename);

    //  Save Player Resources
    auto resourceMgr = GetGSComponent<ResourceManager>();
    if (resourceMgr)
    {
        j["PlayerResources"] = {
            {  "Wood",  resourceMgr->totalWood },
            { "Stone", resourceMgr->totalStone },
            { "Steel", resourceMgr->totalSteel }
        };
    }

    auto mapMgr = GetGSComponent<MapManager>();
    if (mapMgr)
    {
        //  Save Map Resources
        json& j_map_resources = j["MapResources"];
        for (const auto& res : mapMgr->GetResources())
        {
            j_map_resources.push_back({
                {          "Type",          res->GetType() },
                {             "X",         res->GetGridX() },
                {             "Y",         res->GetGridY() },
                { "CurrentAmount", res->GetCurrentAmount() }
            });
        }

        //  Save Road Info (based on TileManager)
        json& j_roads = j["Roads"];
        auto  tileMgr = mapMgr->GetMap()->GetTileManager();
        if (tileMgr)
        {
            auto roadTiles = tileMgr->GetTilesOfType(Dokev::TileType::ROAD);
            for (const auto& pos : roadTiles)
            {
                j_roads.push_back({ pos.x, pos.y });
            }
        }
    }

    //  Save Human Info
    auto gameObjMgr = GetGSComponent<Dokev::GameObjectManager>();
    if (gameObjMgr)
    {
        json& j_humans = j["Humans"];
        for (const auto& human : gameObjMgr->GetHumans())
        {
            j_humans.push_back({
                {  "Position", { human->GetGridPosition().x, human->GetGridPosition().y } },
                {     "State",                                          human->GetState() },
                { "Happiness",                                      human->GetHappiness() }
            });
        }
    }

    //  Save Camera Position
    j["Camera"] = {
        { "Position", { camera.GetPosition().x, camera.GetPosition().y } },
        {     "Zoom",                                   camera.GetZoom() }
    };

    // Write to file
    try
    {
        std::ofstream o(filename);
        o << std::setw(4) << j << std::endl;
    }
    catch (const std::exception& e)
    {
        Engine::GetLogger().LogError("Failed to save game: " + std::string(e.what()));
    }
}

void GamePlayState::LoadGame(const std::string& filename)
{
    json j;
    // Engine::GetLogger().LogEvent("Loading game from " + filename);

    // Read file
    try
    {
        std::ifstream i(filename);
        if (!i.is_open())
        {
            Engine::GetLogger().LogError("Save file not found: " + filename);
            return;
        }
        i >> j;
    }
    catch (const std::exception& e)
    {
        Engine::GetLogger().LogError("Failed to load/parse game file: " + std::string(e.what()));
        return;
    }

    // --- Reset State ---
    GetGSComponent<Dokev::GameObjectManager>()->Unload();

    auto mapMgr = GetGSComponent<MapManager>();
    if (mapMgr)
    {
        // Clear existing map-bound entities
        mapMgr->GetResources().clear();
        mapMgr->GetRoad()->removeAllRoad(*mapMgr->GetMap());
    }
    else
    {
        return; // Stop loading if MapManager doesn't exist
    }


    //  Load Player Resources
    auto resourceMgr = GetGSComponent<ResourceManager>();
    if (resourceMgr && j.contains("PlayerResources"))
    {
        resourceMgr->SetResources(j["PlayerResources"].value("Wood", 0), j["PlayerResources"].value("Stone", 0), j["PlayerResources"].value("Steel", 0));
    }

    //  Load Map Resources
    if (j.contains("MapResources"))
    {
        auto& resources = mapMgr->GetResources();
        for (const auto& resData : j["MapResources"])
        {
            ResourceType type   = resData.at("Type").get<ResourceType>();
            int          x      = resData.at("X").get<int>();
            int          y      = resData.at("Y").get<int>();
            int          amount = resData.at("CurrentAmount").get<int>();

            if (type == ResourceType::Wood)
                resources.push_back(std::make_unique<Wood>(x, y, amount));
            else if (type == ResourceType::Stone)
                resources.push_back(std::make_unique<Stone>(x, y, amount));
            else if (type == ResourceType::Steel)
                resources.push_back(std::make_unique<Steel>(x, y, amount));
        }
    }

    //  Load Road Info
    if (j.contains("Roads"))
    {
        auto road = mapMgr->GetRoad();
        auto map  = mapMgr->GetMap();
        for (const auto& pos : j["Roads"])
        {
            road->addRoad(pos[0].get<int>(), pos[1].get<int>(), *map);
        }
    }

    //  Load Human Info
    auto gameObjMgr = GetGSComponent<Dokev::GameObjectManager>();
    if (gameObjMgr && j.contains("Humans"))
    {
        for (const auto& humanData : j["Humans"])
        {
            Math::fvec2 pos      = { humanData["Position"][0].get<float>(), humanData["Position"][1].get<float>() };
            Human*      newHuman = new Human(pos);
            gameObjMgr->AddHuman(newHuman);
        }
    }

    //  Load Camera Position
    if (j.contains("Camera"))
    {
        camera.SetPosition({ j["Camera"]["Position"][0].get<double>(), j["Camera"]["Position"][1].get<double>() });
        camera.SetZoom(j["Camera"].value("Zoom", 0.3f));
    }

    Engine::GetLogger().LogEvent("Game loaded successfully.");
}
