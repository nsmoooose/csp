#ifndef __TYPESLOD_H__
#define __TYPESLOD_H__

#include "Framework.h"

class StandardLOD
{
  private:

    StandardPolygonMesh       *p_NearMesh;
    StandardPolygonBillboard  *p_NearBillboard;

    StandardPolygonMesh       *p_MidMesh;
    StandardPolygonBillboard  *p_MidBillboard;

    StandardPolygonMesh       *p_FarMesh;
    StandardPolygonBillboard  *p_FarBillboard;

    float                      p_NearZ;
    float                      p_FarZ;
    float                      p_EndZ;

    StandardCamera            *p_Camera;
    StandardVector3            p_Pos;

  public:

    StandardLOD();
    ~StandardLOD();

    short Initialize(StandardCamera *Camera);
    void  Uninitialize();

    void SetNear(StandardPolygonMesh *Mesh);
    void SetNear(StandardPolygonBillboard *Bill);
    
    void SetMidStart(float Z);
    void SetMid(StandardPolygonMesh *Mesh);
    void SetMid(StandardPolygonBillboard *Bill);
    void SetMidEnd(float Z);

    void SetFar(StandardPolygonMesh *Mesh);
    void SetFar(StandardPolygonBillboard *Bill);
    void SetFarEnd(float Z);

    void SetPos(float x, float y, float z);
    
    unsigned long Draw();

};

#endif
