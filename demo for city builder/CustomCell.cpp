#include "CustomCell.h"
std::vector<CustomCell::CustomCell_Infra_Data> CustomCell::CustomCell_Infra_Datalist;
std::vector<Texture> CustomCell::CustomCellTextures;

CustomCell::CustomCell(Image import_image)
{
	Import_Image_To_CustomCell_Infra_DataList(import_image);
}

CustomCell::~CustomCell()
{
}  

void CustomCell::Import_Image_To_CustomCell_Infra_DataList(Image import_image)
{
	unsigned int import_image_width = import_image.width;
	unsigned int import_image_height = import_image.height;

	CustomCell_Infra_Data CustomCellInfra{ import_image_width,import_image_height };
	CustomCellInfra.pixels.resize(import_image_width * import_image_height); //reserve memory for the pixel array to avoid multiple allocations


	for (unsigned int y = 0; y < import_image_height; y++)
	{
		for (unsigned int x = 0; x < import_image_width; x++) //done this way to avoid cache misses -_-;;
		{
			Color C = GetImageColor(import_image, x, y);

			CustomCellInfra.pixels.at(y * import_image_width + x) = C;
		}
	}
	CustomCell_Infra_Datalist.push_back(CustomCellInfra);
}

const CustomCell::CustomCell_Infra_Data& CustomCell::Get_CustomCell_Infra_Data(int index) const
{
	return CustomCell_Infra_Datalist.at(index);
}

int CustomCell::Delete_CustomCell_Infra_Data(int index)
{
	//delete the texture and 2d array at the index given as parameter
	return 0;//
}

void CustomCell::testDrawPixel_stuff()
{
	unsigned int offset = 0;
	for (const auto& custominfrastructure : CustomCell_Infra_Datalist)
	{
		offset += custominfrastructure.width;
		for(unsigned int y = 0; y < custominfrastructure.height; y++)
			{
			for (unsigned int x = 0; x < custominfrastructure.width; x++)
			{
				Color C = custominfrastructure.pixels.at(y * custominfrastructure.width + x);
				DrawPixel(x+ offset, y, C);
			}
		}

	}
}

Texture CustomCell::interpreted_ImageToCustomCellInfra(Image img)
{
	return Texture();
}
