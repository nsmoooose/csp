#ifndef __BASESCREEN_H__
#define __BASESCREEN_H__

#include "SDL_keysym.h"

// BaseScreen represents a generic screen
// Each screen within the Game/Sim should
// Derive a class from class. Each Screen
// class must support a OnRender() function
// that knows how to draw the screen.

class BaseScreen
{
public:
    virtual void OnInit() = 0;
    virtual void OnExit() = 0;

    virtual void OnUpdateObjects(float dt) = 0;
    virtual void OnRender() = 0;

    // Input Routines, are not required. If the are not overloaded then
    // a dummy routine will be called here.
    virtual bool OnKeyDown(char *key) { return false; }
	virtual bool OnKeyDown(SDLKey key) {return false; }
    virtual void OnKeyUp(char * key) { ; }
	virtual bool OnKeyUp(SDLKey key) { return false; }
    virtual void OnMouseMove(int x, int y) {; }
    virtual void OnMouseButtonDown(int num, int x, int y) {; }
    virtual void OnJoystickAxisMotion(int joynum, int axis, int val) {; }
	virtual void OnJoystickHatMotion(int joynum, int hat, int val) {; }
    virtual void OnJoystickButtonDown(int joynum, int butnum) {; }

};

#endif