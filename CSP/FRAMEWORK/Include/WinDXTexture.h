#ifdef D3D
#ifndef __D3DTEXTURE_H_
  #define __D3DTEXTURE_H_


  #include "Framework.h"

  class WinDXTexture : public StandardTexture
  {

    private:
      StandardGraphics      *p_Graphics;
      _GRAPHICS_INIT_STRUCT *p_Init;

      IDirect3DDevice8      *p_Device;
	    IDirect3DTexture8     *p_Tex;

    public:
	    WinDXTexture(StandardGraphics *Graphics);
	    ~WinDXTexture();

	    short Initialize(const char *filename, int filetype);
      short Initialize(const char *filename);
      short Initialize(const char *filename, _MIPMAP Levels, StandardColor *ColorKey);

	    void  Uninitialize();

      void  Apply();


  };

#endif
#endif
