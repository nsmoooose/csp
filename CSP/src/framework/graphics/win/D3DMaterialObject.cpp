
#ifdef D3D

#include "D3D.h"
#include "D3DMaterialObject.h"


D3DMaterialObject::D3DMaterialObject(StandardGraphics * pDisplay) :
	MaterialObject(pDisplay)
{
	// do the cast here one time to the specific D3DDisplay;
	m_pD3Display = (D3DGraphics *)pDisplay;
}

D3DMaterialObject::~D3DMaterialObject()
{

}

void D3DMaterialObject::Create(_Color * emission,
						_Color * specular,
						_Color * diffuse,
						_Color * ambient,
						float shininess)
{
  Mat = new D3DMATERIAL8;

  ZeroMemory(Mat, sizeof(D3DMATERIAL8));

  Mat->Diffuse = (D3DCOLORVALUE)*(D3DCOLORVALUE *)diffuse;
  Mat->Ambient = (D3DCOLORVALUE)*(D3DCOLORVALUE *)ambient;
  Mat->Emissive = (D3DCOLORVALUE)*(D3DCOLORVALUE *)emission;
  Mat->Specular = (D3DCOLORVALUE)*(D3DCOLORVALUE *)specular;

}

void D3DMaterialObject::Destroy()
{

}

void D3DMaterialObject::Apply()
{
  m_pD3Display->Device->SetMaterial((D3DMATERIAL8 *)Mat );

}

MaterialObject * D3DMaterialObject::GetCurrentMaterial()
{
  return NULL;
}

#endif