// HeightMap.cpp: Implementierung der Klasse HeightMap.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HeightMap.h"


//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HeightMap::HeightMap(char* map_filename, int i_width, int i_height)
{
	FILE *pFile;
	int success;
	unsigned char *buffer;

	// Load TerraGen raw data and save to internal structure
	height = i_height;
	width = i_width;

	pFile = fopen(map_filename, "r");
	if(!pFile)
	{	
		printf("unable to open '%s'!\n", map_filename);
		char text[100];
		gets(text);
		exit(1);
	}

	data = new float[height*width];
	buffer = new unsigned char[height*width];

	printf("Loading TerraGen RAW map\n");
	success = fread(buffer, (height*width*sizeof(char)), 1, pFile);

	// convert RAW 8-bit values to float
	for (int y=0; y<height; y++)					
		for (int x=0; x<width; x++)
		{
			data[y*width+x] = 
				(float)(((unsigned char *)buffer)[y*width+x]);
		} 

	fclose(pFile);
	delete []buffer;
	
	//data = (byte*)lpbitmap->bmBits;
}

HeightMap::~HeightMap()
{
	delete []data;
}


HeightMap::HeightMap()
{

}
