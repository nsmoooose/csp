#ifndef __TYPESVIEWPORT_H__
#define __TYPESVIEWPORT_H__

#include "Framework.h"

class StandardViewport
{

private:


public:
	StandardViewport() {}; 
  virtual ~StandardViewport() {};

  // functions
  virtual short             Initialize(short left, short bottom, unsigned short Width, unsigned short Height) = 0;
  virtual short             Initialize(StandardGraphics *Graphics,
                                       unsigned short Width, unsigned short Height) = 0;
  virtual void              Uninitialize() = 0;

  virtual void              SetViewport() = 0;
  virtual _SURFACE          GetSurface() = 0;

  virtual StandardViewport* CreateViewport() = 0;

  virtual void              BeginScene() = 0;
  virtual void              EndScene() = 0;
  virtual void              Flip() = 0;
  virtual void              Clear() = 0;
  virtual void              DrawTextF(char *buffer, _Rect rect, short LeftCenterRight, StandardColor*color ) = 0;


};

#endif
