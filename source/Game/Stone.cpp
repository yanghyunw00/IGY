#include "Stone.hpp"
#include "Dokev/DKUtility.hpp" 

Stone::Stone(int x, int y, int amount)
    : Resource(x, y, amount, ResourceType::Stone)
{
    int texNum = Dokev::GetRandomValue(1, 4);
    std::string textureName = "Assets/6432/rock_" + std::to_string(texNum) + ".png";
    texture = Dokev::LoadTexture(textureName);
}