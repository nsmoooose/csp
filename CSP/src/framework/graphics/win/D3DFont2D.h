#idndef __D3DFONT2D_H_
#define __D3DFONT2D_H_


/** \class D3DFont2D
 * DirectX Font class
 */
class D3DFont2D : public Font2D
{
public:
	D3DFont2D(StandardGraphics * pDisplay)	{}
	virtual ~D3DFont2D() {}
	virtual void Create(const char * filename, int type);
	virtual void DrawTextF(char *buffer, short bufferbytecount, _Rect rect, short LeftCenterRight, _Color*color);
	virtual void Begin();
	virtual void End();

protected:
};

#endif