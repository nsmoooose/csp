#include "TypesLOD.h"

StandardLOD::StandardLOD()
{

  p_NearMesh = 0;
  p_NearBillboard = 0; 

  p_MidMesh = 0;
  p_MidBillboard = 0; 

  p_FarMesh = 0;
  p_FarBillboard = 0; 

  p_NearZ = 0;
  p_FarZ = 0;
  p_EndZ = 0;

  p_Pos.x = 0;
  p_Pos.y = 0;
  p_Pos.z = 0;
}

StandardLOD::~StandardLOD()
{
  Uninitialize();
}

short StandardLOD::Initialize(StandardCamera *Camera)
{
  p_Camera = Camera;

  return 0;
}

void StandardLOD::Uninitialize()
{
  return;
}

void StandardLOD::SetNear(StandardPolygonMesh *Mesh)
{
  p_NearMesh = Mesh;
}

void StandardLOD::SetNear(StandardPolygonBillboard *Bill)
{
  p_NearBillboard = Bill;
}

void StandardLOD::SetMid(StandardPolygonMesh *Mesh)
{
  p_MidMesh = Mesh;
}

void StandardLOD::SetMid(StandardPolygonBillboard *Bill)
{
  p_MidBillboard = Bill;
}

void StandardLOD::SetFar(StandardPolygonMesh *Mesh)
{
  p_FarMesh = Mesh;
}

void StandardLOD::SetFar(StandardPolygonBillboard *Bill)
{
  p_FarBillboard = Bill;
}

void StandardLOD::SetMidStart(float Z)
{
  p_NearZ = Z;
}
void StandardLOD::SetMidEnd(float Z)
{
  p_FarZ = Z;
}
void StandardLOD::SetFarEnd(float Z)
{
  p_EndZ = Z;
}

void StandardLOD::SetPos(float x, float y, float z)
{
  p_Pos.x = x;
  p_Pos.y = y;
  p_Pos.z = z;
}

unsigned long StandardLOD::Draw()
{

  unsigned long TPF = 0;

  StandardMatrix4 tempCamera = *p_Camera->GetCameraInverseMatrix();

  StandardVector3 temp1, tempResult;

  temp1.x = tempCamera.rowcol[3][0];
  temp1.y = tempCamera.rowcol[3][1];
  temp1.z = tempCamera.rowcol[3][2];

  tempResult = p_Pos - temp1;

  float Length;

  Length = tempResult.Length();

  if(Length < p_NearZ)
  {
    if(p_NearMesh != 0)
    {
      p_NearMesh->SetPos(p_Pos.x,p_Pos.y,p_Pos.z);
      TPF += p_NearMesh->Draw();
    }
    if(p_NearBillboard != 0)
    {
      p_NearBillboard->SetPos(p_Pos.x,p_Pos.y,p_Pos.z);
      TPF += p_NearBillboard->Draw();
    } 
  }
  if(Length >= p_NearZ && Length <= p_FarZ)
  {
    if(p_MidMesh != 0)
    {
      p_MidMesh->SetPos(p_Pos.x,p_Pos.y,p_Pos.z);
      TPF += p_MidMesh->Draw();
    }
    if(p_MidBillboard != 0)
    {
      p_MidBillboard->SetPos(p_Pos.x,p_Pos.y,p_Pos.z);
      TPF += p_MidBillboard->Draw();
    } 
  }
  if(Length > p_FarZ && Length <= p_EndZ)
  {
    if(p_FarMesh != 0)
    {
      p_FarMesh->SetPos(p_Pos.x,p_Pos.y,p_Pos.z);
      TPF += p_FarMesh->Draw();
    }
    if(p_FarBillboard != 0)
    {
      p_FarBillboard->SetPos(p_Pos.x,p_Pos.y,p_Pos.z);
      TPF += p_FarBillboard->Draw();
    } 
  }

  return TPF;
}
