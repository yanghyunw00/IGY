/**
 * \file Drought.cpp
 * \brief
 * \author
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "Drought.hpp"
#include "Game/Entities/WorkBuilding.hpp"

void Drought::OnStart()
{
    for (auto& b : gameObjectManager->GetBuildings())
        if (auto* wb = dynamic_cast<WorkBuilding*>(b.get()))
            wb->SetProductionRate(productionPenalty);

    cityMetrics->SetHappinessChangeRate(0.6f);
    cityMetrics->GetImmigrationManager()->Block();
}

void Drought::OnTick(double dt)
{
    elapsed += dt;
}

void Drought::OnEnd()
{
    for (auto& b : gameObjectManager->GetBuildings())
        if (auto* wb = dynamic_cast<WorkBuilding*>(b.get()))
            wb->SetProductionRate(1.0f);

    cityMetrics->SetHappinessChangeRate(1.0f);
    cityMetrics->GetImmigrationManager()->Free();
}

bool Drought::IsFinished() const
{
    return elapsed >= maxDuration;
}

void Drought::Draw(int index) const
{
    (void)index;
}
