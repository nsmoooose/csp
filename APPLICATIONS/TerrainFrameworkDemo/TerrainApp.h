#include <Framework.h>

#include "BaseApp.h"
#include "SISS/TerrainEngine/TerrainEngine.h"
#include "OGLText.h"
#include "SISS/TerrainEngine/Vector.h"


class TerrainApp : public BaseApp
{

public:
	TerrainApp();
	virtual ~TerrainApp();



  virtual short Initialize(_APP_INIT_STRUCT *Init);
  virtual void  Uninitialize();

  
  void  OnRender();
  void  OnMovement(_KEY_UNBUFFERED *key, _MOUSE *mouse, _JOYSTICK *joy);
  void  OnNetwork();
  void  On2D(char*InputLine);
  void  OnGUI(_KEY_BUFFERED *key, _MOUSE *mouse, _JOYSTICK *joy, _GUI * GUI);

private:
	float m_fSpeed;
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

	float m_vRotation[3];













};