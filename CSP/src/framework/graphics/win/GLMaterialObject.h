#ifndef __GLMATERIALOBJECT_H_
#define __GLMATERIALOBJECT_H_

#include "graphics/typesgraphics.h"

#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>		// Header File For The Glaux Library

/** \class GLMaterialObject
 * This class implements the MaterialObject class
 * for the OpenGL api.
 */
class GLMaterialObject : public MaterialObject
{
public:
	GLMaterialObject(StandardGraphics * pDisplay);
	~GLMaterialObject();
	virtual void Create(_Color * emission,
						_Color * specular,
						_Color * diffuse,
						_Color * ambient,
						float shininess);
	virtual void Destroy();
	virtual void Apply();
	virtual MaterialObject * GetCurrentMaterial();

private:
	_Color emission;
	_Color specular;
	_Color diffuse;
	_Color ambient;

	float  shininess;

	static GLMaterialObject * m_currMat;

};

#endif