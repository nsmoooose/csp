#ifndef __GAMESCREEN_H__
#define __GAMESCREEN_H__

#include "AirplaneObject.h"
#include "BaseScreen.h"
#include "sdl_joystick.h"
#include "TypesVector3.h"
#include "SDL_keysym.h"


class GameScreen : public BaseScreen
{
public:
    virtual void OnInit();
    virtual void OnExit();

    virtual void OnRender();
    virtual void OnUpdateObjects(float dt);


    //virtual bool OnKeyDown(char * key); the base class method is not redefined here
	virtual bool OnKeyDown(SDLKey key);
    virtual bool OnKeyUp(SDLKey key);
    virtual void OnMouseMove(int x, int y);
    virtual void OnMouseButtonDown(int num, int x, int y);
    virtual void OnJoystickAxisMotion(int joynum, int axis, int val);
	virtual void OnJoystickHatMotion(int joynum, int hat, int val);
    virtual void OnJoystickButtonDown(int joynum, int butnum);
	

protected:
	static double const angleOffset;
    int m_iViewMode;
	float m_fangleRotX, m_fangleRotZ, m_fdisToPlane, m_fangleOffsetX, m_fangleOffsetZ;
	float m_fscaleFactor;
    float missile_delay;
	StandardVector3 m_fixedCamPos;

	bool m_bInteralView;

    
	void NormalView();
	void TurnViewAboutX(float fangleMax = pi / 2);
    void TurnViewAboutZ(float fangleMax = pi);
	void ScaleView();
	void OnUpdateKeyboardControl(BaseObject * p_pPlayerPlane);
	StandardVector3 GetNewFixedCamPos(BaseObject * const p_pPlayerPlane) const;
    void SetCamera();
};

#endif