///////////////////////////////////////////////////////////////////////////
//
//   Falcon 4.0 Terrain Disk Quad Class
//	 for the CSP Terrain Editor
//
//   coded by Stormbringer
//   email: storm_bringer@gmx.li 
//
//   14.04.2001
//
///////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "F4Definitions.h"
#include "F4DiskQuad.h"

void F4ConvertDiskQuad(F4MemQuad *MemQuad, F4DiskQuad *DiskQuad, int iLOD)
{
	int i;

	for(i=0; i<POSTS_PER_BLOCK; i++)
	{
		MemQuad->fz = -(float)(DiskQuad->iz);
		MemQuad->sTextureID = DiskQuad->uiTextureID;
		MemQuad++;
		DiskQuad++;
	}
	// iLOD is not used yet
}