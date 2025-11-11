#pragma once
#include <string>
#include "Dokev/DKTexture.hpp"

class GridMap;

enum class ResourceType
{
    Wood,
    Steel,
    Stone
};

class Resource
{
protected:
    int gridX;
    int gridY;
    int currentAmount;
    int maxAmount;
    ResourceType type;
    bool isOccupied;
    
    Dokev::Texture2D texture;

public:
    Resource(int x, int y, int amount, ResourceType resourceType);
    virtual ~Resource() = default;

    // Getter
    int GetGridX() const { return gridX; }
    int GetGridY() const { return gridY; }
    int GetCurrentAmount() const { return currentAmount; }
    int GetMaxAmount() const { return maxAmount; }
    ResourceType GetType() const { return type; }
    bool IsOccupied() const { return isOccupied; }
    bool IsDepleted() const { return currentAmount <= 0; }

    // Setter
    void SetOccupied(bool occupied) { isOccupied = occupied; }

    virtual int Mine(int amount);
    virtual float GetMiningSpeed() const { return 1.0f; }
    virtual std::string GetName() const = 0;

    virtual void Draw(int viewIndex, const GridMap* map) const;
};