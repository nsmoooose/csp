// The pure abstract class for Graphics.

#ifndef SGTag
#define SGTag

  #include "TypesGraphics.h"

  class StandardGraphics: public StandardFramework
  {

  private:


  public:

    virtual short           Create3DWindow() = 0;

    virtual _VertexBuffer*  CreateVertexBuffer(void *buffer, unsigned long bufferbytecount, unsigned long vertexbytecount) = 0;
    virtual _IndexBuffer*   CreateIndexBuffer(void *buffer, unsigned long bufferbytecount, unsigned long indexbytecount) = 0;
    virtual _Texture*       CreateTexture(char *Filename) = 0;
    virtual _Light*         CreateDirectionalLight(_Vector* Direction, _Color* Color) = 0;
    virtual _Texture*       CreateTexture(short Width, short Height, short MipMap) = 0;
    virtual _Material*      CreateMaterial(_Color *Diffuse, _Color *Ambient, _Color *Emissive) = 0;

	virtual PolygonListObject * CreatePolygonListObject() = 0;
	virtual MaterialObject * CreateMaterialObject() = 0;
	virtual Camera * CreateCamera() = 0;
	virtual Texture2D * CreateTexture2D() = 0;



    virtual bool            CanFlip() = 0;

    virtual void            SetTransformWorld(_Matrix *matrix) = 0;
    virtual void            SetTransformCamera(_Matrix *matrix) = 0;
    virtual void            SetTransformFOV(_Matrix *matrix) = 0;

    virtual void            BeginScene() = 0;
    virtual void            ClearSurface() = 0;
    virtual void            EndScene() = 0;
    virtual void            Flip() = 0;

    virtual void            SetVertexBuffer(_VertexBuffer *VertexBuffer, short vertexbytecount) = 0;
    virtual void            SetTexture(short stage, _Texture *Texture) = 0;
    virtual void            SetMaterial(_Material *Mat) = 0;
    virtual void            SetIndexBuffer(void* Buffer) = 0;

    virtual void            DrawTriangleListBuffer(short vertexcount, short indexcount) = 0;

    virtual void            SetLight(short lightnum, _Light *Light) = 0;
    virtual void            LightEnabled(short lightnum, short lighton) = 0;

    virtual void            DrawTextF(char *buffer, short bufferbytecount, _Rect rect, short LeftCenterRight, _Color *color ) = 0;
    virtual _Rect           GetCoordsRect() = 0;
    virtual void            SetForeground(short on) = 0;
  
    virtual void            DeleteVertexBuffer(_VertexBuffer *buff) = 0;
    virtual void            DeleteIndexBuffer(_IndexBuffer *buff) = 0;
    virtual void            DeleteTexture(_Texture *tex) = 0;
    virtual void            DeleteMaterial(_Material *mat) = 0;
    virtual void            DeleteLight(_Light *light) = 0;


	virtual void SetIdentity() = 0;
	virtual void SetModelViewMatrix() = 0;
	virtual void SetProjectionMatrix() = 0;
	virtual void SetTextureMatrix() = 0;

	virtual void Translate(float x, float y, float z) = 0;
	virtual void Rotate(float angle, float x, float y, float z) = 0;
	virtual void Scale(float x, float y, float z) = 0;
	virtual void LoadMatrix() = 0;
	virtual void MultiMatrix() = 0;
	virtual void PushMatrix() = 0;
	virtual void PopMatrix() = 0;

	virtual void DrawPoint(float x, float y, float z) = 0;
	virtual void DrawPoint(_GraphicsVector * v) = 0;
	virtual void DrawPointArray(int vertexCount, _GraphicsVector *v) = 0;
	virtual void DrawLine(float x1, float y1, float z1,
							  float x2, float y2, float z2) = 0;
	virtual void DrawRect(float x1, float y1, float x2, float y2) = 0;
	virtual void DrawTriangle(float x1, float y1, float z1,
					float x2, float y2, float z2, 
					float x3, float y3, float z3) = 0;
	virtual void DrawTriangle(_GraphicsVector * v1, _GraphicsVector * v2,
					_GraphicsVector * v3) = 0;
	virtual void DrawTriangleArray(int vertexCount, _GraphicsVector * v) = 0;
	virtual void DrawTriangleFan(int vertexCount, _GraphicsVector *v) = 0;
	virtual void DrawTriangleStrip(int vertexCount, _GraphicsVector *v) = 0;
	virtual void DrawTriangleArray(int vertexCount, _ShadedVector *v) = 0;
	virtual void DrawTriangleFan(int vertexCount, _ShadedVector *v) = 0;
	virtual void DrawTriangleStrip(int vertexCount, _ShadedVector *v) = 0;
	virtual void DrawQuad() = 0;
	virtual void DrawString(const char * str) = 0;

	virtual _Viewport * CreateViewport() = 0;
	virtual _Viewport * CreateViewport(float left, float top, float width, float height) = 0;
	virtual void ModifyViewport(float left, float top, float width, float height) {}
	virtual void SetViewport(_Viewport *view) = 0;
	virtual void SetViewport(float left, float top, float width, float height) = 0;
	virtual void SetOrtho(double left, double right, double bottom,
		double top, double nearPos, double farPos) = 0;
	virtual void DeleteViewport(_Viewport *view) = 0;
	virtual void SetPerspective(double fov, double aspect, double nearPos, double farPos) = 0;

	virtual void BeginPrimitive(PrimitiveTypes) = 0;
	virtual void EndPrimitive() = 0;
	virtual void Vertex(float x, float y, float z) = 0;
	virtual void Normal(float x, float y, float z) = 0;

	virtual void SetColor(float r, float g, float b, float a) = 0;
	virtual void SetColor(float r, float g, float b) = 0;

	virtual void Flush() = 0;
	virtual void SetClearColor(float r, float g, float b, float a) = 0;

	virtual void CullFrontFace() = 0;
	virtual void CullBackFace() = 0; 
	virtual void SetFrontFace(PolygonEnum direct) = 0;

	virtual void SetTextureMode(int flag) = 0;
	virtual void Look(double x, double y, double z,
		              double tar_x, double tar_y, double tar_z,
					  double up_x, double up_y, double up_z) = 0;





  };



#endif
