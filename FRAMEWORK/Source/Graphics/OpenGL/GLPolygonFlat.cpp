#ifdef OGL

#include "Framework.h"
#include "GLGlobals.h"
#include <GLPolygonFlat.h>

// functions

static const int BUFFERBLOCK = 1024;

#ifndef INDEXSIZE
#define INDEXSIZE 2
#endif

GLPolygonFlat::GLPolygonFlat(StandardGraphics *Graphics)
{
#ifdef TRACE_GRAPHICS
  fprintf(stdout, "GLPolygonFlat::GLPolygonFlat()\n");
#endif

  p_Graphics = Graphics;
  p_Init = p_Graphics->GetInit();


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
  
  m_VertexFormat = GLStateMachine::VCFORMAT;
  m_PolygonType = GL_TRIANGLES;


}

short GLPolygonFlat::Initialize(float left, float top, float right, float bottom,
                                        bool Absolute, StandardColor *Color)
{
  m_PolygonType = ConvertPolygonTypes(FW_TRIANGLES);

  p_Coords.x1 = left;
  p_Coords.y1 = top;
  p_Coords.x2 = right;
  p_Coords.y2 = bottom;

  p_usPointCount = 4;

  long int bufferbytecount = 4 * sizeof(StandardTLVertex);
  
  unsigned short x;

  p_usPolygonCount = 2;

  StandardTLVertex  Vertexes[4];

  Vertexes[0].Initialize( left,  top,  Color->r,Color->g,Color->b,Color->a);
  Vertexes[1].Initialize( right,  top,  Color->r,Color->g,Color->b,Color->a);
  Vertexes[2].Initialize( left,  bottom,  Color->r,Color->g,Color->b,Color->a);
  Vertexes[3].Initialize( right,  bottom,  Color->r,Color->g,Color->b,Color->a);
  
  if (Absolute == false)
  {
    // convert coords
    for(x = 0; x < 4; x++)
    {
      Vertexes[x].p.x = (Vertexes[x].p.x * p_Init->Width);
      Vertexes[x].p.y = (Vertexes[x].p.y * p_Init->Height);
    }
  }

  m_pVertices = (float *)malloc(sizeof(GLfloat) * 4 * 3);
  memset(m_pVertices, 0x18 , sizeof(GLfloat) * 4 * 3);

  m_VertexFormat = GLStateMachine::VFORMAT;

  for(int i = 0; i<4; i++)
    {
      memcpy(m_pVertices+i*3,   (void *)&Vertexes[i].p.x, sizeof(GLfloat));
      memcpy(m_pVertices+i*3+1, (void *)&Vertexes[i].p.y, sizeof(GLfloat));
      memcpy(m_pVertices+i*3+2, (void *)&Vertexes[i].p.z, sizeof(GLfloat));
     
      //      memcpy(m_pTextureCoords+i*2,   (void *)&Vertexes[i].tu, sizeof(GLfloat));
      //      memcpy(m_pTextureCoords+i*2+1, (void *)&Vertexes[i].tv, sizeof(GLfloat));
      
    }


  short Indx[6];

  Indx[0] = 0;
  Indx[1] = 1;
  Indx[2] = 2;
  Indx[3] = 1;
  Indx[4] = 3;
  Indx[5] = 2;

  SetIndexBuffer(Indx, sizeof(Indx));

  return 0;
}

short GLPolygonFlat::Initialize(PrimitiveTypes   type,
                                       bool Absolute,
                                       unsigned short    VertexCount, 
                                       StandardTLVertex  *Vertexes)
{
#ifdef TRACE_GRAPHICS 
  fprintf(stdout, "GLPolygonFlat::Initialize(PrimitiveTypes type, unsigned short vertexCount,  StandardTVertex *v)\n");
#endif

  m_PolygonType = ConvertPolygonTypes(type);
  m_iCount = VertexCount;

  m_pVertices = (float *)malloc(sizeof(GLfloat) * VertexCount * 3);
  memset(m_pVertices, 0x18 , sizeof(GLfloat) * VertexCount * 3);

  m_pNormals = NULL;
  m_pColors = NULL;

  m_pTextureCoords = (float *)malloc(sizeof(GLfloat) * VertexCount * 2);
  memset(m_pTextureCoords, 0x18, sizeof(GLfloat) * VertexCount * 2);

  m_VertexFormat = GLStateMachine::VFORMAT;

  for(int i = 0; i<VertexCount; i++)
    {
      memcpy(m_pVertices+i*3,   (void *)&Vertexes[i].p.x, sizeof(GLfloat));
      memcpy(m_pVertices+i*3+1, (void *)&Vertexes[i].p.y, sizeof(GLfloat));
      memcpy(m_pVertices+i*3+2, (void *)&Vertexes[i].p.z, sizeof(GLfloat));
     
      
    }

  return 0;

}
  
void GLPolygonFlat::Uninitialize()
{

}

void GLPolygonFlat::SetIndexBuffer(void *buffer, unsigned long bufferbytecount)
{
#ifdef TRACE_GRAPHICS
  fprintf(stdout, "GLPolygonFlat::SetIndexBuffer(int indexCount, void * indices)\n");
#endif
  m_iIndexed = 1;
  m_iIndexType = GL_UNSIGNED_SHORT;
  m_iIndexSize = 2;
  m_iIndexCount = bufferbytecount >> 1;  // divide by 2
  m_iIndices = (GLuint*)malloc(bufferbytecount);
  memset((void*)m_iIndices, 0x18, bufferbytecount);
  memcpy((void*)m_iIndices, (void*)buffer, bufferbytecount);

}

unsigned long GLPolygonFlat::Draw()
{
#ifdef TRACE_GRAPHICS
  fprintf(stdout, "GLPolygonFlat::Draw()\n");
#endif

	unsigned long renderedPolygonCount = 0;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixf((float*)p_Matrix);

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
		glDrawElements(m_PolygonType, m_iIndexCount, m_iIndexType, m_iIndices);
		renderedPolygonCount = m_iIndexCount/3;
	}

	else
	{
		glDrawArrays(m_PolygonType, 0, m_iCount);
		renderedPolygonCount = m_iCount/3;
	}

	glPopMatrix();
#ifdef TRACE_GRAPHICS
	fprintf(stdout, "GLPolygonFlat::Draw() - %d Polygons rendered\n", renderedPolygonCount);
#endif

	return renderedPolygonCount;
}
  
_Rect* GLPolygonFlat::GetCoords()
{
  return NULL;
}


GLenum GLPolygonFlat::ConvertPolygonTypes(int type)
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
