#ifdef D3D

#include "Framework.h"


class WinDXPolygonBillboard : public StandardPolygonBillboard
{

private:

  StandardGraphics        *p_Graphics;
  IDirect3DDevice8        *p_Device;

  IDirect3DVertexBuffer8  *p_Buffer;
  IDirect3DIndexBuffer8   *p_Index;
  
  StandardMatrix4          p_Matrix;
  StandardCamera          *p_Camera;

  bool                     p_YAxisLocked;

public:

  WinDXPolygonBillboard(StandardGraphics *Graphics);
  ~WinDXPolygonBillboard();

  // functions
  short             Initialize(StandardCamera *Camera, float Width, float Height, bool YAxisLocked);

  void              Uninitialize();

  unsigned long     Draw();
  void              SetPos(float x, float y, float z);


};
#endif