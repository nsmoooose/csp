/////////////////////////////////////////////////
// Camera.cpp
/////////////////////////////////////////////////

#include "BaseCode.h"
#include "Camera.h"
///////////////////////////////////////////////////////////////////////////
//
//   TerrainLab  - CSP - http://csp.homeip.net
//
//   coded by Stormbringer and		zzed
//   email:   storm_bringer@gmx.li	zzed@gmx.li
//
///////////////////////////////////////////////////////////////////////////
//
//   Implementation of Class CCamera  
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	09/25/2001	file created	-	Stormbringer
//
//
///////////////////////////////////////////////////////////////////////////

CCamera::CCamera()
{
	m_fPosX = 2500;
	m_fPosY = 0;
	m_fPosZ = 1500;
	m_fRotX = 0;
	m_fRotY = 0;
	m_fRotZ = 0;
	m_fSpeed = 0;
	m_vRotation[0] = 0;
	m_vRotation[1] = 0;
	m_vRotation[2] = 0;
}

CCamera::~CCamera()
{
}

CCamera::ResetCamera()
{
	m_fPosX = (float)m_pTerrainData->m_iMeshWidth*m_pTerrainData->m_iMeshGridDistance / 2;
	m_fPosY = -1000;
	m_fPosZ = 1000;
	m_fRotX = 0;
	m_fRotY = 0;
	m_fRotZ = 0;
	m_fSpeed = 0;
	m_vRotation[0] = 0;
	m_vRotation[1] = 0;
	m_vRotation[2] = 0;
}

