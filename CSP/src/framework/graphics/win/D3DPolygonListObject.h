#ifndef __D3DPOLYGONLISTOBJECT_H_
#define __D3DPOLYGONLISTOBJECT_H_



class D3DPolygonListObject : public PolygonListObject
{
public:
	D3DPolygonListObject(StandardGraphics * m_pDisplay);
	virtual ~D3DPolygonListObject();

	virtual void Create(PrimitiveTypes type, int vertexCount, _GraphicsVector * v);
	virtual void Create(PrimitiveTypes type, int vertexCount, _GraphicsVector * v, _GraphicsVector *n);
	virtual void Create(PrimitiveTypes type, int vertexCount, _NormalVertexStruct *v);
	virtual void SetIndices(int indexCount, int size, void * indices);
	virtual void Destroy();
	virtual void Draw();

	virtual void SetName(char * name);
	virtual char * GetName();

	virtual void SetMatrix(_Matrix *  mat);
	virtual void MultiMatrix(_Matrix * mat);
	virtual void Translate(float X, float Y, float Z);
	virtual void Rotate(float angle, float x, float y, float z);


private:
//  LPDIRECT3DDEVICE8 m_Device;
  IDirect3DVertexBuffer8 * m_Buffer;
  int m_iVertexCount;
  int m_iIndexCount;
  D3DFORMAT m_iIndexType;
  D3DPRIMITIVETYPE m_polygonType;
  int m_iIndexed;
  IDirect3DIndexBuffer8 * m_iIndices;
  D3DGraphics * m_pD3Display;
  int m_iIndexSize;
  int m_iIndexBufferByteCount;
  int m_iPrimitiveCount;

protected:
	D3DFORMAT ConvertIndexTypes(int type);
	D3DPRIMITIVETYPE ConvertPolygonTypes(PrimitiveTypes polygonType);


};

#endif
