#if !defined(AFX_ENGINESTATUS_H__AEC30300_0905_11D5_98AF_444553540000__INCLUDED_)
#define AFX_ENGINESTATUS_H__AEC30300_0905_11D5_98AF_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EngineStatus.h : Header-Datei
//

#define		MAP_TYPE_RAW		1
#define		MAP_TYPE_FALCON4	2

#include "MipMapRenderer.h"
#include "TerrainData.h"	

class CEngineStatus
{
public:
	CEngineStatus();
	virtual ~CEngineStatus();

	char					m_sMapFileName[256];
	float m_vRotation[3];
	float m_fSpeed;
	int						m_mousestate;
	int						m_lastmouseposx;
	int						m_lastmouseposy;
	float					m_posx;
	float 					m_posy;
	float					m_posz;
	float					m_rotx;
	float					m_roty;
	int						m_iViewportHeight;
	int						m_iViewportWidth;
	int 					m_iShowStats;
	bool					m_bLines;
	bool					m_bTextures;
	float					m_fAspectRatio;
	float					m_fEpsilonNearClip;
	float					m_fEpsilonTopCoor;
	float					m_fEpsilonDeltaH;
	int						m_iEpsilonVRes;
	int						m_iQuadLength;
	int						m_iMapType;
};

#endif