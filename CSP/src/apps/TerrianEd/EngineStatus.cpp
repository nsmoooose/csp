#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "stdafx.h"
#include "EngineStatus.h"


/////////////////////////////////////////////////////////////////////////////
// CEngineStatus

CEngineStatus::CEngineStatus()
{
	m_mousestate = 0;
	m_posx = 12.0f;
	m_posy = 12.0f;
	m_posz = 18.0f;
	m_rotx = 0;
	m_roty = 0;
	m_fSpeed = 100.0f;
	m_lastmouseposx = 0;
	m_lastmouseposy = 0;
	m_vRotation[0] = 0;
	m_vRotation[1] = 0;
	m_vRotation[2] = 0;
	m_bLines = true;
	m_bTextures = 0;
	m_fEpsilonNearClip = 0.1f;
	m_fEpsilonTopCoor = 0.08f;
	m_fEpsilonDeltaH = 1.0f;
	m_iEpsilonVRes = 480;
	m_iQuadLength = 64;
}

CEngineStatus::~CEngineStatus()
{
}
