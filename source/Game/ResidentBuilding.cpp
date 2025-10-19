/**
 * \file Resident.cpp
 * \brief Dervied class of Building, manage human tiredness
 * \author Dongyun Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "ResidentBuilding.hpp"

ResidentBuilding::ResidentBuilding(vec2 pos, int w, int h) : Building(pos, w, h), satisfaction(1.0f), taxrate(10)
{
    max_capacity     = 50;
    current_capacity = 0;
    cost             = 100;
}

ResidentBuilding::~ResidentBuilding()
{
    // TODO
}

void ResidentBuilding::Update([[maybe_unused]]float dt)
{
    // TODO
}

void ResidentBuilding::Draw()
{
    // TODO
}

void ResidentBuilding::AddResident(int count)
{
    current_capacity += count;
    if (current_capacity > max_capacity)
    {
        current_capacity = max_capacity;
    }
}

void ResidentBuilding::RemoveResident(int count)
{
    if(count > static_cast<int>(current_capacity))
    {
        current_capacity = 0;
    }
    else
    {
        current_capacity -= count;
    }
}

int ResidentBuilding::GetTaxIncome() const
{
    return static_cast<int>(current_capacity * taxrate / 100);
}

float ResidentBuilding::GetSatisfaction() const
{
    return satisfaction;
}

bool ResidentBuilding::IsFull() const
{
    return current_capacity >= max_capacity;
}