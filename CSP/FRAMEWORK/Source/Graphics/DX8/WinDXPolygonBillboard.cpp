#ifdef D3D

#include "WinDXPolygonBillboard.h"

WinDXPolygonBillboard::WinDXPolygonBillboard(StandardGraphics *Graphics)
{

  p_Graphics = Graphics;
  p_Device = (IDirect3DDevice8 *)p_Graphics->GetDevice();

  p_Buffer = 0;
  p_Index = 0;

  p_Matrix =  StandardMatrix4::IDENTITY;
  p_Camera = 0;

  p_YAxisLocked = false;

}

WinDXPolygonBillboard::~WinDXPolygonBillboard()
{
  Uninitialize();
}

short WinDXPolygonBillboard::Initialize(StandardCamera *Camera, float Width, float Height, bool YAxisLocked)
{

  p_Camera = Camera;
  p_YAxisLocked = YAxisLocked;

  // ---

  StandardNTVertex Billboard[4];
  _Index  Index[6];

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

  // --- create it now

  p_Device->CreateVertexBuffer(sizeof(Billboard), 0, 
	      D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, D3DPOOL_DEFAULT, &p_Buffer);

  VOID* pVertices;

  p_Buffer->Lock( 0, sizeof(Billboard), (BYTE**)&pVertices, 0 );
  memcpy( (void*)pVertices, (void*)Billboard, sizeof(Billboard) );
  p_Buffer->Unlock();

  p_Device->CreateIndexBuffer(6 * sizeof(_Index), 0, D3DFMT_INDEX16 , D3DPOOL_DEFAULT, &p_Index);

  p_Index->Lock( 0, 6 * sizeof(_Index), (BYTE**)&pVertices, 0 );
  memcpy( pVertices, Index, 6 * sizeof(_Index) );
  p_Index->Unlock();

  return 0;
}

void WinDXPolygonBillboard::SetPos(float x, float y, float z)
{

  p_Matrix = TranslationMatrix4(x, y, z);

  return;
}

unsigned long WinDXPolygonBillboard::Draw()
{

  StandardMatrix4 tempCamera = *p_Camera->GetCameraInverseMatrix();

  if(p_YAxisLocked == true)
  {

    StandardVector3 tempVec1, tempVec2, tempForward, tempUp, tempRight;

    tempVec1.x = tempCamera.rowcol[3][0];
    tempVec1.y = 0;
    tempVec1.z = tempCamera.rowcol[3][2];

    tempVec2.x = p_Matrix.rowcol[3][0];
    tempVec2.y = 0;
    tempVec2.z = p_Matrix.rowcol[3][2];

    tempForward = tempVec2 - tempVec1;

    tempForward.Normalize();

    tempUp.x = 0;
    tempUp.y = 1;
    tempUp.z = 0;

    tempRight = Cross(tempUp, tempForward);

    // not transposed. REQUIRED FOR ODD REASON.

    p_Matrix.rowcol[0][0] = tempRight.x;
    p_Matrix.rowcol[0][1] = tempRight.y;
    p_Matrix.rowcol[0][2] = tempRight.z;

    p_Matrix.rowcol[1][0] = tempUp.x;
    p_Matrix.rowcol[1][1] = tempUp.y;
    p_Matrix.rowcol[1][2] = tempUp.z;

    p_Matrix.rowcol[2][0] = tempForward.x;
    p_Matrix.rowcol[2][1] = tempForward.y;
    p_Matrix.rowcol[2][2] = tempForward.z;
    
/*  // transposed but it doesn't work.
    p_Matrix.rowcol[0][0] = tempRight.x;
    p_Matrix.rowcol[1][0] = tempRight.y;
    p_Matrix.rowcol[2][0] = tempRight.z;

    p_Matrix.rowcol[0][1] = tempUp.x;
    p_Matrix.rowcol[1][1] = tempUp.y;
    p_Matrix.rowcol[2][1] = tempUp.z;

    p_Matrix.rowcol[0][2] = tempForward.x;
    p_Matrix.rowcol[1][2] = tempForward.y;
    p_Matrix.rowcol[2][2] = tempForward.z;
*/

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

  // this function takes NON-TRANSPOSED (COL/ROW) (for some $#*&*&@# reason)

  p_Device->SetTransform(D3DTS_WORLD, (D3DMATRIX *)&p_Matrix);

  p_Device->SetVertexShader(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);

  p_Device->SetStreamSource( 0, p_Buffer, sizeof(StandardNTVertex));
	p_Device->SetIndices(p_Index, 0);

  p_Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 4, 0, 2);
  
  return 2;
}

void WinDXPolygonBillboard::Uninitialize()
{

  if(p_Index !=0)
  {
    p_Index->Release();
    p_Index = 0;
  }

  if(p_Buffer != 0)
  {
    p_Buffer->Release();
    p_Buffer = 0;
  }

  return;
}

#endif

