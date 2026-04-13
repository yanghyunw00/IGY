/**
 * \file NPCNameManager.cpp
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "NPCNameManager.hpp"
#include "Dokev/Core/DKUtility.hpp"
#include "Engine/Path.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

NPCNameManager& NPCNameManager::Instance()
{
    static NPCNameManager instance;
    return instance;
}

NPCNameManager::NPCNameManager()
{
    Load();
}

void NPCNameManager::Load()
{
    try
    {
        std::ifstream file(assets::locate_asset("Assets/resources/npc_names.json").string());
        if (!file.is_open())
            return;

        json j;
        file >> j;

        if (j.contains("first_names"))
            firstNames = j["first_names"].get<std::vector<std::string>>();

        if (j.contains("last_names"))
            lastNames = j["last_names"].get<std::vector<std::string>>();

        loaded = !firstNames.empty() && !lastNames.empty();
    }
    catch (...)
    {
        std::cout << " No npc names" << std::endl;
    }
}

std::string NPCNameManager::GenerateName()
{
    if (!loaded || firstNames.empty() || lastNames.empty())
        return "Unknown";

    int r  = Dokev::GetRandomInt(0, static_cast<int>(firstNames.size()) - 1);
    int r2 = Dokev::GetRandomInt(0,  static_cast<int>(lastNames.size()) - 1);

    return firstNames[r] + " " + lastNames[r2];
}
