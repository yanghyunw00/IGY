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

    // Static texture cache to prevent memory leaks
    static Dokev::Texture2D homeTexture;
    static Dokev::Texture2D missingTexture;
    static bool texturesLoaded;

    static void LoadTextures();

public:
    ResidentBuilding(Math::fvec2 pos, int w, int h, BuildingType bt);
    ~ResidentBuilding() override;

    void Update(float dt) override;
    void Draw(int viewIndex, const GridMap* map) override;

    std::string TypeName() override {return "ResidentBuilding";}

    // Residence function
    void AddResident(int count);
    void RemoveResident(int count);
    int GetTaxIncome() const;
    float GetSatisfaction() const;
    bool IsFull() const;


};
