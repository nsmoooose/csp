#ifndef __GAMESCREEN_H__
#define __GAMESCREEN_H__

#include "AirplaneObject.h"
#include "BaseScreen.h"
#include "SDL_keysym.h"
#include "SDL_joystick.h"
#include "TypesVector3.h"


/**
 * class GameScreen - Describe me!
 *
 * @author unknown
 */
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

	bool m_bInternalView;
	bool m_bPreviousState;

	void NormalView();
	void TurnViewAboutX(double fangleMax = pi / 2);
    void TurnViewAboutZ(double fangleMax = pi);
	void ScaleView();
	StandardVector3 GetNewFixedCamPos(BaseObject * const p_pPlayerPlane) const;
    void SetCamera();
};

#endif
