///////////////////////////////////////////////////////////////////////////
//
//   TerrainLab  - CSP - http://csp.homeip.net
//
//   coded by Stormbringer and		zzed
//   email:   storm_bringer@gmx.li	zzed@gmx.li
//
///////////////////////////////////////////////////////////////////////////
//
//   Header file of Class CCamera  
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	09/25/2001	file created	-	Stormbringer
//
//
///////////////////////////////////////////////////////////////////////////

#if !defined(CAMERA_H)
#define CAMERA_H

#include "BaseCode.h"
#include "TerrainData.h"

extern CTerrainData *m_pTerrainData;

class CCamera
{
public:
	CCamera();
	~CCamera();
	ResetCamera();

	float	m_fPosX;
	float	m_fPosY;
	float	m_fPosZ;
	float	m_fRotX;
	float	m_fRotY;
	float	m_fRotZ;
	float	m_fSpeed;
	float	m_vRotation[3];
};

#endif