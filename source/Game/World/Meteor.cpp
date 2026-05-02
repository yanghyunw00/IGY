#include "Meteor.hpp"
#include "Dokev/Core/DKUtility.hpp"
#include "Game/Entities/Building.hpp"
#include "Game/Managers/DKCityMetrics.hpp"
#include "Game/World/GridMap.hpp"
#include "Engine/Path.hpp"

void Meteor::OnStart()
{
    int cx = Dokev::GetRandomInt(0, GRID_SIZE_X - 1);
    int cy = Dokev::GetRandomInt(0, GRID_SIZE_Y - 1);

    // sprite = Dokev::AnimatedSprite(assets::locate_asset("Assets/images/meteor.spt"));
    // sprite.PlayAnimation("meteor");

    std::vector<Math::ivec2> removeRange;
    for (const auto& b : gameObjectManager->GetBuildings())
    {
        auto  pos = b->GetGrid();
        float dx  = pos.x - cx;
        float dy  = pos.y - cy;
        if (dx * dx + dy * dy <= radius * radius)
            removeRange.push_back({ static_cast<int>(pos.x), static_cast<int>(pos.y) });
    }
    for (auto& pos : removeRange)
    {
        gameObjectManager->RemoveBuilding(pos.x, pos.y);
        if (gridMap) gridMap->GetTileManager()->DeallocateTile(pos.x, pos.y);
    }

    cityMetrics->GetImmigrationManager()->Block();
}

void Meteor::OnEnd()
{
    cityMetrics->GetImmigrationManager()->Free();
}

void Meteor::Draw() const
{
   // sprite.DrawAtPosition(drawX, drawY);
}
