#ifdef OGL

#include <stdlib.h>
#include <memory.h>

#include "Framework.h"
#include "GLGlobals.h"
#include "GLPolygonBillboard.h"

extern GLStateMachine	statesGL;

static const int BUFFERBLOCK = 1024;
#ifndef INDEXSIZE
#define INDEXSIZE 2
#endif


GLPolygonBillboard::GLPolygonBillboard()
{

  fprintf(stdout, "GLPolygonBillboard::GLPolygonBillboard()\n");

  p_Matrix = StandardMatrix4::IDENTITY;
  p_Camera = 0;

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

  // VNT vertex is the default for now.
  m_VertexFormat = GLStateMachine::VNTFORMAT;
  m_PolygonType = GL_TRIANGLES;


  p_YAxisLocked = false;

}

GLPolygonBillboard::~GLPolygonBillboard()
{
  Uninitialize();
}

short GLPolygonBillboard::Initialize(StandardCamera *Camera, float Width, float Height, bool YAxisLocked)
{

  fprintf(stdout, "GLPolygonBillboard::Initialize()\n");

  p_Camera = Camera;
  p_YAxisLocked = YAxisLocked;

  StandardNTVertex Billboard[4];
  _Index  Index[6];

  int vertexCount = 4;
  int indexCount = 6;


  // --- fill out billboard

  Billboard[0].Initialize(-Width/2, Height, 0,  0,1,0,  0,0);
  Billboard[1].Initialize( Width/2, Height, 0,  0,1,0,  1,0);
  Billboard[2].Initialize( Width/2,      0, 0,  0,1,0,  1,1);
  Billboard[3].Initialize(-Width/2,      0, 0,  0,1,0,  0,1);

  Index[0] = 0;
  Index[1] = 1;
  Index[2] = 2;
  Index[3] = 0;
  Index[4] = 2;
  Index[5] = 3;

  m_PolygonType = GL_TRIANGLES;
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
      memcpy(m_pVertices+i*3,   (void *)&Billboard[i].p.x, sizeof(GLfloat));
      memcpy(m_pVertices+i*3+1, (void *)&Billboard[i].p.y, sizeof(GLfloat));
      memcpy(m_pVertices+i*3+2, (void *)&Billboard[i].p.z, sizeof(GLfloat));
      
      memcpy(m_pNormals+i*3,   (void *)&Billboard[i].n.x, sizeof(GLfloat));
      memcpy(m_pNormals+i*3+1, (void *)&Billboard[i].n.y, sizeof(GLfloat));
      memcpy(m_pNormals+i*3+2, (void *)&Billboard[i].n.z, sizeof(GLfloat));

      memcpy(m_pTextureCoords+i*2,   (void *)&Billboard[i].tu, sizeof(GLfloat));
      memcpy(m_pTextureCoords+i*2+1, (void *)&Billboard[i].tv, sizeof(GLfloat));

    }

  m_iIndexed = 1;
  m_iIndexType = GL_UNSIGNED_SHORT;
  m_iIndexSize = 2;
  m_iIndexCount = indexCount;
  m_iIndices = (GLuint*)malloc(INDEXSIZE*indexCount);
  memset((void*)m_iIndices, 0x18, INDEXSIZE*indexCount);
  memcpy((void*)m_iIndices, (void*)Index, indexCount*INDEXSIZE);


  return 0;
}

void GLPolygonBillboard::SetPos(float x, float y, float z)
{
  p_Matrix = TranslationMatrix4(x,y,z);

  return;
}

unsigned long GLPolygonBillboard::Draw()
{
#ifdef TRACE_GRAPHICS
  fprintf(stdout, "GLPolygonBillboard::Draw()\n");
#endif


  StandardMatrix4 tempCamera = *p_Camera->GetCameraInverseMatrix();
  if (p_YAxisLocked == true)
    {
      StandardVector3 tempVec1, tempVec2, tempForward, tempUp, tempRight;

      tempVec1.x = tempCamera.rowcol[3][0];
      tempVec1.y = 0;
      tempVec1.z = tempCamera.rowcol[3][2];

      tempVec2.x = p_Matrix.rowcol[3][0];
      tempVec2.y = 0;
      tempVec2.z = p_Matrix.rowcol[3][2];

      tempForward - tempVec2 - tempVec1;

      tempForward.Normalize();

      tempUp.x = 0;
      tempUp.y = 1;
      tempUp.z = 0;

      tempRight = Cross(tempUp, tempForward);

      p_Matrix.rowcol[0][0] = tempRight.x;
      p_Matrix.rowcol[0][1] = tempRight.y;
      p_Matrix.rowcol[0][2] = tempRight.z;

      p_Matrix.rowcol[1][0] = tempUp.x;
      p_Matrix.rowcol[1][1] = tempUp.y;
      p_Matrix.rowcol[1][2] = tempUp.z;

      p_Matrix.rowcol[2][0] = tempForward.x;
      p_Matrix.rowcol[2][1] = tempForward.y;
      p_Matrix.rowcol[2][2] = tempForward.z;

    }
  else
    {
      p_Matrix.rowcol[0][0] = tempCamera.rowcol[0][0];
      p_Matrix.rowcol[1][0] = tempCamera.rowcol[0][0];
      p_Matrix.rowcol[2][0] = tempCamera.rowcol[0][0];

      p_Matrix.rowcol[0][1] = tempCamera.rowcol[0][1];
      p_Matrix.rowcol[1][1] = tempCamera.rowcol[1][1];
      p_Matrix.rowcol[2][1] = tempCamera.rowcol[2][1];

      p_Matrix.rowcol[0][2] = tempCamera.rowcol[0][2];
      p_Matrix.rowcol[1][2] = tempCamera.rowcol[1][2];
      p_Matrix.rowcol[2][2] = tempCamera.rowcol[2][2];

    }

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glMultMatrixf((float*)p_Matrix);

  GLStateMachine::changeArrays(GLStateMachine::VNTFORMAT);

  glVertexPointer(3, GL_FLOAT, 0, m_pVertices);
  glTexCoordPointer(2,GL_FLOAT,  0, m_pTextureCoords);
  glNormalPointer(GL_FLOAT, 0, m_pNormals);



  glDrawElements(m_PolygonType, m_iIndexCount, m_iIndexType, m_iIndices);

  glPopMatrix();

  return 2;
}

void GLPolygonBillboard::Uninitialize()
{


  return;
}

#endif

