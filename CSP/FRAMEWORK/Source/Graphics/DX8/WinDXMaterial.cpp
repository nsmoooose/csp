#ifdef D3D

#include "WinDXMaterial.h"

WinDXMaterial::WinDXMaterial(StandardGraphics *Graphics)
{

  p_Graphics = Graphics;
  p_Device = (IDirect3DDevice8 *)p_Graphics->GetDevice();

  Mat = 0;
}

WinDXMaterial::~WinDXMaterial()
{
  Uninitialize();
}

void WinDXMaterial::Initialize(StandardColor * emission,
						                   StandardColor * specular,
						                   StandardColor * diffuse,
						                   StandardColor * ambient,
						                   float shininess)
{

  Mat = new D3DMATERIAL8;

  ZeroMemory(Mat, sizeof(D3DMATERIAL8));

  D3DCOLORVALUE p_Color;

  p_Color.r = diffuse->r / (float)255;
  p_Color.g = diffuse->g / (float)255;
  p_Color.b = diffuse->b / (float)255;
  p_Color.a = diffuse->a / (float)255;

  Mat->Diffuse = p_Color;

  p_Color.r = ambient->r / (float)255;
  p_Color.g = ambient->g / (float)255;
  p_Color.b = ambient->b / (float)255;
  p_Color.a = ambient->a / (float)255;

  Mat->Ambient = p_Color;

  p_Color.r = emission->r / (float)255;
  p_Color.g = emission->g / (float)255;
  p_Color.b = emission->b / (float)255;
  p_Color.a = emission->a / (float)255;

  Mat->Emissive = p_Color;

  p_Color.r = specular->r / (float)255;
  p_Color.g = specular->g / (float)255;
  p_Color.b = specular->b / (float)255;
  p_Color.a = specular->a / (float)255;

  Mat->Specular = p_Color;

  Mat->Power = shininess;

}

inline void WinDXMaterial::Apply()
{
  p_Device->SetMaterial(Mat);
}

void WinDXMaterial::Uninitialize()
{
  if(Mat != 0)
  {
    delete Mat;
    Mat = 0;
  }
}

StandardMaterial * WinDXMaterial::GetCurrentMaterial()
{
  return NULL;
}

#endif


