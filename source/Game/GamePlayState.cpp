/**
 * \file GamePlayState.cpp
 * \author Hyunwoo Yang
 * \author
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "GamePlayState.hpp"
#include "Building.hpp"
#include "CS200/IRenderer2D.hpp"
#include "CS200/NDC.hpp"
#include "CS200/RGBA.hpp"
#include "CS200/RenderingAPI.hpp"
#include "Dokev/DKCityMetrics.hpp"
#include "Dokev/DKColor.hpp"
#include "Dokev/DKGameObjectManager.hpp"
#include "Dokev/DKImage.hpp"
#include "Dokev/DKMetaProgramming.hpp"
#include "Dokev/DKParticles.hpp"
#include "Dokev/DKShapes.hpp"
#include "Dokev/DKTexture.hpp"
#include "GridMap.hpp"
#include "Human.hpp"
#include "ResidentBuilding.hpp"
#include "WorkBuilding.hpp"


#include "Engine/Engine.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Input.hpp"
#include "Engine/Logger.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Path.hpp"
#include "Engine/Window.hpp"
#include "Game/Resource.hpp"
#include "Game/ResourceManager.hpp"
#include "Game/Road.hpp"
#include "Game/Steel.hpp"
#include "Game/Stone.hpp"
#include "Game/Wood.hpp"
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

    // Initialize MapManager component
    AddGSComponent<MapManager>();
    GetGSComponent<MapManager>()->Initialize();

    // Initialize ResourceManager
    AddGSComponent<ResourceManager>();

    // Initialize CityMetrics
    AddGSComponent<CityMetrics>();

    // Initialize GameObjectManager component
    AddGSComponent<Dokev::GameObjectManager>();
    GetGSComponent<Dokev::GameObjectManager>()->AddHuman(new Human(Math::fvec2(5.0f, 5.0f)));
    GetGSComponent<Dokev::GameObjectManager>()->AddHuman(new Human(Math::fvec2(78.0f, 13.0f)));

    camera.SetZoom(0.3f);

    smokeSprite = Dokev::AnimatedSprite(assets::locate_asset("Assets/images/Smoke.spt"));

    worldEditor.Initialize(
        GetGSComponent<MapManager>().get(), &camera, &index, &immigrationTimer, &immigrationInterval, GetGSComponent<Dokev::GameObjectManager>().get(), GetGSComponent<ResourceManager>().get());

    // Initialize UI Manager
    uiManager.Initialize();

    // Initialize Tutorial Manager
    tutorialManager.Initialize(&uiManager);

    // Load building costs from JSON
    buildingCostManager.LoadFromFile("Assets/resources/building_costs.json");

    mouseSelection.SetGridMap(GetGSComponent<MapManager>()->GetMap());
    mouseSelection.Load(assets::locate_asset("Assets/images/textures/select.png"), assets::locate_asset("Assets/images/textures/selectRed.png"));
}

void GamePlayState::Update(double dt)
{
    [[maybe_unused]] const auto& environment = Engine::GetWindowEnvironment();
    camera.UpdateZoomInterpolation(dt);
    worldEditor.UpdateInput();

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

    UpdateGSComponents(dt);

    auto cityMetrics = GetGSComponent<CityMetrics>();
    if (cityMetrics)
    {
        cityMetrics->Update(static_cast<float>(dt));
    }

    // Update all humans
    auto  mapMgr = GetGSComponent<MapManager>();
    auto& humans = GetGSComponent<Dokev::GameObjectManager>()->GetHumans();

    if (!humans.empty())
    {
        for (auto& human : humans)
        {
            human->Update(static_cast<float>(dt), mapMgr->GetMap(), mapMgr->GetRoad());
        }
    }

    GetGSComponent<Dokev::GameObjectManager>()->Refresh();

    auto& buildings = GetGSComponent<Dokev::GameObjectManager>()->GetBuildings();

    if (!buildings.empty())
    {
        for (auto& building : buildings)
        {
            building->Update(static_cast<float>(dt));
        }
    }

    smokeEmitter.Update(environment.DeltaTime);

    // const auto& input = Engine::GetInput();
    //  if (input.KeyJustPressed(CS230::Input::Keys::P))
    //  {
    //      Math::vec2 emit_pos = camera.GetPosition();
    //      Math::vec2 emit_vel(0.0, 0.0);
    //      Math::vec2 direction(0.0, 200.0);
    //      smokeEmitter.Emit(5, emit_pos, emit_vel, direction, 3.14159 / 4, 0.5);
    //  }

    // Auto immigration system
    auto gameObjMgr  = GetGSComponent<Dokev::GameObjectManager>();
    auto resourceMgr = GetGSComponent<ResourceManager>();

    size_t humanCount = gameObjMgr->GetHumans().size();
    size_t homeCount  = 0;

    for (const auto& building : gameObjMgr->GetBuildings())
    {
        if (Dokev::Meta::is_residential_type(building->GetBuildingType()))
        {
            homeCount++;
        }
    }

    // immigration condition
    int totalResources    = resourceMgr->GetTotalResources();
    int requiredResources = static_cast<int>(humanCount) * 100;

    if (humanCount < homeCount && totalResources >= requiredResources)
    {
        gameObjMgr->AddHuman(new Human(Human::CreateImmigrant()));
    }

    // end condition
    if (gameObjMgr->GetHumans().size() > 10 || cityMetrics->GetHappiness() <= 10.0f)
    {
        SDL_Event e{};
        e.type         = SDL_WINDOWEVENT;
        e.window.event = SDL_WINDOWEVENT_CLOSE;
        SDL_PushEvent(&e);
        return;
    }

    // Happiness system based on population, workplaces, and homes
    size_t workplaceCount = 0;
    for (const auto& building : gameObjMgr->GetBuildings())
    {
        BuildingType type = building->GetBuildingType();
        if (Dokev::Meta::is_workshop_type(type))
        {
            workplaceCount++;
        }
    }

    float happinessChangeRate = 0.0f;

    if (homeCount > humanCount)
    {
        // 사람이 거주지보다 많음- 만족도 천천히 증가
        happinessChangeRate = 1.0f;
    }
    else if (workplaceCount == 0 && homeCount > 0)
    {
        // 거주지만 있음 - 만족도 빠르게 증가 (최상)
        happinessChangeRate = 1.5f;
    }
    else if (humanCount >= workplaceCount && homeCount == 0)
    {
        // 사람이 워크샾보다 많고 거주지 없음 - 만족도 빠르게 감소
        happinessChangeRate = -2.0f;
    }
    else if (workplaceCount > humanCount && humanCount > homeCount)
    {
        // 워크샾이 사람보다 많고 거주지도 부족함 - 만족도 매우 빠르게 감소 (최악)
        happinessChangeRate = -1.5f;
    }
    else if (workplaceCount > humanCount && homeCount > 0)
    {
        // 워크샾이 사람보다 많고 거주지는 있음 - 만족도 천천히 감소
        happinessChangeRate = -1.0f;
    }

    if (cityMetrics)
    {
        cityMetrics->SetHappinessChangeRate(happinessChangeRate);
    }

    // mouse hovering & building instructure
    mouse.Update();
    auto        windowSize     = Engine::GetWindow().GetSize();
    Math::fvec2 mouseScreenPos = mouse.GetPositionFloat();
    mouseScreenPos.y           = static_cast<float>(windowSize.y) - mouseScreenPos.y;
    bool mouseClicked          = mouse.MouseJustPressed(Dokev::Mouse::Button::Left);

    auto previousUISelection = uiManager.GetSelectedBuilding();
    uiManager.Update(mouseScreenPos, mouseClicked);

    // Check if button was clicked
    auto currentSelection = uiManager.GetSelectedBuilding();
    bool buttonClicked    = (previousUISelection != currentSelection) && (currentSelection != BuildingType::NONE);

    // Update tutorial
    const auto& input       = Engine::GetInput();
    bool        skipPressed = input.KeyJustPressed(CS230::Input::Keys::F);
    tutorialManager.Update(static_cast<float>(dt), mouseClicked, skipPressed);

    if (buttonClicked)
    {
        tutorialManager.OnButtonClicked();
    }
    ///////////////////////////////////////////////////////////
    // Update mouse picking for isometric, building placement//
    ///////////////////////////////////////////////////////////
    if (mouseClicked)
    {
        if (mapMgr && mapMgr->GetMap())
        {
            Math::ivec2 mouseScreen = mouse.GetPosition();

            mapMgr->GetMap()->IsometricDrawMouseAtlas(index, selected_renderer);

            GLubyte pixel[4] = { 0 };
            glBindFramebuffer(GL_FRAMEBUFFER, mapMgr->GetMap()->mouseAtlasFBO);

            mouseScreen.y = windowSize.y - mouseScreen.y;

            glReadPixels(static_cast<int>(mouseScreen.x), static_cast<int>(mouseScreen.y), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

                    if (selectedBuild != BuildingType::DELETE)
                    {
                        if (!clickedCell || clickedCell->getWater() || !tileMgr->CanPlaceTile(gridX, gridY))
                        {
                            // Invalid placement  -> give bool type to mouse selction and set to false
                            //-> draw red texture on mouse selction draw function
                            mouseSelection.SetHasValidPosition(false);
                            return;
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
                                auto* road = mapMgr->GetRoad();
                                if (!road)
                                    break;
                                if (!consumeResources(BuildingType::ROAD))
                                    break;
                                road->addRoad(gridX, gridY, *map);
                                placed = true;
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
                                workshop->AddWorker(1);
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
                                workshop->AddWorker(1);
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
                                workshop->AddWorker(1);
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
                                    // RemoveRoad already has Deallocate Tile
                                    road->removeRoad(gridX, gridY, *map);
                                }
                                else
                                {
                                    tileMgr->DeallocateTile(gridX, gridY);
                                }

                                placed = true;
                                break;
                            }

                        default: break;
                    }

                    if (placed)
                    {
                        uiManager.SetSelectedBuilding(BuildingType::NONE);
                        tutorialManager.OnBuildingPlaced();
                    }
                };

                attemptPlacement();
            }
        }
    }
    mouseSelection.SetViewIndex(index);
}

void GamePlayState::Draw() const
{
    CS200::RenderingAPI::Clear();

    auto& renderer2d           = Engine::GetRenderer2D(selected_renderer);
    auto& renderer2d_IMMEDIATE = Engine::GetRenderer2D(Dokev::RenderState::IMMEDIATE);

    auto windowSize = Engine::GetWindow().GetSize();
    auto ndcMatrix  = CS200::build_ndc_matrix(windowSize);

    auto cameraTransform = camera.GetViewMatrix();

    renderer2d.BeginScene(cameraTransform * ndcMatrix);
    renderer2d_IMMEDIATE.BeginScene(cameraTransform * ndcMatrix);

    GL::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    auto mapMgr = GetGSComponent<MapManager>();
    mapMgr->GetMap()->IsometricDrawMap(index, selected_renderer, &mouseSelection);

    const auto& resources = mapMgr->GetResources();
    // resource
    for (const auto& resource : resources)
    {
        resource->Draw(index, mapMgr->GetMap());
    }
    testTexture.DrawEx(0, 0, 90.0f, 1.0f);

    // Draw smoke animation at position (200, 200)
    Math::TransformationMatrix smokeTransform;
    smokeTransform = Math::TranslationMatrix(Math::vec2(200.0, 200.0));
    // smokeSprite.Draw(smokeTransform);

    // Draw particles
    smokeEmitter.Draw(cameraTransform * ndcMatrix);

    // Draw all humans
    auto& humans = GetGSComponent<Dokev::GameObjectManager>()->GetHumans();
    for (const auto& human : humans)
    {
        human->Draw(index, mapMgr->GetMap());
    }

    auto& buildings = GetGSComponent<Dokev::GameObjectManager>()->GetBuildings();
    for (const auto& building : buildings)
    {
        building->Draw(index, mapMgr->GetMap());
    }

    renderer2d.EndScene();
    renderer2d_IMMEDIATE.EndScene();

    auto ndcMatrixUI = CS200::build_ndc_matrix(windowSize);
    renderer2d.BeginScene(ndcMatrixUI);

    int humanCount = static_cast<int>(GetGSComponent<Dokev::GameObjectManager>()->GetHumans().size());
    uiManager.Draw(GetGSComponent<ResourceManager>().get(), GetGSComponent<CityMetrics>().get(), humanCount, const_cast<BuildingCostManager*>(&buildingCostManager));

    tutorialManager.Draw();

    renderer2d.EndScene();
}

void GamePlayState::DrawImGui()
{
    worldEditor.DrawImGui();
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
    Engine::GetLogger().LogEvent("Loading game from " + filename);

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
