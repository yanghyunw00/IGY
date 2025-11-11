#include "Wood.h"
#include "Dokev/DKUtility.hpp" 

Wood::Wood(int x, int y, int amount)
    : Resource(x, y, amount, ResourceType::Wood)
{
    int texNum = Dokev::GetRandomValue(1, 5);
    std::string textureName = "Assets/resources/tree_" + std::to_string(texNum) + ".png";
    texture = Dokev::LoadTexture(textureName);
}