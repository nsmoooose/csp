#ifndef __D3DMATERIALOBJECT_H_
#define __D3DMATERIALOBJECT_H_

#include "graphics/typesgraphics.h"


/** \class D3DMaterialObject
 * This class implements the MaterialObject class
 * for the Direct3D api.
 */
class D3DMaterialObject : public MaterialObject
{
public:
	D3DMaterialObject(StandardGraphics * pDisplay);
	~D3DMaterialObject();
	virtual void Create(_Color * emission,
						_Color * specular,
						_Color * diffuse,
						_Color * ambient,
						float shininess);
	virtual void Destroy();
	virtual void Apply();
	virtual MaterialObject * GetCurrentMaterial();

private:

	  D3DMATERIAL8 *Mat;

	D3DGraphics * m_pD3Display;

//	_Color emission;
//	_Color specular;
//	_Color diffuse;
//	_Color ambient;

//	float  shininess;

//	static D3DMaterialObject * m_currMat;

};

#endif