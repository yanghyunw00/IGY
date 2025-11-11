/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author Hyunseok Choi
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "GameStateManager.hpp"

namespace CS230
{
    void GameStateManager::PopState()
    {
        using namespace std::literals;
        auto* const state = mGameStateStack.back().get();
        mToClear.push_back(std::move(mGameStateStack.back()));
        mGameStateStack.erase(mGameStateStack.end() - 1);
        Engine::GetLogger().LogEvent("Exiting state "s + state->GetName());
        state->Unload();
    }

    void GameStateManager::Update()
    {
        mToClear.clear();
        const auto& environment = Engine::GetWindowEnvironment();
        mGameStateStack.back()->Update(environment.DeltaTime);
    }

    void GameStateManager::Draw()
    {
        for (auto& game_state : mGameStateStack)
        {
            game_state->Draw();
        }
    }

    void GameStateManager::DrawImGui()
    {
        mGameStateStack.back()->DrawImGui();
    }

    void GameStateManager::Clear()
    {
        while (!mGameStateStack.empty())
            PopState();
        mToClear.clear();
    }

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