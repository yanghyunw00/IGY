/**
 * \file GamePlayState.cpp
 * \author Hyunwoo Yang
 * \author
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "GamePlayState.hpp"
#include "CS200/IRenderer2D.hpp"
#include "CS200/NDC.hpp"
#include "CS200/RGBA.hpp"
#include "CS200/RenderingAPI.hpp"
#include "Dokev/DKColor.hpp"
#include "Dokev/DKImage.hpp"
#include "Dokev/DKParticles.hpp"
#include "Dokev/DKShapes.hpp"
#include "Dokev/DKTexture.hpp"
#include "Dokev/DKGameObjectManager.hpp"
#include "Human.hpp"
#include "GridMap.hpp"
#include "Building.hpp"
#include "ResidentBuilding.hpp"
#include "WorkBuilding.hpp"

#include "Engine/Path.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Input.hpp"
#include "Engine/Logger.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Window.hpp"
#include "OpenGL/GL.hpp"
#include <GL/glew.h>
#include <SDL.h>
#include <cmath>
#include <filesystem>
#include <imgui.h>
#include <iomanip>
#include <numbers>
#include <sstream>
#include "Game/Resource.hpp"
#include "Game/ResourceManager.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include "Game/Wood.hpp"
#include "Game/Stone.hpp"
#include "Game/Steel.hpp"
#include "Game/Road.hpp"

using json = nlohmann::json;

NLOHMANN_JSON_SERIALIZE_ENUM( ResourceType, {
    {ResourceType::Wood, "Wood"},
    {ResourceType::Stone, "Stone"},
    {ResourceType::Steel, "Steel"}
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

    // Initialize GameObjectManager component
    AddGSComponent<Dokev::GameObjectManager>();
    GetGSComponent<Dokev::GameObjectManager>()->AddHuman(new Human(Math::fvec2(5.0f, 5.0f)));
    GetGSComponent<Dokev::GameObjectManager>()->AddHuman(new Human(Math::fvec2(78.0f, 13.0f)));

    camera.SetZoom(0.3f);

    try
    {
        smokeSprite = Dokev::AnimatedSprite(assets::locate_asset("Assets/images/Smoke.spt"));
    }
    catch (const std::exception& e)
    {
        printf("Failed to load smoke animation: %s\n", e.what());
    }

    worldEditor.Initialize(GetGSComponent<MapManager>().get(), &camera, &index,
                          &immigrationTimer, &immigrationInterval,
                          GetGSComponent<Dokev::GameObjectManager>().get(),
                          GetGSComponent<ResourceManager>().get());

    // Initialize UI Manager
    uiManager.Initialize();
}

void GamePlayState::Update(double dt)
{
    [[maybe_unused]] const auto& environment = Engine::GetWindowEnvironment();
    worldEditor.UpdateInput();
    const auto& input = Engine::GetInput();
    
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

    // Update all humans
    auto mapMgr = GetGSComponent<MapManager>();
    auto& humans = GetGSComponent<Dokev::GameObjectManager>()->GetHumans();
    for (auto& human : humans)
    {
        human->Update(static_cast<float>(dt), mapMgr->GetMap(), mapMgr->GetRoad());
    }
    GetGSComponent<Dokev::GameObjectManager>()->Refresh();

    smokeEmitter.Update(environment.DeltaTime);

    if (input.KeyJustPressed(CS230::Input::Keys::P))
    {
        Math::vec2 emit_pos = camera.GetPosition();
        Math::vec2 emit_vel(0.0, 0.0);
        Math::vec2 direction(0.0, 200.0);
        smokeEmitter.Emit(5, emit_pos, emit_vel, direction, 3.14159 / 4, 0.5);
    }

    // Auto immigration system
    immigrationTimer += static_cast<float>(dt);
    if (immigrationTimer >= immigrationInterval)
    {
        immigrationTimer = 0.0f;
        //GetGSComponent<Dokev::GameObjectManager>()->AddHuman(new Human(Human::CreateImmigrant()));
    }

    mouse.Update();
    auto windowSize = Engine::GetWindow().GetSize();
    Math::fvec2 mouseScreenPos = mouse.GetPositionFloat();
    mouseScreenPos.y = static_cast<float>(windowSize.y) - mouseScreenPos.y;
    bool mouseClicked = mouse.MouseJustPressed(Dokev::Mouse::Button::Left);

    auto previousUISelection = uiManager.GetSelectedBuilding();
    uiManager.Update(mouseScreenPos, mouseClicked);

    if (mouseClicked && uiManager.GetSelectedBuilding() == previousUISelection)
    {
        auto mapMgr = GetGSComponent<MapManager>();
        if (mapMgr && mapMgr->GetMap())
        {
            mapMgr->GetMap()->IsometricDrawMouseAtlas(index, selected_renderer);

            GLubyte pixel[4] = { 0 };
            glBindFramebuffer(GL_FRAMEBUFFER, mapMgr->GetMap()->mouseAtlasFBO);

            Math::ivec2 mouseScreen = mouse.GetPosition();
            mouseScreen.y = windowSize.y - mouseScreen.y;

            glReadPixels(static_cast<int>(mouseScreen.x), static_cast<int>(mouseScreen.y),
                        1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            float tileX = pixel[0] * static_cast<float>(GRID_SIZE_X - 1) / 255.0f;
            float tileY = pixel[1] * static_cast<float>(GRID_SIZE_Y - 1) / 255.0f;

        }
    }
}

void GamePlayState::Draw() const
{
    CS200::RenderingAPI::Clear();

    auto& renderer2d = Engine::GetRenderer2D(selected_renderer);
    auto& renderer2d_IMMEDIATE = Engine::GetRenderer2D(Dokev::RenderState::IMMEDIATE);

    auto windowSize = Engine::GetWindow().GetSize();
    auto ndcMatrix  = CS200::build_ndc_matrix(windowSize);

    auto cameraTransform = camera.GetViewMatrix();

    renderer2d.BeginScene(cameraTransform * ndcMatrix);
    renderer2d_IMMEDIATE.BeginScene(cameraTransform * ndcMatrix);

    GL::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    auto mapMgr = GetGSComponent<MapManager>();
    mapMgr->GetMap()->IsometricDrawMap(index, selected_renderer);

    const auto& resources = mapMgr->GetResources();
    //resource
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

    // Draw all buildings
    auto& buildings = GetGSComponent<Dokev::GameObjectManager>()->GetBuildings();
    for(const auto& building : buildings)
    {
        building->Draw(index, mapMgr->GetMap());
    }

    renderer2d.EndScene();
    renderer2d_IMMEDIATE.EndScene();

    auto ndcMatrixUI = CS200::build_ndc_matrix(windowSize);
    renderer2d.BeginScene(ndcMatrixUI);
    uiManager.Draw(GetGSComponent<ResourceManager>().get());
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

    // 1. Save Player Resources
    auto resourceMgr = GetGSComponent<ResourceManager>();
    if (resourceMgr)
    {
        j["PlayerResources"] = {
            {"Wood", resourceMgr->totalWood},
            {"Stone", resourceMgr->totalStone},
            {"Steel", resourceMgr->totalSteel}
        };
    }

    auto mapMgr = GetGSComponent<MapManager>();
    if (mapMgr)
    {
        // 2. Save Map Resources
        json& j_map_resources = j["MapResources"];
        for (const auto& res : mapMgr->GetResources())
        {
            j_map_resources.push_back({
                {"Type", res->GetType()},
                {"X", res->GetGridX()},
                {"Y", res->GetGridY()},
                {"CurrentAmount", res->GetCurrentAmount()}
            });
        }

        // 3. Save Road Info (based on TileManager)
        json& j_roads = j["Roads"];
        auto tileMgr = mapMgr->GetMap()->GetTileManager();
        if (tileMgr)
        {
            auto roadTiles = tileMgr->GetTilesOfType(Dokev::TileType::ROAD);
            for (const auto& pos : roadTiles)
            {
                j_roads.push_back({pos.x, pos.y});
            }
        }
    }

    // 4. Save Human Info
    auto gameObjMgr = GetGSComponent<Dokev::GameObjectManager>();
    if (gameObjMgr)
    {
        json& j_humans = j["Humans"];
        for (const auto& human : gameObjMgr->GetHumans())
        {
            j_humans.push_back({
                {"Position", {human->GetGridPosition().x, human->GetGridPosition().y}},
                {"State", human->GetState()}, 
                {"Happiness", human->GetHappiness()}
            });
        }
    }
    
    // 5. Save Camera Position
    j["Camera"] = {
        {"Position", {camera.GetPosition().x, camera.GetPosition().y}},
        {"Zoom", camera.GetZoom()}
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


    // 1. Load Player Resources
    auto resourceMgr = GetGSComponent<ResourceManager>();
    if (resourceMgr && j.contains("PlayerResources"))
    {
        resourceMgr->SetResources(
            j["PlayerResources"].value("Wood", 0),
            j["PlayerResources"].value("Stone", 0),
            j["PlayerResources"].value("Steel", 0)
        );
    }

    // 2. Load Map Resources
    if (j.contains("MapResources"))
    {
        auto& resources = mapMgr->GetResources(); 
        for (const auto& resData : j["MapResources"])
        {
            ResourceType type = resData.at("Type").get<ResourceType>();
            int x = resData.at("X").get<int>();
            int y = resData.at("Y").get<int>();
            int amount = resData.at("CurrentAmount").get<int>();

            if (type == ResourceType::Wood)
                resources.push_back(std::make_unique<Wood>(x, y, amount));
            else if (type == ResourceType::Stone)
                resources.push_back(std::make_unique<Stone>(x, y, amount));
            else if (type == ResourceType::Steel)
                resources.push_back(std::make_unique<Steel>(x, y, amount));
        }
    }

    // 3. Load Road Info
    if (j.contains("Roads"))
    {
        auto road = mapMgr->GetRoad();
        auto map = mapMgr->GetMap();
        for (const auto& pos : j["Roads"])
        {
            road->addRoad(pos[0].get<int>(), pos[1].get<int>(), *map);
        }
    }

    // 4. Load Human Info
    auto gameObjMgr = GetGSComponent<Dokev::GameObjectManager>();
    if (gameObjMgr && j.contains("Humans"))
    {
        for (const auto& humanData : j["Humans"])
        {
            Math::fvec2 pos = {humanData["Position"][0].get<float>(), humanData["Position"][1].get<float>()};
            Human* newHuman = new Human(pos);
            gameObjMgr->AddHuman(newHuman);
        }
    }
    
    // 5. Load Camera Position
    if (j.contains("Camera"))
    {
        camera.SetPosition({j["Camera"]["Position"][0].get<double>(), j["Camera"]["Position"][1].get<double>()});
        camera.SetZoom(j["Camera"].value("Zoom", 0.3f));
    }
    
    Engine::GetLogger().LogEvent("Game loaded successfully.");
}