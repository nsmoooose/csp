#ifndef __TYPESPOLYGONFLAT_H__
#define __TYPESPOLYGONFLAT_H__

#include "Framework.h"

class StandardPolygonFlat
{

private:


public:

  virtual ~StandardPolygonFlat() {};

  // functions

  virtual short             Initialize(float left, float top, float right, float bottom,
                                        bool Absolute, StandardColor *Color) = 0;

  virtual short             Initialize(PrimitiveTypes   type,
                                       bool Absolute,
                                       unsigned short    VertexCount, 
                                       StandardTLVertex  *Vertexes) = 0;
  
  virtual void              Uninitialize() = 0;

  virtual void              SetIndexBuffer(void *buffer, unsigned long bufferbytecount) = 0;
  virtual unsigned long     Draw() = 0;
  virtual _Rect*            GetCoords() = 0;

};


#endif
