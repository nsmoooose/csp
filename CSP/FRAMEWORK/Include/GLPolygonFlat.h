#ifndef __GLPOLYGONFLAT_H__
#define __GLPOLYGONFLAT_H__

#include "Framework.h"

class GLPolygonFlat : public StandardPolygonFlat
{

private:
  unsigned short           p_usPointCount;
  unsigned short           p_usPolygonCount;

  int                      m_iIndexed;
  _GRAPHICS_INIT_STRUCT   *p_Init;
  _Rect                    p_Coords;

  GLfloat * m_pVertices;
  GLfloat * m_pNormals;
  GLfloat * m_pTextureCoords;
  GLvoid  * m_pColors;
  GLuint * m_iIndices;
  GLuint m_iCount;
  GLenum m_PolygonType;
  int m_VertexFormat;
  int m_iIndexCount;
  int m_iIndexType;
  int m_iIndexSize;
  int m_iMemBufSize;

  StandardMatrix4          p_Matrix;

  StandardGraphics        *p_Graphics;

public:

  GLPolygonFlat(StandardGraphics *Graphics);

  virtual ~GLPolygonFlat() {};

  // functions

  short             Initialize(float left, float top, float right, float bottom,
                                        bool Absolute, StandardColor *Color);

  short             Initialize(PrimitiveTypes   type,
                                       bool Absolute,
                                       unsigned short    VertexCount, 
                                       StandardTLVertex  *Vertexes);
  
  void              Uninitialize();

  void              SetIndexBuffer(void *buffer, unsigned long bufferbytecount);
  unsigned long     Draw();
  _Rect*            GetCoords();

  GLenum ConvertPolygonTypes(int type);


};


#endif

