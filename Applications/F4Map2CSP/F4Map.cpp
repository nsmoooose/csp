///////////////////////////////////////////////////////////////////////////
//
//   Falcon 4.0 Map Reader Class
//	 for the CSP Terrain Editor
//
//   coded by Stormbringer
//   email: storm_bringer@gmx.li 
//
//   11.04.2001
//
///////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include <io.h>
#include "assert.h"
#include <fcntl.h>
#include "F4map.h"
#include "F4MemQuad.h"
#include "F4TerrainBlock.h"

CF4Map::CF4Map()
{
}

CF4Map::~CF4Map()
{
}


bool CF4Map::Setup(CString SourceMapName)
{
	FILE *pFile;
	int i, success, height, width;

	pFile = fopen((LPCTSTR)SourceMapName, "r");
	assert(pFile);

	success = fread(&m_fQuadWidthFeet, sizeof(float), 1, pFile);
	assert(success);
	success = fread(&m_iMEAwidth, sizeof(int), 1, pFile);
	assert(success);
	success = fread(&m_iMEAheight, sizeof(int), 1, pFile);
	assert(success);
	success = fread(&m_fDistToMEACell, sizeof(float), 1, pFile);
	assert(success);
	success = fread(&m_iNumberOfLevels, sizeof(int), 1, pFile);
	assert(success);
	success = fread(&m_iNearTextureLODLevel, sizeof(int), 1, pFile);
	assert(success);
	success = fread(&m_iFarTextureLODLevel, sizeof(int), 1, pFile);
	assert(success);

	//We load the color palette here, but we won't use the data at the moment
	success = fread(&m_aiColorPalette, sizeof(m_aiColorPalette), 1, pFile);
	assert(success);

	//Now we have to allocate mem for the different LOD-levels of the map.
	//However, we'll only use the Level 0 map. Nobody wants to copy the F4 terrain engine ;-)
	m_pTerrainLevel = new CF4TerrainLevel[m_iNumberOfLevels];
	assert(m_pTerrainLevel);

	//We read level information for all 6 levels to keep the file offset correct
	for(i=0;i<m_iNumberOfLevels; i++)
	{
		success = fread(&width, sizeof(int), 1, pFile);
		success = fread(&height, sizeof(int), 1, pFile);
		assert((width < 9999) && (height < 9999));							//such values are never reached
		m_pTerrainLevel[i].Setup(i, width, height, SourceMapName);
	}

	fclose(pFile);

	return TRUE;
}

void CF4Map::Cleanup()
{
	int i;

	//We have to clean up each level before we release its memory
	for(i=0;i<m_iNumberOfLevels; i++)
		m_pTerrainLevel[i].Cleanup();

	if(m_pTerrainLevel)
	   delete[] m_pTerrainLevel;
	
	return;
}

void CF4Map::ShowMapData(CString& outputstring)
{
	const char linefeed[]={ 13, 10, 0 };
	CString bf;
	int i;

	bf.Format("Converting Falcon 4.0 Terrain Map...");
	outputstring += bf;
	outputstring += linefeed;
	// i think this is not the most elegant way to force the cursor to a new line, but
	// the only one that worked for me. "\n" isn't interpreted correctly, (while "\t" works allright)
	// so i had to add the byte sequence for "carriage return + line feed" manually.
	bf.Format("m_fQuadWidthFeet: %.2f", m_fQuadWidthFeet);
	outputstring += bf;
	outputstring += linefeed;
	bf.Format("m_iMEAwidth: %i", m_iMEAwidth);
	outputstring += bf;
	outputstring += linefeed;
	bf.Format("m_iMEAheight: %i", m_iMEAheight);
	outputstring += bf;
	outputstring += linefeed;
	bf.Format("m_fDistToMEACell: %.2f", m_fDistToMEACell);
	outputstring += bf;
	outputstring += linefeed;
	bf.Format("m_iNumberOfLevels: %i", m_iNumberOfLevels);
	outputstring += bf;
	outputstring += linefeed;
	bf.Format("m_iNearTextureLODLevel: %i", m_iNearTextureLODLevel);
	outputstring += bf;
	outputstring += linefeed;
	bf.Format("m_iFarTextureLODLevel: %i", m_iFarTextureLODLevel);
	outputstring += bf;
	outputstring += linefeed;

	for(i=0; i<m_iNumberOfLevels; i++)
	{
		bf.Format("Level[%i] Blocks Width: %i, Height: %i", i, m_pTerrainLevel[i].GetWidth(),
															m_pTerrainLevel[i].GetHeight());
		outputstring += bf;
		outputstring += linefeed;
		bf.Format("Feet/Block: %.2f, Feet/Quad: %.2f", m_pTerrainLevel[i].GetFeetPerBlock(),
													   m_pTerrainLevel[i].GetFeetPerQuad());
		outputstring += bf;
		outputstring += linefeed;
	}

	return;
}

void CF4Map::ConvertMAPtoRAW(CString DestMapName, int Level)
{
	long file;
	UINT success, numberofquads, block_x, block_y;
	UINT quadsinrow, quadsinblockrow, quad_x, quad_y, offset;
	short *array;
	CF4TerrainBlock *block;
	F4MemQuad *quad;

	quadsinblockrow = (int)sqrt((double)POSTS_PER_BLOCK);

	// calculate the size of the heightmap. 
	numberofquads = m_pTerrainLevel[Level].m_iBlocksWidth *
					m_pTerrainLevel[Level].m_iBlocksHeight * 
					POSTS_PER_BLOCK;

	// add one to each row and column so that it can be rendered by the mipmap-algorithm.
	quadsinrow = (int)sqrt((double)numberofquads);
	numberofquads += 2 * quadsinrow + 1; 

	// now alloc the mem for the heightmap
	array = new short[numberofquads];
	assert(array);

	quad = new F4MemQuad;
	assert(quad);

	// now load all the map blocks, and save their height-information in our heightmap
	for(block_y=0; block_y<m_pTerrainLevel[Level].m_iBlocksHeight; block_y++)
	{
		for(block_x=0; block_x<m_pTerrainLevel[Level].m_iBlocksWidth; block_x++)
		{
			m_pTerrainLevel[Level].RequestBlock(block_y, block_x);
			block = m_pTerrainLevel[Level].GetBlockPtr(block_y, block_x); 

			// read the quad height data values from each block
			for(quad_y=0; quad_y<quadsinblockrow; quad_y++)
			{
				for(quad_x=0; quad_x<quadsinblockrow; quad_x++)
				{
					quad = block->GetQuad(quad_y, quad_x);
					offset = block_y * (quadsinrow + 1) * quadsinblockrow+
							 quad_y * (quadsinrow + 1) +
							 block_x * quadsinblockrow + 
							 quad_x;
					array[offset] = -(short)(quad->fz);
				}
			}
		}
	}

	quadsinrow++;

	for(quad_x=0; quad_x<quadsinrow; quad_x++)
	{
		array[quadsinrow * (quadsinrow - 1) + quad_x] = 
		array[quadsinrow * (quadsinrow - 2) + quad_x];
	}

	for(quad_y=0; quad_y<quadsinrow; quad_y++)
	{
		array[quad_y * quadsinrow + quadsinrow - 1] = 
		array[quad_y * quadsinrow + quadsinrow - 2];
	}

	file = _open((LPCTSTR)DestMapName, _O_CREAT | _O_RDWR | _O_BINARY);
	if(file==-1)
	{	
		MessageBox(NULL, "unable to open terrain file", NULL, MB_OK);
		char text[100];
		gets(text);
		return;
	}

	success = _write(file, array, sizeof(short)*numberofquads);
	assert(success);
	_close(file);

//	if(quad)  delete quad;
	if(array) delete []array;
	
	return;
}