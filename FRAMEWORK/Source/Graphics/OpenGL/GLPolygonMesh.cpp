///////////////////////////////////////////////////////////////////////////
//
//   GLPolygonMesh  - CSP - http://csp.homeip.net
//
//   coded by Wolverine and GKW
//
///////////////////////////////////////////////////////////////////////////
//   
//  This class represents a PolygonMesh. It implements
//  the StandardPolygonMesh abstract interface class.
//  In this OpenGL implementation the vertex arrays
//  are used to render the mesh which is based on a set
//  vertices and indexes.
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//
///////////////////////////////////////////////////////////////////////////

#ifdef OGL

#include "Framework.h"
#include "GLPolygonMesh.h"
#include "GLGlobals.h"

extern GLStateMachine	statesGL;

static const int BUFFERBLOCK = 1024;
#ifndef INDEXSIZE
#define INDEXSIZE 2
#endif

///////////////////////////////////////////////////

/**
 * \fn GLPolygonMesh::GLPolygonMesh()
 * \brief Constructor for GLPolygonMesh, Member variables are initialized.
 */
GLPolygonMesh::GLPolygonMesh()
{
    CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLPolygonMesh::GLPolygonMesh()");

	p_Matrix = StandardMatrix4::IDENTITY;
	m_pVertices = NULL;
	m_pNormals = NULL;
	m_pTextureCoords = NULL;
        m_pColors = NULL;
	m_iIndices = NULL;
	m_iCount = 0;
	m_PolygonType = 0;  
	m_iIndexed = 0;
	m_VertexFormat = 0;
	m_iIndexType = 0;
	m_iIndexSize = 0;
	m_iMemBufSize = 0;
	m_iIndexCount = 0;

	// VNT vertex is the default for now.
	m_VertexFormat = GLStateMachine::VNTFORMAT;
	m_PolygonType = GL_TRIANGLES;
}

/**
 * \fn GLPolygonMesh::~GLPolygonMesh()
 * \brief Destructor for GLPolygonMesh(), memory buffers are freed.
 */
GLPolygonMesh::~GLPolygonMesh()
{
    CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLPolygonMesh::~GLPolygonMesh()");

	free(m_pVertices);
	free(m_pNormals);
	free(m_pTextureCoords);
	free(m_pColors);
	free(m_iIndices);
}

/**
 * \fn short GLPolygonMesh::Initialize(PrimitiveTypes Type, unsigned short vertexCount, StandardVertex *Vertexes)
 * \brief Initialize a Polygon Mesh to used only coordinates in the vertex.
 * \param Type The type of polygon mesh to use. 
 * \param vertexCount The number of vertices to use.
 * \param Vertices The array of vertices to use. 
 */
short GLPolygonMesh::Initialize(PrimitiveTypes Type, unsigned short vertexCount, StandardVertex *Vertices)
{
    CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLPolygonMesh::Initialize(PrimitiveTypes Type, unsigned short vertexCount, StandardVertex *Vertices)");

	m_PolygonType = ConvertPolygonTypes(Type);
	m_iCount = vertexCount;
	m_pColors = NULL;

	m_pVertices = (float *)malloc(sizeof(GLfloat) * vertexCount * 3);
	memset(m_pVertices, 0x18 , sizeof(GLfloat) * vertexCount * 3);

	m_VertexFormat = GLStateMachine::VFORMAT;

	for(int i = 0; i<vertexCount; i++)
	{
		memcpy(m_pVertices+i*3,   (void *)&Vertices[i].p.x, sizeof(GLfloat));
		memcpy(m_pVertices+i*3+1, (void *)&Vertices[i].p.y, sizeof(GLfloat));
		memcpy(m_pVertices+i*3+2, (void *)&Vertices[i].p.z, sizeof(GLfloat));
	}

	return 0;

}

/**
 * \fn short GLPolygonMesh::Initialize(PrimitiveTypes type, unsigned short vertexCount, StandardNTVertex *v)
 * \brief Initialize a PolygonMesh using Coords,Normals and TextureCoords in the Vertex.
 * \param type The type of mesh to use
 * \param vertexCount The number of vertices in the mesh.
 * \param v The vertex array to use.
 */
short GLPolygonMesh::Initialize(PrimitiveTypes type, unsigned short vertexCount, StandardNTVertex *v)
{
    CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLPolygonMesh::Initialize(PrimitiveTypes type, unsigned short vertexCount, StandardNTVertex *v)");

	m_PolygonType = ConvertPolygonTypes(type);
	m_iCount = vertexCount;
	m_pColors = NULL;

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

	return 0;

}

/**
 * \fn short GLPolygonMesh::Initialize(PrimitiveTypes type, unsigned short vertexCount,  StandardTVertex *v)
 * \brief Initialize a PolygonMesh using Coords and TextureCoords in the Vertex.
 * \param type The type of polygon mesh.
 * \param vertexCount The number of vertices in the mesh.
 * \param v The vertex array to use.
 */
short GLPolygonMesh::Initialize(PrimitiveTypes type, unsigned short vertexCount,  StandardTVertex *v)
{
    CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLPolygonMesh::Initialize(PrimitiveTypes type, unsigned short vertexCount,  StandardTVertex *v)" );

	m_PolygonType = ConvertPolygonTypes(type);
	m_iCount = vertexCount;

	m_pVertices = (float *)malloc(sizeof(GLfloat) * vertexCount * 3);
	memset(m_pVertices, 0x18 , sizeof(GLfloat) * vertexCount * 3);

	m_pNormals = NULL;
	m_pColors = NULL;

	m_pTextureCoords = (float *)malloc(sizeof(GLfloat) * vertexCount * 2);
	memset(m_pTextureCoords, 0x18, sizeof(GLfloat) * vertexCount * 2);

	m_VertexFormat = GLStateMachine::VTFORMAT;

	for(int i = 0; i<vertexCount; i++)
	{
		memcpy(m_pVertices+i*3,   (void *)&v[i].p.x, sizeof(GLfloat));
		memcpy(m_pVertices+i*3+1, (void *)&v[i].p.y, sizeof(GLfloat));
		memcpy(m_pVertices+i*3+2, (void *)&v[i].p.z, sizeof(GLfloat));
     
		memcpy(m_pTextureCoords+i*2,   (void *)&v[i].tu, sizeof(GLfloat));
		memcpy(m_pTextureCoords+i*2+1, (void *)&v[i].tv, sizeof(GLfloat));

	}

	return 0;


}

/**
 * \fn short GLPolygonMesh::Initialize(PrimitiveTypes type, unsigned short vertexCount, StandardCVertex * v)
 * \brief Initialize a PolygonMesh using Coords and TextureCoords in the Vertex. Not currently used.
 */
short GLPolygonMesh::Initialize(PrimitiveTypes type, unsigned short vertexCount, StandardCVertex * v)
{
    CSP_LOG(CSP_OPENGL, CSP_TRACE, "short GLPolygonMesh::Initialize(PrimitiveTypes type, unsigned short vertexCount, StandardCVertex * v)" );
    
    return 0;
}

/**
 * \fn void GLPolygonMesh::Uninitialize()
 * \brief resets a PolygonMesh and frees the memory it allocated.
 */
void GLPolygonMesh::Uninitialize()
{
    CSP_LOG(CSP_OPENGL, CSP_TRACE, "void GLPolygonMesh::Uninitialize()" );

	free(m_pVertices);      
	free(m_pNormals);       
	free(m_pTextureCoords); 
	free(m_pColors);
	free(m_iIndices);

	m_pVertices = NULL;
	m_pNormals = NULL;
	m_pTextureCoords = NULL;
	m_iIndices = NULL;
	m_iCount = 0;
	m_PolygonType = 0;  
	m_iIndexed = 0;
	m_VertexFormat = 0;
	m_iIndexType = 0;
	m_iIndexSize = 0;
	m_iMemBufSize = 0;
	m_iIndexCount = 0;

}

/**
 * \fn void GLPolygonMesh::SetIndexBuffer(void *buffer, unsigned long bufferbytecount)
 * \brief Initialize the IndexBuffer portion of a polygon mesh.
 * \param buffer The array of indices to use.
 * \param bufferbytecount The number of bytes in the index buffer.
 */
void GLPolygonMesh::SetIndexBuffer(void *buffer, unsigned long bufferbytecount)
{
    CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLPolygonMesh::SetIndexBuffer(void *buffer, unsigned long bufferbytecount)" );

    m_iIndexed = 1;
    m_iIndexType = GL_UNSIGNED_SHORT;
    m_iIndexSize = 2;
    m_iIndexCount = bufferbytecount >> 1;  // divided by 2.
    m_iIndices = (GLuint*)malloc(bufferbytecount);
    memset((void*)m_iIndices, 0x18, bufferbytecount);
    memcpy((void*)m_iIndices, (void*)buffer, bufferbytecount);

}

/**
 * \fn void GLPolygonMesh::SetIndexBuffer(int indexCount, void * indices)
 * \brief Initialize the IndexBuffer portion of a polygon mesh.
 * \param indexCount The number of indices in the indexBuffer
 * \param indices The array of indices to be used in the indexBuffer.
 */
void GLPolygonMesh::SetIndexBuffer(int indexCount, void * indices)
{
    CSP_LOG(CSP_OPENGL, CSP_TRACE, "void GLPolygonMesh::SetIndexBuffer(int indexCount, void * indices)" );
    m_iIndexed = 1;

    // all indexes will be 2 byte shorts.
    m_iIndexType = GL_UNSIGNED_SHORT;
    m_iIndexSize = 2;
    m_iIndexCount = indexCount;
    m_iIndices = (GLuint*)malloc(INDEXSIZE*indexCount);
    memset((void*)m_iIndices, 0x18, INDEXSIZE*indexCount);
    memcpy((void*)m_iIndices, (void*)indices, indexCount*INDEXSIZE);
}



/** 
 * \fn void GLPolygonMesh::BeginMesh(PrimitiveTypes type)
 * \brief Begin dynamic creation
 * \param type The type of mesh to use.
 * 
 * This routine assumes that the vertexFormat will be VNT. Also an
 * initial block of memory will be allocated to hold BUFFERBLOCK
 * vertices. The addTriangle routine is used to add triangle one
 * at a time to the mesh. The addTriangle will also allocate more
 * memory if needed.
 */
void GLPolygonMesh::BeginMesh(PrimitiveTypes type)
{

    CSP_LOG(CSP_OPENGL, CSP_TRACE, "void GLPolygonMesh::BeginMesh(PrimitiveTypes type)" );

	free(m_pVertices);      
	free(m_pNormals);       
	free(m_pTextureCoords); 

	m_PolygonType = ConvertPolygonTypes(type);
	m_VertexFormat = GLStateMachine::VNTFORMAT;

	m_pVertices = NULL;
	m_pNormals = NULL;
	m_pTextureCoords = NULL;
	m_iIndices = NULL;
	m_iCount = 0;
	m_iIndexed = 0;
	m_iIndexType = 0;
	m_iIndexSize = 0;
	m_iMemBufSize = 0;
	m_iIndexCount = 0;

	// initially alloc some memory
	m_pVertices = (float *)malloc(sizeof(GLfloat) * BUFFERBLOCK * 3);
	memset(m_pVertices, 0x18 , sizeof(GLfloat) * BUFFERBLOCK * 3);
	m_pNormals = (float *)malloc(sizeof(GLfloat) * BUFFERBLOCK * 3);
	memset(m_pNormals, 0x18, sizeof(GLfloat) * BUFFERBLOCK * 3);
	m_pTextureCoords = (float *)malloc(sizeof(GLfloat) * BUFFERBLOCK * 2);
	memset(m_pTextureCoords, 0x18, sizeof(GLfloat) * BUFFERBLOCK * 2);
	m_iMemBufSize = BUFFERBLOCK;
	m_iCount = 0;

}

/** 
 * \fn void GLPolygonMesh::EndMesh()
 * \brief End dynamic creation, this routine is required to be compatible with the DirectX version.
 */
void GLPolygonMesh::EndMesh()
{
    CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLPolygonMesh::EndMesh()" );
}



/**
 * \fn void GLPolygonMesh::AddTriangle(StandardNTVertex & vertex1, StandardNTVertex & vertex2, StandardNTVertex & vertex3)
 * \brief This method adds a triangle during dynamic buffer creation.
 * \param vertex1 The first vertex in the triangle.
 * \param vertex2 The second vertex in the triangle.
 * \param vertex3 The third vertex in the triangle.
 */
void GLPolygonMesh::AddTriangle(StandardNTVertex & vertex1,
								StandardNTVertex & vertex2,
								StandardNTVertex & vertex3)
{
    CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLPolygonMesh::AddTriangle()" );

	AddVertex(vertex1);
	AddVertex(vertex2);
	AddVertex(vertex3);
}

/**
 * \fn void GLPolygonMesh::AddVertex(StandardNTVertex & v)
 * \brief This method adds a vertex during dynamic buffer creation.
 * \param v The vertex to add to the buffer.
 */
void GLPolygonMesh::AddVertex(StandardNTVertex & v)
{


	// add the vertex to the tail. if we are out of space add more.
	if (m_iCount >= m_iMemBufSize)
	{
		m_pVertices = (float *)realloc(m_pVertices, sizeof(GLfloat) * (BUFFERBLOCK+m_iMemBufSize)*3);
		memset(m_pVertices, 0x18 , sizeof(GLfloat) * (BUFFERBLOCK+m_iMemBufSize)*3);
		m_pNormals = (float *)realloc(m_pNormals, sizeof(GLfloat) * (BUFFERBLOCK+m_iMemBufSize)*3);
		memset(m_pNormals, 0x18, sizeof(GLfloat) * (BUFFERBLOCK+m_iMemBufSize)*3);
		m_pTextureCoords = (float *)realloc(m_pTextureCoords, sizeof(GLfloat) * (BUFFERBLOCK+m_iMemBufSize)*2);
		memset(m_pTextureCoords, 0x18, sizeof(GLfloat) * (BUFFERBLOCK+m_iMemBufSize)*2);
		m_iMemBufSize += BUFFERBLOCK;
	}

	*(m_pVertices+m_iCount*3)   = v.p.x;
	*(m_pVertices+m_iCount*3+1) = v.p.y;
	*(m_pVertices+m_iCount*3+2) = v.p.z;

	*(m_pNormals+m_iCount*3)    = v.n.x;
	*(m_pNormals+m_iCount*3+1)  = v.n.y;
	*(m_pNormals+m_iCount*3+2)  = v.n.z;

	*(m_pTextureCoords+m_iCount*2)    = v.tu;
	*(m_pTextureCoords+m_iCount*2+1)  = v.tv;

	m_iCount++;


}

/**
 * \fn unsigned long  GLPolygonMesh::Draw()
 * \brief Renders the Polygon Mesh to the Graphics surface.
 * \return The number of polygons rendered.
 *
 * This routine sets the proper combination of client arrays
 * to use for the particular vertex type. Also Indexs are used
 * if this mesh is indexed. Finally this routine sets a matrix 
 * for this mesh which is then poped at the end.
 */
unsigned long  GLPolygonMesh::Draw()
{
    CSP_LOG(CSP_OPENGL, CSP_TRACE, "unsigned long  GLPolygonMesh::Draw()" );

	unsigned long renderedPolygonCount = 0;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixf((float*)p_Matrix);

	// set the vertex arrays.
	if (m_VertexFormat == GLStateMachine::VNTFORMAT)
	{
		GLStateMachine::changeArrays(GLStateMachine::VNTFORMAT);

		glVertexPointer(3, GL_FLOAT, 0, m_pVertices);
		glTexCoordPointer(2,GL_FLOAT,  0, m_pTextureCoords);
		glNormalPointer(GL_FLOAT, 0, m_pNormals);

	}

	else if (m_VertexFormat == GLStateMachine::VNFORMAT)
	{
		GLStateMachine::changeArrays(GLStateMachine::VNFORMAT);

		glVertexPointer(3, GL_FLOAT, 0, m_pVertices);
		glNormalPointer(GL_FLOAT, 0, m_pNormals);

	}

	else if (m_VertexFormat == GLStateMachine::VFORMAT)
	{

		GLStateMachine::changeArrays(GLStateMachine::VFORMAT);
		glVertexPointer(3, GL_FLOAT, 0, m_pVertices);
			
	}

	else if (m_VertexFormat == GLStateMachine::VTFORMAT)
	{
		GLStateMachine::changeArrays(GLStateMachine::VTFORMAT);
		glVertexPointer(3, GL_FLOAT, 0, m_pVertices);
		glTexCoordPointer(2,GL_FLOAT,  0, m_pTextureCoords);
	}

	else if (m_VertexFormat == GLStateMachine::VCFORMAT)
	{
		GLStateMachine::changeArrays(GLStateMachine::VCFORMAT);
		glVertexPointer(3, GL_FLOAT, 0, m_pVertices);
        // TO DO glColorPointer(
	}

	// draw the vertices.
	if (m_iIndexed)
	{       
        glDrawElements(m_PolygonType, m_iIndexCount , m_iIndexType, m_iIndices);
		renderedPolygonCount = m_iIndexCount/3;
	}
	else
	{
		glDrawArrays(m_PolygonType, 0, m_iCount);
		renderedPolygonCount = m_iCount/3;
	}

	glPopMatrix();

    CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLPolygonMesh - " <<
        renderedPolygonCount << " Polygons rendered" );

	return renderedPolygonCount;
}

/** 
 * \fn void GLPolygonMesh::SetMatrix(const StandardMatrix4 & mat)
 * \brief Set the matrix for the mesh.
 * \param mat The matrix to set for this mesh.
 */
void GLPolygonMesh::SetMatrix(const StandardMatrix4 & mat)
{
    CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLPolygonMesh::SetMatrix(const StandardMatrix4 & mat)" );
    p_Matrix = mat;
}

/** 
 * \fn void GLPolygonMesh::MultiMatrix(const StandardMatrix4 & mat)
 * \brief Multiplies the matrix for the mesh by a matrix.
 * \param mat The matrix to multiply the meshes matrix..
 */
void GLPolygonMesh::MultiMatrix(const StandardMatrix4 & mat)
{
    CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLPolygonMesh::MultiMatrix(const StandardMatrix4 & mat)" );
    p_Matrix *= mat;
}


/** 
 * \fn void GLPolygonMesh::SetMatrix(const StandardMatrix4 * mat)
 * \brief Set the matrix for the mesh.
 * \param mat The matrix to set for this mesh.
 */
void GLPolygonMesh::SetMatrix(StandardMatrix4 *  mat)
{
    CSP_LOG(CSP_OPENGL, CSP_TRACE, "void GLPolygonMesh::SetMatrix(StandardMatrix4 *  mat)" );
    p_Matrix = *mat;

}

/** 
 * \fn void GLPolygonMesh::MultiMatrix(const StandardMatrix4 * mat)
 * \brief Multiplies the matrix for the mesh by a matrix.
 * \param mat The matrix to multiply the meshes matrix..
 */
void GLPolygonMesh::MultiMatrix(StandardMatrix4 * mat)
{
    CSP_LOG(CSP_OPENGL, CSP_TRACE, "void GLPolygonMesh::MultiMatrix(StandardMatrix4 * mat)" );
    p_Matrix *= *mat;

}

void GLPolygonMesh::Translate(float X, float Y, float Z)
{

}

void GLPolygonMesh::Rotate(float angle, float x, float y, float z)
{

}

/**
 * \fn void GLPolygonMesh::SetPos(float x, float y, float z)
 * \brief Sets the position of the mesh. This is the same as using a translation matrix with SetMatrix.
 * \param x The x position of the mesh.
 * \param y The y position of the mesh.
 * \param z The z position of the mesh.
 */
void GLPolygonMesh::SetPos(float x, float y, float z)
{
  p_Matrix = TranslationMatrix4(x, y, z);

}


/**
 * \fn GLenum GLPolygonMesh::ConvertPolygonTypes(int type)
 * \brief Convert the generic framework polygon types to the specific OpenGL ones.
 * \param the Framework polygon type to convert.
 */
GLenum GLPolygonMesh::ConvertPolygonTypes(int type)
{
    CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLPolygonMesh::ConvertPolygonTypes(int type)" );

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
