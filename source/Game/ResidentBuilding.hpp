/**
 * \file Resident.hpp
 * \brief Dervied class of Building, manage human tiredness and tax income
 * \author Dongyun Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include "Building.hpp"

class ResidentBuilding : public Building
{
protected:
    float satisfaction;
    float taxrate;

public:
    ResidentBuilding(vec2 pos, int w, int h);
    ~ResidentBuilding() override;

    void Update([[maybe_unused]] float dt) override;
    void Draw() override;

    // Residence function
    void AddResident(int count);
    void RemoveResident(int count);
    int GetTaxIncome() const;
    float GetSatisfaction() const;
    bool IsFull() const;
};