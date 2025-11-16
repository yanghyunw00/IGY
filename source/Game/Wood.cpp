#include "Wood.hpp"
#include "Dokev/DKUtility.hpp"
#include "Engine/Path.hpp"

Wood::Wood(int x, int y, int amount)
    : Resource(x, y, amount, ResourceType::Wood)
{
    int texNum = Dokev::GetRandomValue(1, 5);
    std::string textureName = "Assets/6432/tree_" + std::to_string(texNum) + ".png";
    texture = Dokev::LoadTexture(assets::locate_asset(textureName).string());
}