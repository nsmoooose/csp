///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   declaration of CSector
//   class represents a square group of quads for efficient memory usage
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//  10/10/2001	created initial version - zzed
//
//
///////////////////////////////////////////////////////////////////////////

#if !defined(SECTOR_H)
#define SECTOR_H

#include "MipMapQuad.h"
#include "TerrainData.h"


class CSector
{
public:
	CQuad	**m_pQuadList;
	// position of this sector in "triangles"
	uint	m_uiXPos;	
	uint	m_uiYPos;

	CSector(uint uiXPos, uint uiYPos, CTerrainData *pData);
	virtual ~CSector();

	CQuad operator[](uint uiElement);
	void AddElement(CQuad* pQuad, uint uiPosition);
	void RemoveElement(uint uiPosition);
	// update references between quads in different sectors; if pNeighbor is 0, default values will be set (no crack resolution)
	void UpdateQuadRefs(uint direction, CSector *pNeighbor);
private:
	uint	m_uiSize;

	CTerrainData	*m_pData;

	void CreateQuads();
};


#endif
