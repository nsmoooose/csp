#ifndef __POLYGONLISTOBJECT_H__
#define __POLYGONLISTOBJECT_H__

#include "graphics/typesgraphics.h"

#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>		// Header File For The Glaux Library


/** \class PolygonListObject
 * This class simple list of triangles
 */


class GLPolygonListObject : public PolygonListObject
{
public:
	GLPolygonListObject(StandardGraphics * m_pDisplay);
	virtual ~GLPolygonListObject();

	virtual void Create(PrimitiveTypes type, int vertexCount, _GraphicsVector * v);
	virtual void Create(PrimitiveTypes type, int vertexCount, _GraphicsVector * v, _GraphicsVector *n);
	virtual void Create(PrimitiveTypes type, int vertexCount, _NormalVertexStruct *v);
	virtual void SetIndices(int indexCount, int indexSize, void * indices);

	virtual void Destroy();
	virtual void Draw();

	virtual void SetName(char * name);
	virtual char * GetName();

	virtual void SetMatrix(_Matrix *  mat);
	virtual void MultiMatrix(_Matrix * mat);
	virtual void Translate(float X, float Y, float Z);
	virtual void Rotate(float angle, float x, float y, float z);


private:

	GLenum ConvertTypes(int type);
	GLenum ConvertIndexTypes(int indexSize);

	GLfloat * m_pVertices;
	GLfloat * m_pNormals;
	GLfloat * m_pTextureCoords;
	GLuint * m_iIndices;
	GLuint m_iCount;
	GLenum m_PolygonType;
	int m_VertexFormat;
	int m_iIndexed;
	int m_iIndexCount;
	int m_iIndexType;
	int m_iIndexSize;

};


#endif

