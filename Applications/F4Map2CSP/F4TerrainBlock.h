///////////////////////////////////////////////////////////////////////////
//
//   Falcon 4.0 Terrain Block Class
//	 for the CSP Terrain Editor
//
//   coded by Stormbringer
//   email: storm_bringer@gmx.li 
//
//   14.04.2001
//
///////////////////////////////////////////////////////////////////////////

#if !defined(F4TERRAINBLOCK_H)
#define F4TERRAINBLOCK_H

#include "F4MemQuad.h"
#include "F4TerrainLevel.h"

class CF4TerrainBlock
{
public:
	CF4TerrainBlock();
	virtual ~CF4TerrainBlock();
	void Setup(CF4TerrainLevel *Level, UINT row, UINT col);
	void Cleanup();
	unsigned int	BlockGetRow()	{	return m_uiBlockRow;	}
	unsigned int	BlockGetCol()	{	return m_uiBlockCol;	}
	F4MemQuad*		BlockGetSWCorner()	{	return m_pMemQuads;		}
	DWORD			BlockGetOffset()	{	return m_dwFileOffset;	}
	void			BlockSetOffset(DWORD offset)	{	m_dwFileOffset = offset;	}
	F4MemQuad*		GetQuad(UINT uiRow, UINT uiCol)	
					{	
						return m_pMemQuads+(uiRow * POSTS_ACROSS_BLOCK + uiCol);
					}

	F4MemQuad *m_pMemQuads;		
	
protected:
	UINT	m_uiBlockRow;									// Block coordinates within level
	UINT	m_uiBlockCol;
	DWORD	m_dwFileOffset;									// Offset of this block's data in disk file
	CF4TerrainLevel *m_pTerrainLevel;

};

#endif

