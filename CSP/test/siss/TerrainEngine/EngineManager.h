///////////////////////////////////////////////////////////////////////////
//
//   test of a terrain-engine based on GeoMipMapping
//
//   coded by zzed and Stormbringer
//   email:   zzed@gmx.li    storm_bringer@gmx.li 
//
//   7.2.2001
//
///////////////////////////////////////////////////////////////////////////

#if !defined(ENGINEMANAGER_H)
#define ENGINEMANAGER_H


#include <gl/glut.h>

#include "../../../src/siss/TerrainEngine/MipMapRenderer.h"
#include "../../../src/siss/TerrainEngine/TerrainData.h"	


class EngineManager  
{
public:
	float m_vRotation[3];
	static void OnMouseMotion(int x, int y);
	static void OnMouse(int button, int state, int x, int y);
	static void DrawWindow();
	static void OnIdle();
	EngineManager();
	virtual ~EngineManager();
	void StartEngine(int argc, char **argv);
	static void OnKeyboard(unsigned char key, int x, int y);
private:
	float m_fSpeed;
	void InitGL();
	void InitGLUT(int argc, char **argv);
	CMipMapRenderer *m_renderer;
//	SimpleHeightmapRenderer *m_renderer;
	CTerrainData				*m_pTerrainData;
	int						m_mousestate;
	int						m_lastmouseposx;
	int						m_lastmouseposy;
	float					m_posx;
	float 					m_posy;
	float					m_posz;
	float					m_rotx;
	float					m_roty;
	int 					m_iShowStats;
	bool					m_bLines;
};

#endif 
