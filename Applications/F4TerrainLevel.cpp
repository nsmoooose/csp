///////////////////////////////////////////////////////////////////////////
//
//   Falcon 4.0 Terrain Level Class
//	 for the CSP Terrain Editor
//
//   coded by Stormbringer
//   email: storm_bringer@gmx.li 
//
//   11.04.2001
//
///////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "assert.h"
#include "io.h"
#include "fcntl.h""
#include "F4TerrainLevel.h"
#include "F4TerrainBlock.h"
#include "F4Definitions.h"
#include "F4Map.h"
#include "F4MemQuad.h"
#include "F4DiskQuad.h"

extern CF4Map *m_pF4Map;
static F4DiskQuad SharedQuadIOBuffer[POSTS_PER_BLOCK];

CF4TerrainLevel::CF4TerrainLevel()
{
}

CF4TerrainLevel::~CF4TerrainLevel()
{
}


bool CF4TerrainLevel::Setup(int iLevel, int iWidth, int iHeight, CString MapFileName)
{
	UINT i, bytesread, OffsetFile = NULL;
	CString FileName, Extension;

	m_iBlocksWidth = iWidth;
	m_iBlocksHeight = iHeight;
	m_iLevel = iLevel;
	m_fFeetPerQuad = LEVEL_POST_TO_WORLD(1, m_iLevel);
	m_fFeetPerBlock = LEVEL_BLOCK_TO_WORLD(1, m_iLevel);

	//Allocate memory for the block pointer array
	m_pTerrainBlocks = new uF4TerrainBlockAdress[m_iBlocksWidth*m_iBlocksHeight];
	assert(m_pTerrainBlocks);

	//Now set the correct file name, then open the offset file
	FileName = MapFileName;
	FileName.Delete(FileName.GetLength() - 4, 4);							// delete the extension of FileName
	Extension.Format(".o%0d", iLevel);										// and add the one for the offset file
	FileName += Extension;

	OffsetFile = open((LPCTSTR)FileName, O_BINARY|O_RDONLY, 0);				// read the offset data to the block pointer array
	if(OffsetFile)
	{	
		bytesread = read(OffsetFile, m_pTerrainBlocks, sizeof(CF4TerrainBlock*) * m_iBlocksWidth*m_iBlocksHeight);
		assert(bytesread == sizeof(CF4TerrainBlock*) * m_iBlocksWidth*m_iBlocksHeight);
		close(OffsetFile);
	}

	// walk through the offsets and shift them up to clear the low bit - but don't ask me why :-)
	for(i=0; i<m_iBlocksWidth*m_iBlocksHeight; i++)
		m_pTerrainBlocks[i].dwOffset = (m_pTerrainBlocks[i].dwOffset << 1) | 1;

	// finally, we open the quad (heightmap) file for this level
	FileName = MapFileName;
	FileName.Delete(FileName.GetLength() - 4, 4);							// delete the extension of FileName
	Extension.Format(".l%0d", iLevel);										// and add the one for the quad file
	FileName += Extension;

	m_iQuadFile = open((LPCTSTR)FileName, O_BINARY|O_RDONLY, 0);	
	assert(m_iQuadFile);

	return TRUE;
}

void CF4TerrainLevel::Cleanup()
{
	close(m_iQuadFile);
	delete[] m_pTerrainBlocks;
	return;
}

void CF4TerrainLevel::PreProcessBlock(CF4TerrainBlock* Block)
{
	DWORD		success;

	// Seek & Read the block data from disk. (the >> 1 is properly done in RequestBlock) 
	success = lseek(m_iQuadFile, Block->BlockGetOffset() >> 1, SEEK_SET);
	assert(success == Block->BlockGetOffset() >> 1);

	success = read(m_iQuadFile, SharedQuadIOBuffer, sizeof(SharedQuadIOBuffer));
	assert(success == sizeof(SharedQuadIOBuffer));

	// Alloc mem for quad data, and decompress it
	Block->m_pMemQuads = new F4MemQuad[POSTS_PER_BLOCK];
	assert(Block->m_pMemQuads);

	F4ConvertDiskQuad(Block->m_pMemQuads, SharedQuadIOBuffer, m_iLevel);

	return;
}

CF4TerrainBlock *CF4TerrainLevel::RequestBlock(int row, int column)
{
	CF4TerrainBlock *Block;

	Block = new CF4TerrainBlock();
	assert(Block);
	Block->Setup(this, row, column);			// !!!!!!!!!!!!!!!!!!!!!!!
	SetBlockPtr(row, column, Block);			// Put block header into block array
	PreProcessBlock(Block);
	return Block;
}

void CF4TerrainLevel::SetBlockPtr(UINT row, UINT column, CF4TerrainBlock *Block)
{
	if(Block)
	{
		// Store offset from which this block was retrieved
		Block->BlockSetOffset(m_pTerrainBlocks[row * m_iBlocksWidth + column].dwOffset);

		// Replace this offset with a memory pointer
		m_pTerrainBlocks[row * m_iBlocksWidth + column].TerrainBlockPtr = Block;
	}
	else
	{
		// the same thing in the other direction
		Block = m_pTerrainBlocks[row * m_iBlocksWidth + column].TerrainBlockPtr;		// Get block adress
		m_pTerrainBlocks[row * m_iBlocksWidth + column].dwOffset = Block->BlockGetOffset();
	}
}

CF4TerrainBlock *CF4TerrainLevel::GetBlockPtr(UINT row, UINT column)
{
	uF4TerrainBlockAdress block;

	block = m_pTerrainBlocks[row * m_iBlocksWidth + column];
	return block.TerrainBlockPtr;
}