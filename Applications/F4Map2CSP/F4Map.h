///////////////////////////////////////////////////////////////////////////
//
//   Falcon 4.0 Map Reader Class
//	 for the CSP Terrain Editor
//
//   coded Stormbringer
//   email: storm_bringer@gmx.li 
//
//   11.04.2001
//
///////////////////////////////////////////////////////////////////////////

#if !defined(F4MAP_H)
#define F4MAP_H

#include "F4TerrainLevel.h"


class CF4Map
{
public:
	CF4Map();
	virtual ~CF4Map();
	bool Setup(CString SourceMapName);
	void Cleanup();
	void ShowMapData(CString& outputstring);
	float GetQuadWidthFeet()	{	return m_fQuadWidthFeet;	}
	void ConvertMAPtoRAW(CString DestMapName, int Level);

private:
	float	m_fQuadWidthFeet;								// Width of each map quad in feet
	int		m_iMEAwidth;									//
	int		m_iMEAheight;									//
	float	m_fDistToMEACell;								// Distance to MEA cell in feet
	int		m_iNumberOfLevels;								// Number of detail levels per map
	int		m_iNearTextureLODLevel;							// 
	int		m_iFarTextureLODLevel;							//
	UINT32	m_aiColorPalette[256];							// the color table, currnetly not used 
	CF4TerrainLevel *m_pTerrainLevel;						// pointer to the various LOD-levels of the map
};

#endif

