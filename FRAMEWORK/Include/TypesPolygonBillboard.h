#ifndef __TYPESPOLYGONBILLBOARD_H__
#define __TYPESPOLYGONBILLBOARD_H__

#include "Framework.h"

class StandardPolygonBillboard
{

private:


public:

  virtual ~StandardPolygonBillboard() {};

  // functions
  virtual short             Initialize(StandardCamera *Camera, float Width, float Height, bool YAxisLocked) = 0;

  virtual void              Uninitialize() = 0;

  virtual unsigned long     Draw() = 0;
  virtual void              SetPos(float x, float y, float z) = 0;


};


#endif
