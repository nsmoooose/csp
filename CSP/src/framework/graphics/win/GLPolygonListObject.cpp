#ifdef OGL

#include <stdlib.h>
#include <memory.h>
#include <windows.h>
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>		// Header File For The Glaux Library

#include <graphics\typesgraphics.h>
#include "graphics/gl_globals.h"

#include "GLPolygonListObject.h"

extern GLStateMachine	statesGL;

///////////////////////////////////////////////////

/** \class GLPolygonListObject
 */

GLPolygonListObject::GLPolygonListObject(StandardGraphics * pDisplay)
	: PolygonListObject( pDisplay)
{
	m_pVertices = NULL;
	m_pNormals = NULL;
	m_pTextureCoords = NULL;
	m_iIndices = NULL;
	m_iCount = 0;
	m_PolygonType = -1;  
	m_iIndexed = 0;
	m_VertexFormat = 0;

}

void GLPolygonListObject::Create(PrimitiveTypes type, int vertexCount, _GraphicsVector * v)
{
	m_PolygonType = ConvertTypes(type);
	m_pTextureCoords = NULL;
	m_iCount = vertexCount;
	m_pVertices = (float *)malloc(sizeof(GLfloat) * vertexCount * 3);
	memset(m_pVertices, 0x18 , sizeof(GLfloat) * vertexCount * 3);
	m_pNormals = NULL;
	m_VertexFormat = GLStateMachine::VFORMAT;

	for(int i = 0; i<vertexCount; i++)
	{
		memcpy(m_pVertices+i*3,   (void *)&v[i].x, sizeof(GLfloat) * 1);
		memcpy(m_pVertices+i*3+1, (void *)&v[i].y, sizeof(GLfloat) * 1);
		memcpy(m_pVertices+i*3+2, (void *)&v[i].z, sizeof(GLfloat) * 1);
	}


}

void GLPolygonListObject::Create(PrimitiveTypes type, int vertexCount, _GraphicsVector * v, _GraphicsVector *n)
{
	m_PolygonType = ConvertTypes(type);
	m_pTextureCoords = NULL;
	m_iCount = vertexCount;
	m_pVertices = (float *)malloc(sizeof(GLfloat) * vertexCount * 3);
	memset(m_pVertices, 0x18 , sizeof(GLfloat) * vertexCount * 3);
	m_pNormals = (float *)malloc(sizeof(GLfloat) * vertexCount * 3);
	memset(m_pNormals, 0x18, sizeof(GLfloat) * vertexCount * 3);
	m_VertexFormat = GLStateMachine::VNFORMAT;

	for(int i = 0; i<vertexCount; i++)
	{
		memcpy(m_pVertices+i*3,   (void *)&v[i].x, sizeof(GLfloat) * 1);
		memcpy(m_pVertices+i*3+1, (void *)&v[i].y, sizeof(GLfloat) * 1);
		memcpy(m_pVertices+i*3+2, (void *)&v[i].z, sizeof(GLfloat) * 1);

		memcpy(m_pNormals+i*3,   (void *)&n[i].x, sizeof(GLfloat) * 1);
		memcpy(m_pNormals+i*3+1, (void *)&n[i].y, sizeof(GLfloat) * 1);
		memcpy(m_pNormals+i*3+2, (void *)&n[i].z, sizeof(GLfloat) * 1);

	}

}

void GLPolygonListObject::Create(PrimitiveTypes type, int vertexCount, _NormalVertexStruct *v)
{
	m_PolygonType = ConvertTypes(type);
	m_iCount = vertexCount;
	m_pVertices = (float *)malloc(sizeof(GLfloat) * vertexCount * 3);
	memset(m_pVertices, 0x18 , sizeof(GLfloat) * vertexCount * 3);
	m_pNormals = (float *)malloc(sizeof(GLfloat) * vertexCount * 3);
	memset(m_pNormals, 0x18, sizeof(GLfloat) * vertexCount * 3);
	m_pTextureCoords = (float *)malloc(sizeof(GLfloat) * vertexCount * 2);
	memset(m_pTextureCoords, 0x18, sizeof(GLfloat) * vertexCount * 2);
	m_VertexFormat = GLStateMachine::VNTFORMAT;

	for(int i = 0; i<vertexCount; i++)
	{
		memcpy(m_pVertices+i*3,   (void *)&v[i].p.x, sizeof(GLfloat));
		memcpy(m_pVertices+i*3+1, (void *)&v[i].p.y, sizeof(GLfloat));
		memcpy(m_pVertices+i*3+2, (void *)&v[i].p.z, sizeof(GLfloat));

		memcpy(m_pNormals+i*3,   (void *)&v[i].n.x, sizeof(GLfloat));
		memcpy(m_pNormals+i*3+1, (void *)&v[i].n.y, sizeof(GLfloat));
		memcpy(m_pNormals+i*3+2, (void *)&v[i].n.z, sizeof(GLfloat));

		memcpy(m_pTextureCoords+i*2,   (void *)&v[i].tu, sizeof(GLfloat));
		memcpy(m_pTextureCoords+i*2+1, (void *)&v[i].tv, sizeof(GLfloat));

	}


}

void GLPolygonListObject::SetIndices(int indexCount, int indexSize, void * indices)
{
  m_iIndexed = 1;
  m_iIndexType = ConvertIndexTypes(indexSize);
  m_iIndexSize = indexSize;
  m_iIndexCount = indexCount;
  m_iIndices = (GLuint*)malloc(sizeof(indexSize)*indexCount);
  memset((void*)m_iIndices, 0x18, sizeof(indexSize)*indexCount);
  memcpy((void*)m_iIndices, (void*)indices, indexCount*sizeof(indexSize));
}


void GLPolygonListObject::Destroy()
{
	free(m_pVertices);
	free(m_pNormals);
	free(m_pTextureCoords);
	m_pVertices = NULL;
	m_pNormals = NULL;
	m_pTextureCoords = NULL;
	m_iCount = 0;
	m_VertexFormat = 0;
}


GLPolygonListObject::~GLPolygonListObject()
{
	free(m_pVertices);
	free(m_pNormals);
	free(m_pTextureCoords);
}

void GLPolygonListObject::Draw()
{

	// set the vertex arrays.
	if (m_VertexFormat == GLStateMachine::VNTFORMAT)
	{
//		if (GLStateMachine::activeArrays != GLStateMachine::VNTFORMAT)
		GLStateMachine::changeArrays(GLStateMachine::VNTFORMAT);

		glVertexPointer(3, GL_FLOAT, 0, m_pVertices);
		glTexCoordPointer(2,GL_FLOAT,  0, m_pTextureCoords);
		glNormalPointer(GL_FLOAT, 0, m_pNormals);


	}

	else if (m_VertexFormat == GLStateMachine::VNFORMAT)
	{
//			if (GLStateMachine::activeArrays != GLStateMachine::VNFORMAT)
		GLStateMachine::changeArrays(GLStateMachine::VNFORMAT);

		glVertexPointer(3, GL_FLOAT, m_iCount, m_pVertices);
		glNormalPointer(GL_FLOAT, m_iCount, m_pNormals);

//glDrawArrays(m_PolygonType, 0, m_iCount);
	}

	else if (m_VertexFormat == GLStateMachine::VFORMAT)
	{
		GLStateMachine::changeArrays(GLStateMachine::VFORMAT);

		glVertexPointer(3, GL_FLOAT, m_iCount, m_pVertices);
			
//glDrawArrays(m_PolygonType, 0, m_iCount);
	}

	// draw the vertices.
	if (m_iIndexed)
	{
		glDrawElements(m_PolygonType, m_iIndexCount, m_iIndexType, m_iIndices);
	}

	else
		glDrawArrays(m_PolygonType, 0, m_iCount);


}

void GLPolygonListObject::SetName(char * name)
{

}

char * GLPolygonListObject::GetName()
{
	return NULL;
}

void GLPolygonListObject::SetMatrix(_Matrix *  mat)
{

}

void GLPolygonListObject::MultiMatrix(_Matrix * mat)
{

}

void GLPolygonListObject::Translate(float X, float Y, float Z)
{

}

void GLPolygonListObject::Rotate(float angle, float x, float y, float z)
{

}

GLenum GLPolygonListObject::ConvertIndexTypes(int indexSize)
{
	switch(indexSize)
	{
		case 1:
		return GL_UNSIGNED_BYTE;
		break;
		
		case 2:
		return GL_UNSIGNED_SHORT;
		break;
		
		case 4:
		return GL_UNSIGNED_INT;
		break;

		default:
		return -1;
	}
}

GLenum GLPolygonListObject::ConvertTypes(int type)
{
	switch(type)
	{
	case FW_POINTS:
		return GL_POINTS;
		break;
	case FW_LINES:
		return GL_LINES;
		break;
	case FW_LINE_STRIP:
		return GL_LINE_STRIP;
		break;
	case FW_LINE_LOOP:
		return GL_LINE_LOOP;
		break;
	case FW_TRIANGLES:
		return GL_TRIANGLES;
		break;
	case FW_TRIANGLE_STRIP:
		return GL_TRIANGLE_STRIP;
		break;
	case FW_TRIANGLE_FAN:
		return GL_TRIANGLE_FAN;
		break;
	case FW_QUADS:
		return GL_QUADS;
		break;
	case FW_QUAD_STRIP:
		return GL_QUAD_STRIP;
		break;
	case FW_POLYGON:
		return GL_POLYGON;
		break;
	default: 
		return 0;
		break;
	}
}

#endif