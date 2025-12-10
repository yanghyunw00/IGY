#include "Steel.hpp"
#include "Dokev/Core/DKUtility.hpp"
#include "Engine/Path.hpp"

Steel::Steel(int x, int y, int amount)
    : Resource(x, y, amount, ResourceType::Steel)
{
    int texNum = Dokev::GetRandomValue(1, 3);
    std::string textureName = "Assets/6432/steel_" + std::to_string(texNum) + ".png";
    texture = Dokev::LoadTexture(assets::locate_asset(textureName).string());
}