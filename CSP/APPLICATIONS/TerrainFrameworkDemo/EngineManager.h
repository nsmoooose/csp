///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   declaration of class EngineManager - it defines the interface between
//   the terrain engine itself and the input/output devices
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	6/13/2001	moved interpretation of SCN-CFG file to CSCNDataLoader - zzed
//
//
//
///////////////////////////////////////////////////////////////////////////

#if !defined(ENGINEMANAGER_H)
#define ENGINEMANAGER_H

//#include <GL/glut.h>

#pragma warning( disable : 4786 )	// disable warning about truncated characters in debug-info


#include "SISS/TerrainEngine/TerrainEngine.h"
#include "OGLText.h"
#include "SISS/TerrainEngine/Vector.h"


class EngineManager  
{
public:
	float m_vRotation[3];
	static void OnMouseMotion(int x, int y);
	static void OnMouse(int button, int state, int x, int y);
	static void DrawWindow();
	static void OnIdle();
	static void OnReshapeWindow(int width, int height);
	EngineManager();
	virtual ~EngineManager();
	void StartEngine(int argc, char **argv);
	static void OnKeyboard(unsigned char key, int x, int y);
private:
	float m_fSpeed;
	void InitGL();
	void InitGLUT(int argc, char **argv);
	//void ReadInitialValues();
	//CMipMapRenderer *m_renderer;
//	SimpleHeightmapRenderer *m_renderer;
	//CTerrainData				*m_pTerrainData;
	CTerrainEngine*			m_pTerrainEngine;
	int						m_mousestate;
	int						m_lastmouseposx;
	int						m_lastmouseposy;
	CVector					m_vPos;
	float					m_rotx;
	float					m_roty;
	int 					m_iShowStats;
	unsigned char			m_ucDrawingStyle;
	unsigned char			m_ucMapType;
	int						m_iWindowWidth;
	int						m_iWindowHeight;
	RHANDLE					m_hSplashTex;
	uint					m_hGLSplashTex;
	float					m_fSplashAngle;
	float					m_fSplashRotSpeed;
	uint					m_tLastFrame;
};

#endif 
