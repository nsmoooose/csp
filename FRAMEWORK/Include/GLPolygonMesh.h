#ifndef __POLYGONMESH_H__
#define __POLYGONMESH_H__

/** \class GLPolygonMesh
 * This class simple list of triangles
 */

#include "Framework.h"

class GLPolygonMesh : public StandardPolygonMesh
{
public:
	GLPolygonMesh();
	virtual ~GLPolygonMesh();

    short        Initialize(PrimitiveTypes Type, unsigned short vertexCount, StandardVertex *Vertexes);
    short        Initialize(PrimitiveTypes type, unsigned short vertexCount, StandardNTVertex *v);
    short        Initialize(PrimitiveTypes type, unsigned short vertexCount,  StandardTVertex *v);
    short        Initialize(PrimitiveTypes type, unsigned short vertexCount, StandardCVertex * v);

    void         Uninitialize();

    void         BeginMesh(PrimitiveTypes type);
    void         EndMesh();


    void         AddVertex(StandardNTVertex & vertex);
    void         AddTriangle(StandardNTVertex & vertex1,
		                       StandardNTVertex & vertex2,
				                   StandardNTVertex & vertex3);
    void         SetIndexBuffer(int indexCount, void * indices);
    void         SetIndexBuffer(void *buffer, unsigned long bufferbytecount);

    unsigned long          Draw();
    void         SetPos(float x, float y, float z);

	virtual void SetMatrix(StandardMatrix4 * mat);
	virtual void MultiMatrix(StandardMatrix4 * mat);
	virtual void Translate(float X, float Y, float Z);
	virtual void Rotate(float angle, float x, float y, float z);

    virtual void SetMatrix(const StandardMatrix4 & mat);
    virtual void MultiMatrix(const StandardMatrix4 & mat);





private:

	GLenum ConvertPolygonTypes(int type);

	GLfloat * m_pVertices;
	GLfloat * m_pNormals;
	GLfloat * m_pTextureCoords;
	GLvoid  * m_pColors;
	GLuint * m_iIndices;
	GLuint m_iCount;
	GLenum m_PolygonType;
	int m_VertexFormat;
	int m_iIndexed;
	int m_iIndexCount;
	int m_iIndexType;
	int m_iIndexSize;
	int m_iMemBufSize;

    StandardMatrix4          p_Matrix;


};


#endif

