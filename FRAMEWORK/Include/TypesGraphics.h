#ifndef __TYPESGRAPHICS_H__
#define __TYPESGRAPHICS_H__

#include "Framework.h"

class StandardGraphics
{

  private:

  public:
    virtual ~StandardGraphics() {};

    // functions
	  //setup and cleanup
    virtual short                   Initialize(StandardApp *App, _GRAPHICS_INIT_STRUCT *Init) = 0;
    virtual void                    Uninitialize() = 0;

	  // viewports
    virtual StandardViewport*       CreateViewport() = 0;
    virtual void                    SetViewport(StandardViewport*) = 0;

	  // scene management
    virtual void                    BeginScene() = 0;
    virtual void                    EndScene() = 0;
    virtual void                    Flip() = 0;
    virtual void                    Clear() = 0;
    virtual void                    DrawTextF(char *buffer, _Rect rect, short LeftCenterRight, short TopCenterBottom,  StandardColor* color) = 0;
	  
    virtual void                    SetBackgroundColor(StandardColor *color) = 0;
    virtual void                    SetFogColor(StandardColor *color) = 0;
    virtual void                    SetAmbientColor(StandardColor *color) = 0;
    virtual void                    SetGamma(StandardColor *color) = 0;

	  // drawing
    virtual StandardPolygonMesh*       CreatePolygonMesh() = 0;
    virtual StandardPolygonFlat*       CreatePolygonFlat() = 0;
    virtual StandardPolygonBillboard*  CreatePolygonBillboard() = 0;

	  virtual unsigned long           DrawPolygonMesh(StandardPolygonMesh * pMesh) = 0;

	  // textures
	  virtual StandardTexture*        CreateTexture() = 0;
    virtual void                    SetTexture(StandardTexture *pTexture) = 0;

	  // materials  
	  virtual StandardMaterial*       CreateMaterial() = 0;
	  virtual void                    SetMaterial(StandardMaterial *pMaterial) = 0;

	  // camera
	  virtual StandardCamera*         CreateCamera() = 0;
    virtual void                    SetCamera(StandardCamera *Camera) = 0;

	  // light
    //	virtual StandardLight *     CreateLight() = 0;
    //	virtual void                SetLight(StandardLight * pLight) = 0;

	  // states
	  virtual void                    SetRenderState(StateTypes stateType, int state) = 0;
	  virtual int                     GetRenderState(StateTypes stateType) = 0;

  //protected:

    virtual _FORMAT                 GetSurfaceFormat() = 0;
    virtual _FORMAT                 GetZSurfaceFormat() = 0;
    virtual _DEVICE                 GetDevice() = 0;
    virtual _FONT                   GetFont() = 0;
    virtual _GRAPHICS_INIT_STRUCT*  GetInit() = 0;

};

#endif
