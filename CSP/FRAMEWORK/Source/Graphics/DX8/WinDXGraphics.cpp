#ifdef D3D

#include "WinDXGraphics.h"

WinDXGraphics::WinDXGraphics()
{
  p_D3D = 0;
  p_Device = 0;
  p_Font = 0;

  p_SurfaceFormat = D3DFMT_UNKNOWN;
  p_ZBufferFormat = D3DFMT_UNKNOWN;

}

WinDXGraphics::~WinDXGraphics()
{
  Uninitialize();
}

short WinDXGraphics::Initialize(StandardApp *App, _GRAPHICS_INIT_STRUCT *Init)
{

  p_App = App;

  memcpy(&p_Init, Init, sizeof(p_Init));

  // ----

  D3DFORMAT tryformat = D3DFMT_UNKNOWN;

  short threeshot;
  long lerror;

  p_D3D = Direct3DCreate8( D3D_SDK_VERSION );

  D3DDISPLAYMODE d3ddm;

  p_D3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm );

  D3DPRESENT_PARAMETERS d3dpp; 
  ZeroMemory( &d3dpp, sizeof(d3dpp) );

  if(Init->Fullscreen == true)
  {
    d3dpp.Windowed   = FALSE;

    for(threeshot = 0; threeshot < 3; threeshot ++)
    {
      switch(Init->Depth)
      {
        case 32:
          switch(threeshot)
          {
            case 0:
              tryformat = D3DFMT_A8R8G8B8;
              break;
            case 1:
              tryformat = D3DFMT_X8R8G8B8;
              break;
            case 2:
              tryformat = D3DFMT_R8G8B8;
              break;
          }
          break;
        default:
          switch(threeshot)
          {
            case 0:
              tryformat = D3DFMT_R5G6B5;
              break;
            case 1:
              tryformat = D3DFMT_X1R5G5B5;
              break;
            case 2:
              tryformat = D3DFMT_A1R5G5B5;
              break;
          }
          break;
      }
      lerror = p_D3D->CheckDeviceType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, tryformat, tryformat, FALSE);

      if(lerror == 0)
      {
        p_SurfaceFormat = tryformat;
        break;
      }
    }
    
    if(p_SurfaceFormat == D3DFMT_UNKNOWN)
    {
      return _FUNKY_VIDEO_FORMAT;
    }

	  if(Init->VSync == false)
	  {
		  d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	  }

    D3DDISPLAYMODE dm;

    short counter;
    UINT highesthz = 0;

    threeshot = p_D3D->GetAdapterModeCount(D3DADAPTER_DEFAULT);
    for(counter = 0; counter < threeshot; counter++)
    {
      p_D3D->EnumAdapterModes(D3DADAPTER_DEFAULT, counter, &dm);
      if(Init->Width == dm.Width && Init->Height == dm.Height)
      {
        if(dm.RefreshRate > highesthz)
        {
          highesthz = dm.RefreshRate;
        }
      }

    }
    d3dpp.FullScreen_RefreshRateInHz = highesthz;

  }
  else
  {
    d3dpp.FullScreen_RefreshRateInHz = 0;
    d3dpp.Windowed   = TRUE;
    p_SurfaceFormat = d3ddm.Format;
  }

  d3dpp.BackBufferFormat = p_SurfaceFormat;
  d3dpp.BackBufferWidth = Init->Width;
  d3dpp.BackBufferHeight = Init->Height;
  d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;

  p_ZBufferFormat = D3DFMT_UNKNOWN;

  for(threeshot = 0; threeshot < 7; threeshot ++)
  {
    switch(threeshot)
    {
      case 0:
        tryformat = D3DFMT_D32;
        break;
      case 1:
        tryformat = D3DFMT_D24S8;
        break;
      case 2:
        tryformat = D3DFMT_D24X8;
        break;
      case 3:
        tryformat = D3DFMT_D24X4S4;
        break;
      case 4:
        tryformat = D3DFMT_D16;
        break;  
      case 5:
        tryformat = D3DFMT_D15S1;
        break;
      case 6:
        tryformat = D3DFMT_D16_LOCKABLE;
        break;
	    default:
		    break;

    }
    lerror = p_D3D->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, p_SurfaceFormat, p_SurfaceFormat, tryformat);
    if(lerror == 0)
    {
      p_ZBufferFormat = tryformat;
      break;
    }
  }

  if(p_ZBufferFormat == D3DFMT_UNKNOWN)
  {
      return _FUNKY_DEPTH_FORMAT;
  }

  d3dpp.EnableAutoDepthStencil = TRUE;
  d3dpp.AutoDepthStencilFormat = p_ZBufferFormat;

  if(Init->HardwardTL == true)
  {
    lerror = p_D3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (HWND)p_Init.Handle,
                        D3DCREATE_HARDWARE_VERTEXPROCESSING,
                        &d3dpp, &p_Device );
  }
  else
  {
    lerror = p_D3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (HWND)p_Init.Handle,
                        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                        &d3dpp, &p_Device );
  }

  if(lerror != 0)
  {
    return _CREATING_3DDEVICE_FAILED;
  }

  p_Device->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &p_Viewport);
  p_Device->GetDepthStencilSurface(&p_ZBuffer);

  // Set up the textures

  p_Device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
  p_Device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
  p_Device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

  p_Device->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
  p_Device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE  );
  p_Device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

  p_Device->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
  p_Device->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
  p_Device->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );

  p_Device->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_MIRROR  );
  p_Device->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_MIRROR  );

  p_Device->SetRenderState(D3DRS_ALPHATESTENABLE,  TRUE );
  p_Device->SetRenderState(D3DRS_ALPHAREF,         0x08 );
  p_Device->SetRenderState(D3DRS_ALPHAFUNC,        D3DCMP_GREATEREQUAL );

  p_Device->SetRenderState(D3DRS_ALPHABLENDENABLE,        TRUE);
  p_Device->SetRenderState(D3DRS_SRCBLEND,                D3DBLEND_SRCALPHA);
  p_Device->SetRenderState(D3DRS_DESTBLEND,               D3DBLEND_INVSRCALPHA);

  p_Device->SetRenderState(D3DRS_DITHERENABLE,            TRUE );
  //p_Device->SetRenderState(D3DRS_SPECULARENABLE,          TRUE );
  p_Device->SetRenderState(D3DRS_ZENABLE,                 D3DZB_TRUE  );
  p_Device->SetRenderState(D3DRS_ZFUNC,                   D3DCMP_LESSEQUAL);
  p_Device->SetRenderState(D3DRS_AMBIENT,                 0xFF707070);
  p_Device->SetRenderState(D3DRS_COLORVERTEX,             TRUE);
  p_Device->SetRenderState(D3DRS_LIGHTING,                TRUE );
  p_Device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE,   D3DMCS_MATERIAL);
  p_Device->SetRenderState(D3DRS_LASTPIXEL,               FALSE);

  if(Init->Wireframe == TRUE)
  {
    p_Device->SetRenderState( D3DRS_FILLMODE,        D3DFILL_WIREFRAME );
  }

  //p_Device->SetRenderState( D3DRS_SHADEMODE,        D3DSHADE_FLAT  );

  HFONT font = CreateFont(15,5,0,0, FW_LIGHT, 0,0,0, ANSI_CHARSET,
                            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                            DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,
                            "Courier");

  D3DXCreateFont(p_Device, font , &p_Font);

  DeleteObject(font);

  p_Device->AddRef();

  D3DLIGHT8 d3dLight;
  ZeroMemory(&d3dLight, sizeof(D3DLIGHT8));

  d3dLight.Type = D3DLIGHT_DIRECTIONAL;
  d3dLight.Diffuse.r  = 1.0f;
  d3dLight.Diffuse.g  = 1.0f;
  d3dLight.Diffuse.b  = 1.0f;
  d3dLight.Ambient.r  = 1.0f;
  d3dLight.Ambient.g  = 1.0f;
  d3dLight.Ambient.b  = 1.0f;
  d3dLight.Specular.r = 1.0f;
  d3dLight.Specular.g = 1.0f;
  d3dLight.Specular.b = 1.0f;

  d3dLight.Direction.x = 0.1f;
  d3dLight.Direction.y = -0.2f;
  d3dLight.Direction.z = 0.8f;

  p_Device->SetLight(0, &d3dLight);
  p_Device->LightEnable(0, TRUE);

  return 0;
}

void WinDXGraphics::Uninitialize()
{

  if(p_Font != 0)
  {
    p_Font->Release();
    p_Font = 0;
  }

  if(p_Device != 0)
  {
    p_Device->Release();
    p_Device->Release();
    p_Device = 0;
  }

  if(p_D3D != 0)
  {
    p_D3D->Release();
    p_D3D = 0;
  }

  return;
}


inline void WinDXGraphics::BeginScene()
{
  p_Camera->Apply();

  p_Device->BeginScene();
}

inline void WinDXGraphics::EndScene()
{
  p_Device->EndScene();
}

inline void WinDXGraphics::Flip()
{
  p_Device->Present( NULL, NULL, NULL, NULL );
}

inline void WinDXGraphics::Clear()
{
  p_Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, p_ClearRGBA, 1.0f, 0);
}

void WinDXGraphics::DrawTextF(char *buffer, _Rect rect, short LeftCenterRight, short TopCenterBottom,  StandardColor* color)
{

  DWORD format;

  D3DCOLOR ColorRGBA;
  
  ColorRGBA = D3DCOLOR_RGBA(color->r, color->g, color->b, color->a);

  RECT rect2;

  rect2.top = (long)(rect.y1 * p_Init.Height);
  rect2.left = (long)(rect.x1 * p_Init.Width);
  rect2.right = (long)(rect.x2 * p_Init.Width);
  rect2.bottom = (long)(rect.y2 * p_Init.Height);

  format = DT_WORDBREAK;

  switch(LeftCenterRight)
  {
  case 0:
    format += DT_LEFT;
    break;
  case 1:
    format += DT_CENTER;
    break;
  case 2:
    format += DT_RIGHT;
    break;
  }

  switch(TopCenterBottom)
  {
  case 0:
    format += DT_TOP;
    break;
  case 1:
    format += DT_VCENTER;
    break;
  case 2:
    format += DT_BOTTOM;
    break;
  }

  p_Font->DrawText(buffer, strlen(buffer), &rect2, format, ColorRGBA);


}

StandardViewport* WinDXGraphics::CreateViewport()
{

  StandardViewport *viewport;

  viewport = new WinDXViewport();

  return viewport;
}

void WinDXGraphics::SetViewport(StandardViewport *viewport)
{
  p_Device->SetRenderTarget(p_Viewport, p_ZBuffer);

  //p_Device->SetTransform( D3DTS_PROJECTION, (D3DMATRIX *)&GetProjectionMatrix());

  return;
}

StandardPolygonMesh* WinDXGraphics::CreatePolygonMesh()
{

  WinDXPolygonMesh *polygonMesh = new WinDXPolygonMesh(this);

  return polygonMesh;
}

StandardPolygonFlat* WinDXGraphics::CreatePolygonFlat()
{

  WinDXPolygonFlat *polygonMesh = new WinDXPolygonFlat(this);

  return polygonMesh;
}


StandardPolygonBillboard* WinDXGraphics::CreatePolygonBillboard()
{

  WinDXPolygonBillboard *polygonMesh = new WinDXPolygonBillboard(this);

  return polygonMesh;
}

unsigned long WinDXGraphics::DrawPolygonMesh(StandardPolygonMesh * pMesh)
{
  return pMesh->Draw();
}

StandardTexture* WinDXGraphics::CreateTexture()
{
	WinDXTexture* pTexture = new WinDXTexture(this);

	return pTexture;
}

void WinDXGraphics::SetTexture(StandardTexture *pTexture)
{
  if(pTexture != 0)
  {
	  pTexture->Apply();
  }
  else
  {
    p_Device->SetTexture(0, 0);
  }
}

StandardMaterial * WinDXGraphics::CreateMaterial()
{
	WinDXMaterial * pMaterial = new WinDXMaterial(this);
	return pMaterial;
}

void WinDXGraphics::SetMaterial(StandardMaterial *pMaterial)
{
  if(pMaterial != 0)
  {
    pMaterial->Apply();
  }
  else
  {
    //p_Device->SetMaterial(0);
  }
}

StandardCamera * WinDXGraphics::CreateCamera()
{
  WinDXCamera * pCamera = new WinDXCamera(this);
  return pCamera;
}

void WinDXGraphics::SetCamera(StandardCamera *Camera)
{
  p_Camera = Camera;

  p_Device->SetTransform( D3DTS_PROJECTION, (D3DMATRIX *)p_Camera->GetProjectionMatrix());

}

void  WinDXGraphics::SetRenderState(StateTypes stateType, int state)
{
	switch (stateType)
	{
	case FW_WINDING:
     if(state == FW_CW)
     {
     p_Device->SetRenderState(D3DRS_CULLMODE,                D3DCULL_CW);
     }
     else if(state == FW_CCW)
     {
     p_Device->SetRenderState(D3DRS_CULLMODE,                D3DCULL_CCW);
     }
	break;
	
	case FW_SHADING:
		if (state == FW_SMOOTH)
		{
      p_Device->SetRenderState( D3DRS_SHADEMODE,        D3DSHADE_GOURAUD   );
		}
		else if (state == FW_FLAT)
		{
      p_Device->SetRenderState( D3DRS_SHADEMODE,        D3DSHADE_FLAT  );
		}
	break;

	case FW_LIGHTING:
		if (state == (int)true)
		{
	    p_Device->SetRenderState(D3DRS_LIGHTING,                TRUE );
		}
		else
		{
      p_Device->SetRenderState(D3DRS_LIGHTING,                FALSE );
		}
	break;
	
	case FW_ZBUFFER:
		if (state == (int)true)
		{
      p_Device->SetRenderState(D3DRS_ZENABLE,                 D3DZB_TRUE  );
		}
		else
		{
      p_Device->SetRenderState(D3DRS_ZENABLE,                 D3DZB_FALSE  );
		}
	break;

	case FW_ZBUFFERWRITE:
		if (state == (int)true)
		{
      p_Device->SetRenderState(D3DRS_ZWRITEENABLE,                 TRUE  );
		}
		else
		{
      p_Device->SetRenderState(D3DRS_ZWRITEENABLE,                 FALSE  );
		}
	break;



	case FW_CULLFACE:
		//if (state == FW_FRONTFACE)
		//{
		//	glEnable(GL_CULL_FACE);
		//	glCullFace(GL_FRONT);
		//}
		//else if (state == FW_BACKFACE)
		//{
		//	glEnable(GL_CULL_FACE);
		//	glCullFace(GL_BACK);
		//}
	break;
	case FW_TEXTURE:
		//if (state == true)
		//{
		//	glEnable( GL_TEXTURE_2D );
		//}
		//else
		//{
		//	glDisable(GL_TEXTURE_2D);
		//}
	break;
	case FW_BLENDING:
		//if (state == true)
		//{
		//	glEnable( GL_BLEND );
		//}
		//else
		//{
		//	glDisable (GL_BLEND);
		//}
	break;
	case FW_WIREFRAME:
    if (state == (int)true)
    {
      p_Device->SetRenderState( D3DRS_FILLMODE,        D3DFILL_WIREFRAME );
    }
    else
    {
      p_Device->SetRenderState( D3DRS_FILLMODE,        D3DFILL_SOLID  );
    }
  break;
	case FW_FOG:
		if (state == (int)true)
		{
      p_Device->SetRenderState( D3DRS_FOGENABLE,        TRUE  );
		}
		else
		{
      p_Device->SetRenderState( D3DRS_FOGENABLE,        FALSE  );
		}
	  break;
	case FW_FOGMODE:
		{
			switch (state)
			{
			case FW_FOG_EXP:
				{
          p_Device->SetRenderState( D3DRS_FOGTABLEMODE,        D3DFOG_EXP   );
          p_Device->SetRenderState( D3DRS_FOGVERTEXMODE,       D3DFOG_EXP   );
				}
			  break;
				
			case FW_FOG_EXP2:
				{
          p_Device->SetRenderState( D3DRS_FOGTABLEMODE,        D3DFOG_EXP2    );
          p_Device->SetRenderState( D3DRS_FOGVERTEXMODE,       D3DFOG_EXP2   );
				}
			  break;

			case FW_FOG_LINEAR:
				{
          p_Device->SetRenderState( D3DRS_FOGTABLEMODE,        D3DFOG_LINEAR );
          p_Device->SetRenderState( D3DRS_FOGVERTEXMODE,       D3DFOG_LINEAR );
				}
			  break;
			}
		}
	  break;
	case FW_FOGSTART:
    {
      float fog  = (float)state;
      p_Device->SetRenderState( D3DRS_FOGSTART,        *(DWORD *)(&fog));

		}
	  break;
	case FW_FOGEND:
		{
      float fog  = (float)state;
      p_Device->SetRenderState( D3DRS_FOGEND,        *(DWORD*) (&fog));

		}
	  break;
	case FW_FOGDENSITY:
		{
      float fog  = (float)state;
      p_Device->SetRenderState( D3DRS_FOGDENSITY,        *(DWORD*) (&fog));
		}
	  break;
	case FW_FOGCOLOR:
		{
			StandardColor *p_FogColor = (StandardColor*)state;
      D3DCOLOR rgb;
      rgb = D3DCOLOR_RGBA(p_FogColor->r, p_FogColor->g, p_FogColor->b, p_FogColor->a);

      p_Device->SetRenderState( D3DRS_FOGCOLOR  , rgb);
		}
	  break;
	}
}

int   WinDXGraphics::GetRenderState(StateTypes stateType)
{
  return 0;
}

void WinDXGraphics::SetBackgroundColor(StandardColor *color)
{
  p_ClearRGBA = D3DCOLOR_RGBA(color->r, color->g, color->b, color->a);
}

void WinDXGraphics::SetAmbientColor(StandardColor *color)
{
  p_Device->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_RGBA(color->r, color->g, color->b, color->a));
}

void WinDXGraphics::SetGamma(StandardColor *color)
{

  D3DGAMMARAMP ramp;

  p_Device->SetGammaRamp(D3DSGR_NO_CALIBRATION , &ramp); 

}

void WinDXGraphics::SetFogColor(StandardColor *color)
{
  SetRenderState(FW_FOG, true);
  SetRenderState(FW_FOGCOLOR, (int)color);
  SetRenderState(FW_FOGMODE, FW_FOG_LINEAR);
  SetRenderState(FW_FOGSTART, (int)p_Init.NearZ);
  SetRenderState(FW_FOGEND, (int)p_Init.FarZ);
}

void*   WinDXGraphics::GetDevice()
{
  return p_Device;
}

void*   WinDXGraphics::GetFont()
{
  return p_Font;
}

_GRAPHICS_INIT_STRUCT* WinDXGraphics::GetInit()
{
  return &p_Init;
}

void* WinDXGraphics::GetSurfaceFormat()
{
  return &p_SurfaceFormat;
}

void* WinDXGraphics::GetZSurfaceFormat()
{
  return &p_ZBufferFormat;
}

//////////////////////////////////////////////////////////

void WinDXGraphics::DumpDXInfo()
{

  FILE *infoFile = fopen("DXInfo.out", "a");

  D3DCAPS8 devCaps;

  p_D3D->GetDeviceCaps(D3DADAPTER_DEFAULT,  D3DDEVTYPE_HAL, &devCaps);

  fprintf(infoFile, "Device Type %d\n", devCaps.DeviceType);
	fprintf(infoFile, "Adapter ordinal %d\n", devCaps.AdapterOrdinal);
	fprintf(infoFile, "Device Caps 0x%x\n", devCaps.Caps);
  if (devCaps.Caps & D3DCAPS_READ_SCANLINE)
  {
		fprintf(infoFile, "D3DCAPS_READ_SCANLINE TRUE\n");
  }
	else
  {
		fprintf(infoFile, "D3DCAPS_READ_SCANLINE FALSE\n");
  }

	fprintf(infoFile, "\n");
	fprintf(infoFile, "Device Caps2 0x%x\n", devCaps.Caps2);

	fprintf(infoFile, "D3DCAPS2_CANCALIBRATEGAMMA %s\n", 
		(devCaps.Caps2 & D3DCAPS2_CANCALIBRATEGAMMA) ? "TRUE" : "FALSE");
	fprintf(infoFile, "D3DCAPS2_CANRENDERWINDOWED %s\n", 
		(devCaps.Caps2 & D3DCAPS2_CANRENDERWINDOWED) ? "TRUE" : "FALSE");
	fprintf(infoFile, "D3DCAPS2_FULLSCREENGAMMA %s\n", 
		(devCaps.Caps2 & D3DCAPS2_FULLSCREENGAMMA) ? "TRUE" : "FALSE");
	fprintf(infoFile, "D3DCAPS2_NO2DDURING3DSCENE %s\n", 
		(devCaps.Caps2 & D3DCAPS2_NO2DDURING3DSCENE) ? "TRUE" : "FALSE");
	fprintf(infoFile, "\n");

	fprintf(infoFile, "Presentation Intervals 0x%x\n", devCaps.PresentationIntervals);
	fprintf(infoFile, "D3DPRESENT_INTERVAL_IMMEDIATE %s\n", 
		(devCaps.PresentationIntervals & D3DPRESENT_INTERVAL_IMMEDIATE) ? "TRUE" : "FALSE");
	fprintf(infoFile, "D3DPRESENT_INTERVAL_ONE %s\n", 
		(devCaps.PresentationIntervals & D3DPRESENT_INTERVAL_ONE) ? "TRUE" : "FALSE");
	fprintf(infoFile, "D3DPRESENT_INTERVAL_TWO %s\n", 
		(devCaps.PresentationIntervals & D3DPRESENT_INTERVAL_TWO) ? "TRUE" : "FALSE");
	fprintf(infoFile, "D3DPRESENT_INTERVAL_THREE %s\n", 
		(devCaps.PresentationIntervals & D3DPRESENT_INTERVAL_THREE) ? "TRUE" : "FALSE");
	fprintf(infoFile, "D3DPRESENT_INTERVAL_FOUR %s\n", 
		(devCaps.PresentationIntervals & D3DPRESENT_INTERVAL_FOUR) ? "TRUE" : "FALSE");
	fprintf(infoFile, "\n");

	fprintf(infoFile, "Cursor Caps 0x%x\n", devCaps.CursorCaps);
	fprintf(infoFile, "D3DCURSORCAPS_COLOR %s\n", 
		(devCaps.CursorCaps & D3DCURSORCAPS_COLOR) ? "TRUE" : "FALSE");
	fprintf(infoFile, "D3DCURSORCAPS_LOWRES %s\n", 
		(devCaps.CursorCaps & D3DCURSORCAPS_LOWRES) ? "TRUE" : "FALSE");
	fprintf(infoFile, "\n");

	fprintf(infoFile, "DevCaps 0x%x\n", devCaps.DevCaps);
	fprintf(infoFile, "D3DDEVCAPS_CANBLTSYSTONONLOCAL %s\n",
		(devCaps.DevCaps & D3DDEVCAPS_CANBLTSYSTONONLOCAL) ? "TRUE" : "FALSE");
	fprintf(infoFile, "D3DDEVCAPS_CANRENDERAFTERFLIP %s\n",
		(devCaps.DevCaps & D3DDEVCAPS_CANRENDERAFTERFLIP) ? "TRUE" : "FALSE");
	fprintf(infoFile, "D3DDEVCAPS_DRAWPRIMTLVERTEX %s\n",
	    (devCaps.DevCaps & D3DDEVCAPS_DRAWPRIMTLVERTEX) ? "TRUE" : "FALSE");
	fprintf(infoFile, "D3DDEVCAPS_EXECUTESYSTEMMEMORY %s\n",
	    (devCaps.DevCaps & D3DDEVCAPS_EXECUTESYSTEMMEMORY) ? "TRUE" : "FALSE");
	fprintf(infoFile, "D3DDEVCAPS_EXECUTEVIDEOMEMORY %s\n",
	    (devCaps.DevCaps & D3DDEVCAPS_EXECUTEVIDEOMEMORY) ? "TRUE" : "FALSE");
	fprintf(infoFile, "D3DDEVCAPS_HWRASTERIZATION %s\n",
	    (devCaps.DevCaps & D3DDEVCAPS_HWRASTERIZATION) ? "TRUE" : "FALSE");
	fprintf(infoFile, "D3DDEVCAPS_HWTRANSFORMANDLIGHT %s\n",
	    (devCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) ? "TRUE" : "FALSE");
	fprintf(infoFile, "D3DDEVCAPS_NPATCHES %s\n",
	    (devCaps.DevCaps & D3DDEVCAPS_NPATCHES) ? "TRUE" : "FALSE");
	fprintf(infoFile, "D3DDEVCAPS_PUREDEVICE %s\n",
	    (devCaps.DevCaps & D3DDEVCAPS_PUREDEVICE) ? "TRUE" : "FALSE");
	fprintf(infoFile, "D3DDEVCAPS_QUINTICRTPATCHES %s\n",
	    (devCaps.DevCaps & D3DDEVCAPS_QUINTICRTPATCHES) ? "TRUE" : "FALSE");
	fprintf(infoFile, "D3DDEVCAPS_RTPATCHES %s\n",
	    (devCaps.DevCaps & D3DDEVCAPS_RTPATCHES) ? "TRUE" : "FALSE");
	fprintf(infoFile, "D3DDEVCAPS_RTPATCHHANDLEZERO %s\n",
	    (devCaps.DevCaps & D3DDEVCAPS_RTPATCHHANDLEZERO) ? "TRUE" : "FALSE");
	fprintf(infoFile, "D3DDEVCAPS_SEPARATETEXTUREMEMORIES %s\n",
	    (devCaps.DevCaps & D3DDEVCAPS_SEPARATETEXTUREMEMORIES) ? "TRUE" : "FALSE");
	fprintf(infoFile, "D3DDEVCAPS_TEXTURENONLOCALVIDMEM %s\n",
	    (devCaps.DevCaps & D3DDEVCAPS_TEXTURENONLOCALVIDMEM) ? "TRUE" : "FALSE");
	fprintf(infoFile, "D3DDEVCAPS_TEXTURESYSTEMMEMORY %s\n",
	    (devCaps.DevCaps & D3DDEVCAPS_TEXTURESYSTEMMEMORY) ? "TRUE" : "FALSE");
	fprintf(infoFile, "D3DDEVCAPS_TEXTUREVIDEOMEMORY %s\n",
	    (devCaps.DevCaps & D3DDEVCAPS_TEXTUREVIDEOMEMORY) ? "TRUE" : "FALSE");
	fprintf(infoFile, "D3DDEVCAPS_TLVERTEXSYSTEMMEMORY %s\n",
	    (devCaps.DevCaps & D3DDEVCAPS_TLVERTEXSYSTEMMEMORY) ? "TRUE" : "FALSE");
	fprintf(infoFile, "D3DDEVCAPS_TLVERTEXVIDEOMEMORY %s\n",
	    (devCaps.DevCaps & D3DDEVCAPS_TLVERTEXVIDEOMEMORY) ? "TRUE" : "FALSE");

	fclose (infoFile);

}


#endif
