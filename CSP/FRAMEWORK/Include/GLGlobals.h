#ifndef __GL_GLOBALS_H__
#define __GL_GLOBALS_H__



#define		MAXTEXTURES		2

class GLMaterial;

class GLStateMachine
{
public:
	GLStateMachine();
	~GLStateMachine();
	void initStateMachine();

	void dumpGLStates();

	// Colors
	StandardVector3		cClearColor;				// Clear Color
	StandardVector3		cColor;						// Current Color
	GLenum		shademode;					// Smooth Shading

	// Materials
	GLMaterial	* cMaterial;					// Current Material

	// Textures
	GLuint		boundTex[MAXTEXTURES];		// Bound texture objects
	GLint		TexEnv[MAXTEXTURES];		// Texture environments


	// Lights
	int			iLights;					// Active Lights
	
	static const int	LIGHT0;
	static const int	LIGHT1;
	static const int	LIGHT2;
	static const int	LIGHT3;
	static const int	LIGHT4;
	static const int	LIGHT5;
	static const int	LIGHT6;
	static const int	LIGHT7;

	// glEnable/glDisable
	static int			iEnable;

	static const int	BLENDING;		// Blending
	static const int	TEXTURE1D;		// 1D Texturing
	static const int	TEXTURE2D;		// 2D Texturing
	static const int	TEXTURE3D;		// 3D Texturing
	static const int	DEPTHTEST;		// Depth Testing
	static const int	FOG;			// Fogging
	static const int	CULLFACE;		// Face culling
	static const int    WIREFRAME;      // wireframe



	// Vertex Array
	static int			activeArrays;	// Active arrays
	float *				vertexArray;	// current vertex array
	float *				colorArray;		// current color array
	float *				normalArray;	// current normal array
	float *				textureArray0;	// current texture array - texunit 0
	float *				textureArray1;	// current texture array - texunit 1

	static const int	VERTEXARRAY;	// Vertex Array Enabled
	static const int	COLORARRAY;		// Color Array
	static const int	NORMALARRAY;	// Normal Array
	static const int	TEXTUREARRAY0;	// Texture Array0
	static const int	TEXTUREARRAY1;	// Texture Array1

	static const int    VNTFORMAT;      // vertex-normal-texture format
	static const int    VTFORMAT;       // vertex-texture format
	static const int    VNFORMAT;       // vertex-normal format
	static const int    VFORMAT;        // vertex format
	static const int    VCFORMAT;       // vertex-color format

	static void changeArrays(int newFormat);
	static void SetWireframe(bool flag);
	static bool GetWireframe();
	static void SetZBuffer(bool flag);
	static bool GetZBuffer();

	// Matirx Modes
	GLenum				eMatrixMode;
};

extern GLStateMachine					statesGL;

#ifdef	WIN32
extern PFNGLMULTITEXCOORD2FVARBPROC			glMultiTexCoord2fv;
extern PFNGLACTIVETEXTUREARBPROC			glActiveTextureARB;
extern PFNGLCLIENTACTIVETEXTUREARBPROC		glClientActiveTextureARB;
#endif


#endif

