#ifdef D3D

#include "Framework.h"


class WinDXPolygonFlat : public StandardPolygonFlat
{

private:

  StandardGraphics        *p_Graphics;
  IDirect3DDevice8        *p_Device;

  IDirect3DVertexBuffer8  *p_Buffer;
  IDirect3DIndexBuffer8   *p_Index;

  unsigned short           p_usPointCount;
  unsigned short           p_usPolygonCount;

  int                      m_iIndexed;
  _GRAPHICS_INIT_STRUCT   *p_Init;
  _Rect                    p_Coords;

  D3DPRIMITIVETYPE         m_polygonType;
  D3DFORMAT                ConvertIndexTypes(int indexType);
  D3DPRIMITIVETYPE         ConvertPolygonTypes(PrimitiveTypes polygonType);

  unsigned short           PointsPerPolygon(PrimitiveTypes polygonType);

  PrimitiveTypes           p_Type;

public:

  WinDXPolygonFlat(StandardGraphics *Graphics);
  ~WinDXPolygonFlat();

  // functions
  short             Initialize(PrimitiveTypes   type,
                               bool             Absolute,
                               unsigned short    VertexCount, 
                               StandardTLVertex  *Vertexes);
  
  short             Initialize(float left, float top, float right, float bottom,
                               bool Absolute, StandardColor *Color);

  void              Uninitialize();

  void              SetIndexBuffer(void *buffer, unsigned long bufferbytecount);
  unsigned long     Draw();
  _Rect*            GetCoords();


};




#endif