#ifdef D3D

#include "Framework.h"


class WinDXPolygonMesh : public StandardPolygonMesh
{

private:

  StandardGraphics        *p_Graphics;
  IDirect3DDevice8        *p_Device;

  IDirect3DVertexBuffer8  *p_Buffer;
  IDirect3DIndexBuffer8   *p_Index;

  unsigned short           p_usPointCount;
  unsigned short           p_usPolygonCount;
  
  StandardMatrix4          p_Matrix;

  int                      m_iIndexCount;
  int                      m_iIndexed;
  int                      m_iIndexBufferByteCount;
  bool                     m_locked;
  void*                    m_pVertices;
  int                      m_iMemBufSize;

  D3DPRIMITIVETYPE         m_polygonType;
  D3DFORMAT                ConvertIndexTypes(int indexType);
  D3DPRIMITIVETYPE         ConvertPolygonTypes(PrimitiveTypes polygonType);
  unsigned short           PointsPerPolygon(PrimitiveTypes polygonType);

  PrimitiveTypes           p_Type;

  int m_iShader;
  int m_iBufferStride;

public:

  WinDXPolygonMesh(StandardGraphics *Graphics);
  ~WinDXPolygonMesh();

  // functions
  short             Initialize(PrimitiveTypes Type, unsigned short vertexCount, StandardVertex *Vertexes);
  short             Initialize(PrimitiveTypes    Type, 
                               unsigned short    vertexCount, 
                               StandardNTVertex  *StandardNTVertex);

  short             Initialize(PrimitiveTypes type, unsigned short vertexCount, StandardTVertex *v);
  short             Initialize(PrimitiveTypes type, unsigned short vertexCount, StandardCVertex *v);

  void              Uninitialize();

  void              BeginMesh(PrimitiveTypes type);
  void              EndMesh();

  void              AddVertex   (StandardNTVertex & vertex);
  void              AddTriangle (StandardNTVertex & vertex1,
	                               StandardNTVertex & vertex2,
								                 StandardNTVertex & vertex3);

  void              SetIndexBuffer(void *buffer, unsigned long indexcount);
  void              SetIndexBuffer(int indexCount, void * indices);
  
  unsigned long     Draw();
  void              SetPos(float x, float y, float z);
  void              SetMatrix(StandardMatrix4 *Matrix);

};
#endif
