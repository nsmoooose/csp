///////////////////////////////////////////////////////////////////////////
//
//   Falcon 4.0 Disk Quad Class
//	 for the CSP Terrain Editor
//
//   coded by Stormbringer
//   email: storm_bringer@gmx.li 
//
//   14.04.2001
//
///////////////////////////////////////////////////////////////////////////

#if !defined(F4DISKQUAD_H)
#define F4DISKQUAD_H

#include "F4MemQuad.h"

#pragma pack (push, 1)

typedef struct F4DiskQuad
{
	unsigned short		uiTextureID;
	short				iz;						// Z Coordinate
	unsigned char		uiColor;
	unsigned char		uiTheta;
	unsigned char		uiPhi;
} F4DiskQuad;
#pragma pack (pop)

//void F4ConvertDiskQuad(F4MemQuad *MemQuad, F4DiskQuad *DiskQuad, int iLOD, float fLightLevel,
//					float *fMinZ, float *fMaxZ);
void F4ConvertDiskQuad(F4MemQuad *MemQuad, F4DiskQuad *DiskQuad, int iLOD);

#endif