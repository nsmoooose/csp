///////////////////////////////////////////////////////////////////////////
//
//   Falcon 4.0 Mem Quad Class
//	 for the CSP Terrain Editor
//
//   coded by Stormbringer
//   email: storm_bringer@gmx.li 
//
//   15.04.2001
//
///////////////////////////////////////////////////////////////////////////

#if !defined(F4MEMQUAD_H)
#define F4MEMQUAD_H

#include "F4definitions.h"

typedef struct F4MemQuad
{
	float				fz;						// Z-Coordinate
	int					iColorIndex;
	float				fu;						// u = East/West Coordinate
	float				fv;						// v = North/South Coordinate
	float				fd;
	short				sTextureID;
	unsigned char		uiTheta;
	unsigned char		uiPhi;
} F4MemQuad;

#endif