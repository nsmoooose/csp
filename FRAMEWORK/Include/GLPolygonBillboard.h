#ifndef __POLYGONBILLBOARD
#define __POLYGONBILLBOARD

#ifdef OGL

#include "Framework.h"

class GLPolygonBillboard : public StandardPolygonBillboard
{

private:

  StandardMatrix4          p_Matrix;
  StandardCamera          *p_Camera;

  bool                     p_YAxisLocked;

  float p_Width;
  float p_Height;

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
  int m_iMemBufSize;


public:

  GLPolygonBillboard();
  ~GLPolygonBillboard();

  // functions
  short             Initialize(StandardCamera *Camera, float Width, float Height, bool AxisLocked);

  void              Uninitialize();

  unsigned long     Draw();
  void              SetPos(float x, float y, float z);


};
#endif

#endif
