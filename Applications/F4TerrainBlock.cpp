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

#include "stdafx.h"
#include "F4TerrainBlock.h"

CF4TerrainBlock::CF4TerrainBlock()
{
}

CF4TerrainBlock::~CF4TerrainBlock()
{
}

void CF4TerrainBlock::Setup(CF4TerrainLevel *Level, UINT row, UINT col)
{
	m_pTerrainLevel = Level;
	m_uiBlockRow	= row;
	m_uiBlockCol	= col;
	m_pMemQuads		= NULL;					// Pointer will be set by F4TerrainLevel::PreProcessBlock()
	m_dwFileOffset	= 0;
}

void CF4TerrainBlock::Cleanup()
{
	if(m_pMemQuads)
	   delete m_pMemQuads;
	m_pMemQuads = NULL;
	m_pTerrainLevel = NULL;
}