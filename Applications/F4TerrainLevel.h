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

#if !defined(F4TERRAINLEVEL_H)
#define F4TERRAINLEVEL_H

class CF4TerrainBlock;
struct F4MemQuad;

typedef union uF4TerrainBlockAdress
{
	CF4TerrainBlock*	TerrainBlockPtr;
	DWORD				dwOffset;
} uF4TerrainBlockAdress;

class CF4TerrainLevel
{
public:
	CF4TerrainLevel();
	virtual ~CF4TerrainLevel();
	bool Setup(int iLevel, int iWidth, int iHeight, CString MapFileName);
	void Cleanup();
	int GetWidth()	{	return m_iBlocksWidth;	}
	int GetHeight()	{	return m_iBlocksHeight;	}
	float GetFeetPerQuad()	{	return m_fFeetPerQuad;	}
	float GetFeetPerBlock()	{	return m_fFeetPerBlock;	}
	int GetLevel()	{	return m_iLevel;	}
	void PreProcessBlock(CF4TerrainBlock* Block);
	CF4TerrainBlock *RequestBlock(int row, int column);
	void SetBlockPtr(UINT row, UINT column, CF4TerrainBlock *Block);
	CF4TerrainBlock *GetBlockPtr(UINT row, UINT column);
	
//private:
	UINT	m_iBlocksWidth;								// each block consists of a certain number of height map points, 
	UINT	m_iBlocksHeight;							// and each terrain map level of a certain number of blocks
	float	m_fFeetPerQuad;								// the real-world distance between two points in the height map
	float	m_fFeetPerBlock;							// the same for a block
	int		m_iLevel;									// the detail level of the current level
	uF4TerrainBlockAdress *m_pTerrainBlocks;			// points to an array of blocks
	int		m_iQuadFile;
};

#endif

