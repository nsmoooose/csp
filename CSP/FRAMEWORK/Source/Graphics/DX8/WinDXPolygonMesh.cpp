#ifdef D3D

#include "WinDXPolygonMesh.h"

static const int BUFFERLOCKSIZE = 1024;

WinDXPolygonMesh::WinDXPolygonMesh(StandardGraphics *Graphics)
{

  p_Graphics = Graphics;
  p_Device = (IDirect3DDevice8 *)p_Graphics->GetDevice();

  p_Buffer = 0;
  p_Index = 0;
  m_iIndexed = 0;
  m_locked = false;
  //m_iIndexSize = 2;

  p_Matrix =  StandardMatrix4::IDENTITY;

  p_usPointCount = 0;
  p_usPolygonCount = 0;
  m_iShader = 0;
  m_iBufferStride = 0;
}

WinDXPolygonMesh::~WinDXPolygonMesh()
{
  Uninitialize();
}

short WinDXPolygonMesh::Initialize(PrimitiveTypes   type, 
                                   unsigned short   vertexCount, 
                                   StandardNTVertex *v)
{

  p_Type = type;
  m_polygonType = ConvertPolygonTypes(p_Type);
  p_usPointCount = vertexCount;
  m_iShader = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
  m_iBufferStride = sizeof(StandardNTVertex);

  long int bufferbytecount = vertexCount*sizeof(StandardNTVertex);

  p_usPolygonCount = vertexCount/PointsPerPolygon(p_Type);

  p_Device->CreateVertexBuffer(bufferbytecount, 0, 
	      D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, D3DPOOL_DEFAULT, &p_Buffer);

  VOID* pVertices;

  p_Buffer->Lock( 0, bufferbytecount, (BYTE**)&pVertices, 0 );
    memcpy( (void*)pVertices, (void*)v, bufferbytecount );
  p_Buffer->Unlock();

  return 0;
}

short WinDXPolygonMesh::Initialize(PrimitiveTypes type, unsigned short vertexCount, StandardVertex *v)
{
  p_Type = type;
  m_polygonType = ConvertPolygonTypes(p_Type);
  p_usPointCount = vertexCount;
  m_iShader = D3DFVF_XYZ;
  m_iBufferStride = sizeof(StandardVertex);

  long int bufferbytecount = vertexCount*sizeof(StandardVertex);

  p_usPolygonCount = vertexCount/PointsPerPolygon(p_Type);

  p_Device->CreateVertexBuffer(bufferbytecount, 0, 
	      D3DFVF_XYZ , D3DPOOL_DEFAULT, &p_Buffer);

  VOID* pVertices;

  p_Buffer->Lock( 0, bufferbytecount, (BYTE**)&pVertices, 0 );
    memcpy( (void*)pVertices, (void*)v, bufferbytecount );
  p_Buffer->Unlock();

  return 0;
}


short WinDXPolygonMesh::Initialize(PrimitiveTypes type, unsigned short vertexCount,  StandardTVertex *v)
{
	m_polygonType = ConvertPolygonTypes(type);
	return 0;
}

short WinDXPolygonMesh::Initialize(PrimitiveTypes type, unsigned short vertexCount, StandardCVertex * v)
{
	m_polygonType = ConvertPolygonTypes(type);
  return 0;
}

void WinDXPolygonMesh::SetIndexBuffer(int indexCount, void * indicies)
{


  HRESULT result;

  m_iIndexed = 1;
  m_iIndexCount = indexCount;

  m_iIndexBufferByteCount = indexCount* sizeof(_Index);
  p_usPolygonCount = (int)(indexCount/3);

  result = p_Device->CreateIndexBuffer(m_iIndexBufferByteCount, 0, D3DFMT_INDEX16 , D3DPOOL_DEFAULT, &p_Index);
  
  VOID* pVertices;

  result = p_Index->Lock( 0, m_iIndexBufferByteCount, (BYTE**)&pVertices, 0 );

  // flip indices from right to left, this is for triangles only.
  unsigned char * pDestPtr = (unsigned char*)pVertices;
  unsigned char * pSrcPtr =  (unsigned char*)indicies;

  for (int i = 0; i<p_usPolygonCount;i++)
  {
	  memcpy( pDestPtr,   pSrcPtr,   sizeof(_Index) );
	  memcpy( pDestPtr+1*sizeof(_Index), pSrcPtr+1*sizeof(_Index), sizeof(_Index) );
	  memcpy( pDestPtr+2*sizeof(_Index), pSrcPtr+2*sizeof(_Index), sizeof(_Index) );
	  pDestPtr+=3*sizeof(_Index);
	  pSrcPtr+=3*sizeof(_Index);
  }

  result = p_Index->Unlock();

  return;

}


void WinDXPolygonMesh::SetIndexBuffer(void *buffer, unsigned long indexcount)
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

/** 
 * Begin dynamic creation
 */
void WinDXPolygonMesh::BeginMesh(PrimitiveTypes type)
{
	m_polygonType = ConvertPolygonTypes(type);

	// create a new vertex buffer if we need one.
	if (p_Buffer == 0)
	{
		p_Device->CreateVertexBuffer(BUFFERLOCKSIZE*sizeof(StandardNTVertex), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 
			D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, D3DPOOL_DEFAULT, &p_Buffer);
	}
	// lock the buffer
	m_locked = true;
  p_usPointCount = 0;
  p_Buffer->Lock( 0, BUFFERLOCKSIZE*sizeof(StandardNTVertex), (BYTE**)&m_pVertices, D3DLOCK_DISCARD );
	m_iMemBufSize = BUFFERLOCKSIZE;
}

/** 
 * End dynamic creation
 */
void WinDXPolygonMesh::EndMesh()
{
	// unlock the buffer
	m_locked = false;
  p_Buffer->Unlock();
}


void WinDXPolygonMesh::AddTriangle(StandardNTVertex & vertex1,
								                   StandardNTVertex & vertex2,
								                   StandardNTVertex & vertex3)
{

  p_usPolygonCount++;
  AddVertex(vertex1);
  AddVertex(vertex2);
  AddVertex(vertex3);
}

void WinDXPolygonMesh::AddVertex(StandardNTVertex & vertex)
{
  if (p_usPointCount >= m_iMemBufSize)
  {
	// need more memory
  }

  *((StandardNTVertex*)m_pVertices+p_usPointCount) = vertex;
  p_usPointCount++;
}

void WinDXPolygonMesh::SetPos(float x, float y, float z)
{

  p_Matrix = TranslationMatrix4(x, y, z);

  return;
}

void WinDXPolygonMesh::SetMatrix(StandardMatrix4 *Matrix)
{
  p_Matrix = *Matrix;
}

unsigned long WinDXPolygonMesh::Draw()
{

  // this function takes TRANSPOSED (ROW/COL)

  p_Device->SetTransform(D3DTS_WORLD, (D3DMATRIX *)&p_Matrix);

  p_Device->SetVertexShader(m_iShader);

  p_Device->SetStreamSource( 0, p_Buffer, m_iBufferStride);

  if (!m_iIndexed)
	  p_Device->DrawPrimitive(m_polygonType, 0, p_usPointCount/3);
  else
  {
	  p_Device->SetIndices(p_Index, 0);
    p_Device->DrawIndexedPrimitive(m_polygonType, 0, p_usPointCount, 0, p_usPolygonCount);
  }
  return p_usPolygonCount;
}

void WinDXPolygonMesh::Uninitialize()
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

D3DFORMAT WinDXPolygonMesh::ConvertIndexTypes(int indexType)
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

D3DPRIMITIVETYPE WinDXPolygonMesh::ConvertPolygonTypes(PrimitiveTypes polygonType)
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

unsigned short WinDXPolygonMesh::PointsPerPolygon(PrimitiveTypes polygonType)
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

