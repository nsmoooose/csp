///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   definition of CSector
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

#include "Sector.h"

#include <assert.h>


CSector::CSector(uint uiXPos, uint uiYPos, CTerrainData *pData)
{
	m_pData = pData;
	m_uiSize = m_pData->m_uiSectorWidth*m_pData->m_uiSectorWidth;
	m_uiXPos = uiXPos;
	m_uiYPos = uiYPos;
	// initialize quadlist
	m_pQuadList = new CQuad*[m_uiSize];
	for (uint i=0; i<m_uiSize; ++i) m_pQuadList[i] = 0;

	CreateQuads();
}

CSector::~CSector()
{
	for (uint i=0; i<m_uiSize; ++i) if (m_pQuadList[i]) delete m_pQuadList[i];
	delete[] m_pQuadList;
}

CQuad CSector::operator[](uint uiElement)
{
	assert(uiElement>m_uiSize && "CSector::operator[]: requested element out of range");
	return *(m_pQuadList[uiElement]);
}

void CSector::AddElement(CQuad* pQuad, uint uiPosition)
{
	if (!m_pQuadList[uiPosition]) delete m_pQuadList[uiPosition];
	m_pQuadList[uiPosition] = pQuad;
}

void CSector::RemoveElement(uint uiPosition)
{
	if (!m_pQuadList[uiPosition]) delete m_pQuadList[uiPosition];
	m_pQuadList[uiPosition] = 0;
}

void CSector::CreateQuads()
{
	uint y;
	for (y=0; y<m_pData->m_uiSectorWidth; ++y)
		for (uint x=0; x<m_pData->m_uiSectorWidth; ++x)
			m_pQuadList[y*m_pData->m_uiSectorWidth+x] = new CQuad(m_uiXPos+x*m_pData->m_uiQuadWidth, m_uiYPos+y*m_pData->m_uiQuadWidth, m_pData);

	// create references between quads
	for (y=0; y<m_pData->m_uiSectorWidth; ++y)
		for (uint x=0; x<m_pData->m_uiSectorWidth; ++x)
		{
			uint index = y*m_pData->m_uiSectorWidth+x;
			CQuad *quad = m_pQuadList[index];
			if (x>0) quad->m_pNeighbors[0] = &m_pQuadList[index-1]->m_uiCurSteps;  // left
			if (y>0) quad->m_pNeighbors[1] = &m_pQuadList[index-m_pData->m_uiSectorWidth]->m_uiCurSteps;  // top
			if (x<m_pData->m_uiSectorWidth-1) quad->m_pNeighbors[2] = &m_pQuadList[index+1]->m_uiCurSteps;  // right
			if (y<m_pData->m_uiSectorWidth-1) quad->m_pNeighbors[3] = &m_pQuadList[index+m_pData->m_uiSectorWidth]->m_uiCurSteps;  // bottom
		}
}

void CSector::UpdateQuadRefs(uint direction, CSector *pNeighbor)
{
	uint	lindex;	// "local" index
	uint	rindex;	// "remote" index
	int		idelta;	// index-delta

	switch (direction)
	{
	case 0:	// left
		lindex = 0;
		rindex = m_pData->m_uiSectorWidth-1;
		idelta = m_pData->m_uiSectorWidth;
		break;
	case 1: // top
		lindex = 0;
		rindex = m_pData->m_uiSectorWidth*(m_pData->m_uiSectorWidth-1);
		idelta = 1;
		break;
	case 2: // right
		lindex = m_pData->m_uiSectorWidth-1;
		rindex = 0;
		idelta = m_pData->m_uiSectorWidth;
		break;
	case 3: // bottom
		lindex = m_pData->m_uiSectorWidth*(m_pData->m_uiSectorWidth-1);
		rindex = 0;
		idelta = 1;
	}
	if (pNeighbor)
		for (uint i=0; i<m_pData->m_uiSectorWidth; ++i)
		{
			m_pQuadList[lindex]->m_pNeighbors[direction] = &pNeighbor->m_pQuadList[rindex]->m_uiCurSteps;
			lindex += idelta; rindex += idelta;
		}
	else
		for (uint i=0; i<m_pData->m_uiSectorWidth; ++i)
		{
			m_pQuadList[lindex]->m_pNeighbors[direction] = &m_pQuadList[lindex]->m_uiMaxSteps;
			lindex += idelta; rindex += idelta;
		}
}
