#ifdef OGL

#ifndef _OPENGLGRAPHICS_H_
#define _OPENGLGRAPHICS_H_

#include "Framework.h"
#include "GLCamera.h"
#include "GLPolygonMesh.h"
#include "GLPolygonFlat.h"
#include "GLPolygonBillboard.h"
#include "GLFont.h"

class GLGraphics : public StandardGraphics 
{

protected:
  _GRAPHICS_INIT_STRUCT  p_Init;
  StandardApp           *p_App;
  bool                  m_depthFlag;

  StandardCamera         *p_Camera;

  GLFont	             *m_text;
  _Index	             *currentIndexBuffer;

public:

  GLGraphics();
  ~GLGraphics();
  
  virtual short                         Initialize(StandardApp *App, _GRAPHICS_INIT_STRUCT *Init) = 0;
  virtual void                          Uninitialize() = 0;

  void                          Clear();

  // creation routines
  virtual StandardPolygonMesh*  CreatePolygonMesh();
  virtual StandardPolygonFlat*  CreatePolygonFlat();
  virtual StandardPolygonBillboard*  CreatePolygonBillboard();

  virtual StandardMaterial*     CreateMaterial();
  virtual StandardCamera*       CreateCamera();
  virtual StandardTexture*      CreateTexture();
//  virtual StandardFont * CreateFont();

  void                    SetCamera(StandardCamera *Camera);

  unsigned long             DrawPolygonMesh(StandardPolygonMesh * polygonMesh);
  void                      DrawTriangle(StandardVector3 v1, StandardVector3 v2, 
		                                      StandardVector3 v3);
  void                      BeginTriangles();
  void                      EndTriangles();


  void                      SetTexture(StandardTexture * texture);
  void                      SetMaterial(StandardMaterial * material);


  // State setting routines

  void                      SetRenderState(StateTypes stateType, int state);
  int                       GetRenderState(StateTypes stateType);

  void                      SetBackgroundColor(StandardColor * color);
  void                      SetFogColor(StandardColor * color);

  virtual void                    BeginScene() = 0;
  virtual void                    EndScene() = 0;


  // Viewports
  virtual StandardViewport* CreateViewport();
  virtual void              ModifyViewport(float left, float top, float width, float height);
  void                      SetViewport(StandardViewport *view);
  void                      DrawViewport();

  void                      DrawTextF(char *buffer, _Rect rect, short LeftCenterRight,short TopCenterBottom, StandardColor *color );

  void*                   GetDevice();
  void*                   GetFont();
  _GRAPHICS_INIT_STRUCT*  GetInit();
  void*                   GetSurfaceFormat();
  void*                   GetZSurfaceFormat();


  void SetAmbientColor(StandardColor *color);

  void SetGamma(StandardColor *color);


};


#endif

#endif
