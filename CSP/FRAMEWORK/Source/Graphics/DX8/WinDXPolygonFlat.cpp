#ifdef D3D

#include "WinDXPolygonFlat.h"

WinDXPolygonFlat::WinDXPolygonFlat(StandardGraphics *Graphics)
{

  p_Graphics = Graphics;
  p_Device = (IDirect3DDevice8 *)p_Graphics->GetDevice();
  p_Init = p_Graphics->GetInit();

  p_Buffer = 0;
  p_Index = 0;

  m_iIndexed = 0;

  p_usPointCount = 0;
  p_usPolygonCount = 0;

}

WinDXPolygonFlat::~WinDXPolygonFlat()
{
  Uninitialize();
}

_Rect* WinDXPolygonFlat::GetCoords()
{
  return &p_Coords;
}

short WinDXPolygonFlat::Initialize(float left, float top, float right, float bottom,
                                   bool Absolute, StandardColor *Color)
{

  p_Type = FW_TRIANGLES;
  m_polygonType = ConvertPolygonTypes(p_Type);

  p_Coords.x1 = left;
  p_Coords.y1 = top;
  p_Coords.x2 = right;
  p_Coords.y2 = bottom;

  p_usPointCount = 4;

  long int bufferbytecount = 4 * sizeof(StandardTLVertex);
  
  unsigned short x;

  p_usPolygonCount = 2;

  StandardTLVertex  Vertexes[4];

  Vertexes[0].Initialize( left,  top,  Color->r,Color->g,Color->b,Color->a);
  Vertexes[1].Initialize( right,  top,  Color->r,Color->g,Color->b,Color->a);
  Vertexes[2].Initialize( left,  bottom,  Color->r,Color->g,Color->b,Color->a);
  Vertexes[3].Initialize( right,  bottom,  Color->r,Color->g,Color->b,Color->a);
  
  p_Device->CreateVertexBuffer(bufferbytecount, 0, 
	      D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &p_Buffer);

  char* pVertices;

  p_Buffer->Lock( 0, bufferbytecount, (BYTE**)&pVertices, 0 );

  if (Absolute == false)
  {
    // convert coords
    for(x = 0; x < 4; x++)
    {
      Vertexes[x].p.x = (Vertexes[x].p.x * p_Init->Width);
      Vertexes[x].p.y = (Vertexes[x].p.y * p_Init->Height);
    }
  }

  memcpy(pVertices, Vertexes, bufferbytecount);

  p_Buffer->Unlock();

  short Indx[6];

  Indx[0] = 0;
  Indx[1] = 1;
  Indx[2] = 2;
  Indx[3] = 1;
  Indx[4] = 3;
  Indx[5] = 2;

  SetIndexBuffer(Indx, 6);


  return 0;
}

short WinDXPolygonFlat::Initialize(PrimitiveTypes   type,
                                   bool             Absolute,
                                   unsigned short    VertexCount, 
                                   StandardTLVertex  *Vertexes)
{

  p_Type = type;
  m_polygonType = ConvertPolygonTypes(type);

  p_usPointCount = VertexCount;

  unsigned long bufferbytecount = VertexCount * sizeof(StandardTLVertex);
  
  unsigned short x;

  p_usPolygonCount = VertexCount/3;

  p_Device->CreateVertexBuffer(bufferbytecount, 0, 
	      D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &p_Buffer);

  char* pVertices;

  p_Buffer->Lock( 0, bufferbytecount, (BYTE**)&pVertices, 0 );

  if(Absolute == false)
  {
    // convert coords
    for(x = 0; x < VertexCount; x++)
    {
      Vertexes[x].p.x = (Vertexes[x].p.x * p_Init->Width);
      Vertexes[x].p.y = (Vertexes[x].p.y * p_Init->Height);
    }
  }

  memcpy(pVertices, Vertexes, bufferbytecount);

  p_Buffer->Unlock();

  return 0;
}

void WinDXPolygonFlat::SetIndexBuffer(void *buffer, unsigned long indexcount)
{

  m_iIndexed = 1;

  long int bufferbytecount = indexcount*sizeof(_Index);

  p_usPolygonCount = (unsigned short)indexcount/PointsPerPolygon(p_Type);

  p_Device->CreateIndexBuffer(bufferbytecount, 0, D3DFMT_INDEX16 , D3DPOOL_DEFAULT, &p_Index);

  VOID* pVertices;

  p_Index->Lock( 0, bufferbytecount, (BYTE**)&pVertices, 0 );

  memcpy( pVertices, buffer, bufferbytecount );

  p_Index->Unlock();

  return;
}

unsigned long WinDXPolygonFlat::Draw()
{

  // this function takes SX/SY

  p_Device->SetStreamSource( 0, p_Buffer, sizeof(StandardTLVertex));
  p_Device->SetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

  if (!m_iIndexed)
	  p_Device->DrawPrimitive(m_polygonType, 0, p_usPointCount/3);
  else
  {
	  p_Device->SetIndices(p_Index, 0);
    p_Device->DrawIndexedPrimitive(m_polygonType, 0, p_usPointCount, 0, p_usPolygonCount);
  }

  return p_usPolygonCount;

}


void WinDXPolygonFlat::Uninitialize()
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
}


D3DFORMAT WinDXPolygonFlat::ConvertIndexTypes(int indexType)
{
	switch (indexType)
	{
		case 1:
		case 2:
		return D3DFMT_INDEX16;
		break;

		case 4:
		return D3DFMT_INDEX32;
		break;

		default:
		return (D3DFORMAT)0;
	}

}

D3DPRIMITIVETYPE WinDXPolygonFlat::ConvertPolygonTypes(PrimitiveTypes polygonType)
{
	switch (polygonType)
	{
	case FW_POINTS:
		return D3DPT_POINTLIST;
		break;

	case FW_LINES:
		return D3DPT_LINELIST;
		break;

	case FW_LINE_STRIP:
		return D3DPT_LINESTRIP;
		break;

	case FW_TRIANGLES:
		return D3DPT_TRIANGLELIST;
		break;

	case FW_TRIANGLE_STRIP:
		return D3DPT_TRIANGLESTRIP;
		break;

	case FW_TRIANGLE_FAN:
		return D3DPT_TRIANGLEFAN;
		break;

	default:
		return (D3DPRIMITIVETYPE)-1;
		break;

	}
}

unsigned short WinDXPolygonFlat::PointsPerPolygon(PrimitiveTypes polygonType)
{
	switch (polygonType)
	{
	case FW_POINTS:
		return 1;
		break;

	case FW_LINES:
		return 2;
		break;

	case FW_LINE_STRIP:
		return 0;
		break;

	case FW_TRIANGLES:
		return 3;
		break;

	case FW_TRIANGLE_STRIP:
		return 0;
		break;

	case FW_TRIANGLE_FAN:
		return 0;
		break;

	default:
		return (D3DPRIMITIVETYPE)-1;
		break;

	}

}

#endif