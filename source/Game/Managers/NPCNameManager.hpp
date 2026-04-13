/**
 * \file NPCNameManager.hpp
 * \brief 
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include <string>
#include <vector>

class NPCNameManager
{
public:
    static NPCNameManager& Instance();
    std::string GenerateName();

private:
    NPCNameManager();

    std::vector<std::string> firstNames;
    std::vector<std::string> lastNames;

    bool loaded = false;
    void Load();
};
