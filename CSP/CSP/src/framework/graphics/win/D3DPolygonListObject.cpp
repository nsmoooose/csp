
#ifdef D3D

#include "D3D.h"
#include "D3DPolygonListObject.h"



D3DPolygonListObject::D3DPolygonListObject(StandardGraphics * pDisplay) 
	: PolygonListObject( pDisplay)
{
	m_pD3Display = (D3DGraphics *)pDisplay;
	m_iIndexed = 0;
	m_iIndices = 0;
	m_iIndexCount = 0;
}

D3DPolygonListObject::~D3DPolygonListObject()
{

}
	
void D3DPolygonListObject::Create(PrimitiveTypes type, int vertexCount, _GraphicsVector * v)
{


}

void D3DPolygonListObject::Create(PrimitiveTypes type, int vertexCount, _GraphicsVector * v, _GraphicsVector *n)
{

}

void D3DPolygonListObject::Create(PrimitiveTypes type, int vertexCount, _NormalVertexStruct *buffer)
{
  m_iVertexCount = vertexCount;
  m_polygonType = ConvertPolygonTypes(type);
  long int bufferbytecount = vertexCount*sizeof(_NormalVertexStruct);
//  LPDIRECT3DDEVICE8 Device = ((D3DGraphics *)m_pDisplay)->GetDevice();
  m_pD3Display->Device->CreateVertexBuffer(bufferbytecount, 0, 
	  _NormalVertexType, D3DPOOL_DEFAULT, &m_Buffer);

  VOID* pVertices;

  m_Buffer->Lock( 0, bufferbytecount, (BYTE**)&pVertices, 0 );

  memset((void*)pVertices, 0x1C, bufferbytecount);
  memcpy( (void*)pVertices, (void*)buffer, bufferbytecount );

  m_Buffer->Unlock();
}

void D3DPolygonListObject::SetIndices(int indexCount, int indexSize, void * indices)
{
  m_iIndexed = 1;
  m_iIndexCount = indexCount;
  m_iIndexType = ConvertIndexTypes(indexSize);
  m_iIndexBufferByteCount = indexCount*indexSize;
  m_iPrimitiveCount = (int)(indexCount/3);
  m_iIndexSize = indexSize;
  m_pD3Display->Device->CreateIndexBuffer(m_iIndexBufferByteCount, 0, m_iIndexType , D3DPOOL_DEFAULT, &m_iIndices);
  VOID* pVertices;
  m_iIndices->Lock( 0, m_iIndexBufferByteCount, (BYTE**)&pVertices, 0 );
  // flip indices from right to left, this is for triangles only.
  unsigned char * pDestPtr = (unsigned char*)pVertices;
  unsigned char * pSrcPtr =  (unsigned char*)indices;
  for (int i = 0; i<m_iPrimitiveCount;i++)
  {

	memcpy( pDestPtr,   pSrcPtr,   m_iIndexSize );
	memcpy( pDestPtr+1*m_iIndexSize, pSrcPtr+2*m_iIndexSize, m_iIndexSize );
	memcpy( pDestPtr+2*m_iIndexSize, pSrcPtr+1*m_iIndexSize, m_iIndexSize );
	pDestPtr+=3*m_iIndexSize;
	pSrcPtr+=3*m_iIndexSize;
  }
  m_iIndices->Unlock();

}


void D3DPolygonListObject::Destroy()
{

}

void D3DPolygonListObject::Draw()
{

  m_pD3Display->Device->SetStreamSource( 0, (IDirect3DVertexBuffer8 *)m_Buffer, sizeof(_NormalVertexStruct));
  m_pD3Display->Device->SetVertexShader(_NormalVertexType);

  if (!m_iIndexed)
	m_pD3Display->Device->DrawPrimitive( m_polygonType, 0, (int)(m_iVertexCount/3));
  else
  {
	m_pD3Display->Device->SetIndices((IDirect3DIndexBuffer8 *)m_iIndices, 0);
    m_pD3Display->Device->DrawIndexedPrimitive( m_polygonType, 0, m_iVertexCount, 0, m_iPrimitiveCount );
  }

}

void D3DPolygonListObject::SetName(char * name)
{

}

char * D3DPolygonListObject::GetName()
{
	return NULL;
}

void D3DPolygonListObject::SetMatrix(_Matrix *  mat)
{

}

void D3DPolygonListObject::MultiMatrix(_Matrix * mat)
{

}

void D3DPolygonListObject::Translate(float X, float Y, float Z)
{

}

void D3DPolygonListObject::Rotate(float angle, float x, float y, float z)
{

}

D3DFORMAT D3DPolygonListObject::ConvertIndexTypes(int indexType)
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

D3DPRIMITIVETYPE D3DPolygonListObject::ConvertPolygonTypes(PrimitiveTypes polygonType)
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

#endif