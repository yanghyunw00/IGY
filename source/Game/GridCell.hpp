/**
 * \file GridCell.hpp
 * \brief 
 * \author Junseok Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once

enum class TerrainType
{
	Water,
	Plains,
	Mountain,
	Snow,
	None
};

struct TerrainThresholds
{
    double waterMax      = 75.0;
    double plainsMax     = 150.0;
    double mountainMax   = 205.0;
};

inline TerrainThresholds DefaultTerrainThresholds()
{
    return TerrainThresholds{};
}

// the basis for all gridMap occupiers. implement a basic is full and is destoryed fuction to be used.
struct GridCell
{
	GridCell() : Height(0.0), Type(TerrainType::None), isOccupied(false), isDestroyed(false) //should accept a normalized height value between 0 and 1 from the perlin noisemap
	{

	}
	GridCell(double height) : Height(height), Type(TerrainType::None), isOccupied(false), isDestroyed(false)//should accept a normalized height value between 0 and 1 from the perlin noisemap
	{
        ApplyTerrain(height, DefaultTerrainThresholds());
	}



	GridCell(double height, int x_param, int y_param, const TerrainThresholds& thresholds = DefaultTerrainThresholds()) : Type(TerrainType::None), isOccupied(false), isDestroyed(false)//should accept a normalized height value between 0 and 1 from the perlin noisemap
	{
        ApplyTerrain(height, thresholds);
		x = x_param;
		y = y_param;
	}

	float getMovementCost() const
	{
		switch (Type)
		{
		case TerrainType::Water:    return 999999.9f; // Water should not be walkable
		case TerrainType::Plains:   return 1.0f;
		case TerrainType::Mountain: return 1.5f;
		case TerrainType::Snow:     return 2.5f;
		default:                    return 999.9f;
		}
	}

	bool isWalkable() const
	{
		return Type != TerrainType::Water;
	}

	// normal gridcells hold information about the terrian it is situated on, if it is being used by some building or person and etc
	// public functions > getter and setter functions to be called when creating or changing the gridcell.


	void setHeight(double height) { Height = height; }
	double getHeight() const { return Height; }
	void setTemperature(double temperature) { Temperature = temperature; }
	double getTemperature() const { return Temperature; }
	void setOccupied(bool occupied) { isOccupied = occupied; }
	bool getOccupied() const { return isOccupied; }
	void setDestroyed(bool destroyed) { isDestroyed = destroyed; }
	bool getDestroyed() const { return isDestroyed; }
	void setWater(bool water) { isWater = water; }
	bool getWater() const { return isWater; }
    void ApplyTerrain(double height, const TerrainThresholds& thresholds)
    {
        Height = height;

        // Set terrain type based on height
        if (Height < thresholds.waterMax)      { Type = TerrainType::Water; isWater = true; }
        else if (Height < thresholds.plainsMax) Type = TerrainType::Plains;
        else if (Height < thresholds.mountainMax) Type = TerrainType::Mountain;
        else                    Type = TerrainType::Snow;
    }

	int x = 0, y = 0;
private:
	double Height = 0.0;
	TerrainType Type = TerrainType::None;
	double Temperature = 0.0;
	int resources = 0;

	bool isOccupied = false; // if the cell is occupied by a building or person
	bool isDestroyed = false; // if the cell is destroyed by a disaster or something else. if true, the cell cannot be used until it is repaired.
	bool isWater = false; // if the cell under water, it cannot be used for building or walking. only for boats and bridges.
};  
