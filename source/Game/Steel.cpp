#include "Steel.hpp"
#include "Dokev/DKUtility.hpp"

Steel::Steel(int x, int y, int amount)
    : Resource(x, y, amount, ResourceType::Steel)
{
    int texNum = Dokev::GetRandomValue(1, 3);
    std::string textureName = "Assets/resources/steel_" + std::to_string(texNum) + ".png";
    texture = Dokev::LoadTexture(textureName);
}