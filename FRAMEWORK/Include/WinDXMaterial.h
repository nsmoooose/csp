#ifdef D3D
#ifndef __D3DMATERIAL_H_
#define __D3DMATERIAL_H_

#include "Framework.h"


/** \class WinDXMaterial
 * This class implements the Material class
 * for the Direct3D api.
 */
class WinDXMaterial : public StandardMaterial
{

private:
  StandardGraphics  *p_Graphics;
  IDirect3DDevice8  *p_Device;

  D3DMATERIAL8 *Mat;

public:

	WinDXMaterial(StandardGraphics *Graphics);
	~WinDXMaterial();

	void Initialize(StandardColor * emission,
						      StandardColor * specular,
						      StandardColor * diffuse,
						      StandardColor * ambient,
						      float shininess);
	void Uninitialize();
	void Apply();

	StandardMaterial * GetCurrentMaterial();

};

#endif
#endif