#ifdef D3D

#ifndef __WINDXGRAPHICS_H_
#define __WINDXGRAPHICS_H_

#include "Framework.h"


#include "WinDXCamera.h"
#include "WinDXViewport.h"
#include "WinDXPolygonMesh.h"
#include "WinDXPolygonFlat.h"
#include "WinDXPolygonBillboard.h"
#include "WinDXTexture.h"
#include "WinDXMaterial.h"

class WinDXGraphics : public StandardGraphics
{

  private:

    StandardApp            *p_App;
    //StandardWindow3D       *p_Window3D;

    IDirect3DSurface8      *p_Viewport;
    IDirect3DSurface8      *p_ZBuffer;
    IDirect3DDevice8       *p_Device;
    IDirect3D8             *p_D3D;

    LPD3DXFONT              p_Font;
    D3DFORMAT               p_SurfaceFormat;
    D3DFORMAT               p_ZBufferFormat;

    _GRAPHICS_INIT_STRUCT   p_Init;

    StandardCamera         *p_Camera;

    D3DLIGHT8               p_Light; 
    D3DCOLOR                p_FogRGBA;
    D3DCOLOR                p_ClearRGBA;

  public:

    WinDXGraphics();
    ~WinDXGraphics();

    // functions
    short                   Initialize(StandardApp *App, _GRAPHICS_INIT_STRUCT *Init);
    void                    Uninitialize();

    // viewports
    StandardViewport*       CreateViewport();
    void                    SetViewport(StandardViewport*);

    // scene
    void                    BeginScene();
    void                    EndScene();
    void                    Flip();
    void                    Clear();
    void                    DrawTextF(char *buffer, _Rect rect, short LeftCenterRight, short TopCenterBottom,  StandardColor* color);
    
    void                    SetBackgroundColor(StandardColor *color);
    void                    SetFogColor(StandardColor *color);
    void                    SetAmbientColor(StandardColor *color);
    void                    SetGamma(StandardColor *color);

    // polygon mesh
    StandardPolygonMesh*         CreatePolygonMesh();
    StandardPolygonFlat*         CreatePolygonFlat();
    StandardPolygonBillboard*    CreatePolygonBillboard();
	  unsigned long           DrawPolygonMesh(StandardPolygonMesh * pMesh);

    // textures
	  StandardTexture*        CreateTexture();
    void                    SetTexture(StandardTexture *Texture);

	  // materials
	  StandardMaterial*       CreateMaterial();
	  void                    SetMaterial(StandardMaterial *pMaterial);

	  // camera
	  StandardCamera*         CreateCamera();
    void                    SetCamera(StandardCamera *Camera);

    // render states
	  void                    SetRenderState(StateTypes stateType, int state);
	  int                     GetRenderState(StateTypes stateType);

  //protected:
	  void                    DumpDXInfo();

    void*                   GetDevice();
    void*                   GetFont();
    _GRAPHICS_INIT_STRUCT*  GetInit();
    void*                   GetSurfaceFormat();
    void*                   GetZSurfaceFormat();

};

#endif
#endif

