#ifndef __GLVIEWPORT_H_
#define __GLVIEWPORT_H_

#include "Framework.h"

#include "GLCamera.h"

class GLViewport : public StandardViewport
{
private:

  int m_left;
  int m_bottom;
  int m_width;
  int m_height;

public:
  GLViewport();
  virtual ~GLViewport();

  // functions
  short             Initialize(short left, short bottom, unsigned short Width, unsigned short Height);
  short             Initialize(StandardGraphics *Graphics,
                              unsigned short Width, unsigned short Height);
  void              Uninitialize();

  StandardViewport*         CreateViewport();
  void                      SetViewport();
  void*             GetSurface();



    void              BeginScene();
    void              EndScene();
    void              Flip();
    void              Clear();
    void              DrawTextF(char *buffer, _Rect rect, short LeftCenterRight, StandardColor*color );




};

#endif
