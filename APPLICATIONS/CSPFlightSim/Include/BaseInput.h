#ifndef __BASEINPUT_H__
#define __BASEINPUT_H__

#include "SDL_keysym.h"
#include "BaseObject.h"


/**
 * class BaseInput - Describe me!
 *
 * @author unknown
 */
class BaseInput
{

public:

	virtual bool OnKeyDown(SDLKey key) = 0;
    virtual bool OnKeyUp(SDLKey key) = 0;
    virtual void OnMouseMove(int x, int y) = 0;
    virtual void OnMouseButtonDown(int num, int x, int y) = 0;
    virtual void OnJoystickAxisMotion(int joynum, int axis, int val) = 0;
	virtual void OnJoystickHatMotion(int joynum, int hat, int val) = 0;
    virtual void OnJoystickButtonDown(int joynum, int butnum) = 0;
    virtual void OnUpdate() const = 0;

	virtual void SetObject(BaseObject * ) = 0;

};

#endif
