#ifdef D3D
#ifndef __WINDXVIEWPORT_H_
#define __WINDXVIEWPORT_H_

#include "Framework.h"


class WinDXViewport : public StandardViewport
{

private:

  friend StandardGraphics;

  StandardGraphics      *p_Graphics;
  IDirect3DDevice8      *p_Device;
  _GRAPHICS_INIT_STRUCT *p_Init;

  IDirect3DTexture8     *p_Viewport;
  IDirect3DSurface8     *p_ZBuffer;
  unsigned short         p_usWidth;
  unsigned short         p_usHeight;

  LPD3DXFONT             p_Font;

public:

  WinDXViewport();
  ~WinDXViewport();

  // functions
  short             Initialize(short left, short bottom, unsigned short Width, unsigned short Height);
  short             Initialize(StandardGraphics *Graphics,
                               unsigned short Width, unsigned short Height);
  void              Uninitialize();
  void              SetViewport();
  void*             GetSurface();

  StandardViewport* CreateViewport();

  void              BeginScene();
  void              EndScene();
  void              Flip();
  void              Clear();
  void              DrawTextF(char *buffer, _Rect rect, short LeftCenterRight, StandardColor*color );

};

#endif
#endif

