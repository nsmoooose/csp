#ifndef __LOGOSCREEN_H__
#define __LOGOSCREEN_H__

#include "SDL.h"
#include "SDL_image.h"

#include "BaseScreen.h"

class LogoScreen : public BaseScreen
{
public:
    virtual void OnInit();
    virtual void OnExit();

    virtual void OnRender();
    virtual void OnUpdateObjects(float dt)  {;  }


private:
     SDL_Surface * m_image;

};

#endif