/**
 * \file CustomCell.hpp
 * \brief
 * \author Junseok Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include <vector>
#include "Dokev/Graphics/DKTexture.hpp"
#include "Dokev/Graphics/DKShapes.hpp"
#include "CS200/RGBA.hpp"

using namespace Dokev;
// has an array of 2d arrays showing representations of custom cell buildings

class CustomCell
{
	struct CustomCell_Infra_Data
	{
		//each infrastructure has a width and height and a 2d array of colors
		CustomCell_Infra_Data(unsigned int width, unsigned int height) : width(width), height(height) {}
		unsigned int width;
		unsigned int height;
		//2d vector
		std::vector<CS200::RGBA> pixels;

	};
public:
		CustomCell(Image import_image );
		CustomCell(); //default constructor
		~CustomCell();

		void Import_Image_To_CustomCell_Infra_DataList(Image import_image);

		//returns a 2d array of colors
		const CustomCell_Infra_Data& Get_CustomCell_Infra_Data(int index) const;

		size_t Get_Count_Of_CustomCell_Infra_Datalist() { return CustomCell_Infra_Datalist.size(); }
		int Delete_CustomCell_Infra_Data([[maybe_unused]] int index);
		
		void testDrawPixel_stuff();
		Texture2D interpreted_ImageToCustomCellInfra([[maybe_unused]] Image img);
private:
	
		Texture2D texture;
		int value;
		// vector of different infrastructures
		static std::vector<CustomCell_Infra_Data> CustomCell_Infra_Datalist;

		static std::vector<Texture2D> CustomCellTextures;
		// vector of 2d arrays representing different buildings
};