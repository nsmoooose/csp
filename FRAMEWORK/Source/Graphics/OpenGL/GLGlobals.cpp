///////////////////////////////////////////////////////////////////////////
//
//   GLGlobals  - CSP - http://csp.homeip.net
//
//   coded by GKW, Wolverine
//
///////////////////////////////////////////////////////////////////////////
//   
//  This code represents a software caching the OpenGL state machine.
//  The code is used to intercept OpenGL state machine calls to prevent
//  a unnecessary system call if the state is already active.
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//
///////////////////////////////////////////////////////////////////////////




#ifdef OGL

#include "Framework.h"
#include "GLGlobals.h"

#include "string"

// Remove this when 1.2 comes out for windows
#ifdef	WIN32
PFNGLMULTITEXCOORD2FVARBPROC		glMultiTexCoord2fv;
PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB;
PFNGLCLIENTACTIVETEXTUREARBPROC		glClientActiveTextureARB;
#endif

// Global state machine
GLStateMachine	statesGL;

// Initialize GLStateMachine Variables
int const		GLStateMachine::LIGHT0			= 1 << 1;
int const		GLStateMachine::LIGHT1			= 1 << 2;
int const		GLStateMachine::LIGHT2			= 1 << 3;
int const		GLStateMachine::LIGHT3			= 1 << 4;
int const		GLStateMachine::LIGHT4			= 1 << 5;
int const		GLStateMachine::LIGHT5			= 1 << 6;
int const		GLStateMachine::LIGHT6			= 1 << 7;
int const		GLStateMachine::LIGHT7			= 1 << 8;


int const		GLStateMachine::BLENDING		= 1 << 1;
int const		GLStateMachine::TEXTURE1D		= 1 << 2;
int const		GLStateMachine::TEXTURE2D		= 1 << 3;
int const		GLStateMachine::TEXTURE3D		= 1 << 4;
int const		GLStateMachine::DEPTHTEST		= 1 << 5;
int const		GLStateMachine::FOG				= 1 << 6;
int const		GLStateMachine::CULLFACE		= 1 << 7;
int const       GLStateMachine::WIREFRAME       = 1 << 8;


int const		GLStateMachine::VERTEXARRAY		= 1 << 1;
int const		GLStateMachine::COLORARRAY		= 1 << 2;
int const		GLStateMachine::NORMALARRAY		= 1 << 3;
int const		GLStateMachine::TEXTUREARRAY0	= 1 << 4;
int const		GLStateMachine::TEXTUREARRAY1	= 1 << 5;


int const       GLStateMachine::VNTFORMAT = VERTEXARRAY | NORMALARRAY | TEXTUREARRAY0;
int const       GLStateMachine::VNFORMAT = VERTEXARRAY | NORMALARRAY;
int const       GLStateMachine::VTFORMAT = VERTEXARRAY | TEXTUREARRAY0;
int const       GLStateMachine::VFORMAT = VERTEXARRAY;
int const       GLStateMachine::VCFORMAT = VERTEXARRAY | COLORARRAY;


int             GLStateMachine::activeArrays = 0;
int             GLStateMachine::iEnable = 0;


/**
 * \fn GLStateMachine::GLStateMachine()
 *	\brief GLStateMachine Constructor
*/
GLStateMachine::GLStateMachine()
{
  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLStateMachine::GLStateMachine()");

}

/**
 * \fn GLStateMachine::~GLStateMachine()
 * \brief GLStateMachine Destructor
 */
GLStateMachine::~GLStateMachine()
{
  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLStateMachine::~GLStateMachine()");

}

/** 
 * \fn void GLStateMachine::initStateMachine()
 * \ brief Initializes the StateMachine. There must be a valid context to initialize against.
 */
void GLStateMachine::initStateMachine()
{

  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLStateMachine::initStateMachine()");

// Remove with 1.2
#ifdef	WIN32
	glMultiTexCoord2fv		= (PFNGLMULTITEXCOORD2FVARBPROC) wglGetProcAddress("glMultiTexCoord2fvARB");
	glActiveTextureARB		= (PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress("glActiveTextureARB");
	glClientActiveTextureARB= (PFNGLCLIENTACTIVETEXTUREARBPROC) wglGetProcAddress("glClientActiveTextureARB");
#endif

	shademode				= GL_FLAT;
	glShadeModel( GL_FLAT );

	for( int i = 0; i < MAXTEXTURES; i++ )
	{
		boundTex[i] = 0;
		TexEnv[i]	= GL_REPLACE;
	}
	iLights			= 0;
	iEnable			= 0;
	activeArrays	= 0;
	vertexArray		= NULL;
	colorArray		= NULL;
	normalArray		= NULL;
	textureArray0	= NULL;
	textureArray1	= NULL;

	eMatrixMode		= GL_PROJECTION;
	glMatrixMode( GL_PROJECTION );


}

/** 
  * \fn GLStateMachine::changeArrays(int newFormat)
  * \brief Change array formats
  * \param newFormat A bitset containing the new set of arrays to use.
  * 
  * The array formats are the
  * vertex buffer arrays that are used by the current
  * vertex type. This routine cache's the Array states to
  * avoid unnecessary system calls.
  */
void GLStateMachine::changeArrays(int newFormat)
{

  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLStateMachine::changeArrays()");

	if (activeArrays == newFormat)
		return;
	int diffFormat = activeArrays ^ newFormat;
	int removeFormat = diffFormat & activeArrays;
	int addFormat = diffFormat & newFormat;

	// remove formats 
	if (removeFormat)
	{
		if (removeFormat & VERTEXARRAY)
			glDisableClientState(GL_VERTEX_ARRAY);
		if (removeFormat & COLORARRAY)
			glDisableClientState(GL_COLOR_ARRAY);
		if (removeFormat & NORMALARRAY)
			glDisableClientState(GL_NORMAL_ARRAY);
		if (removeFormat & TEXTUREARRAY0)
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	
	// add formats
	if (addFormat)
	{
		if (addFormat & VERTEXARRAY)
			glEnableClientState(GL_VERTEX_ARRAY);
		if (addFormat & COLORARRAY)
			glEnableClientState(GL_COLOR_ARRAY);
		if (addFormat & NORMALARRAY)
			glEnableClientState(GL_NORMAL_ARRAY);
		if (addFormat & TEXTUREARRAY0)
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	activeArrays = newFormat;
}


/**
 * \fn  void GLStateMachine::SetWireframe(bool flag)
 * \brief Set wireframe mode.
 * \param flag A flag to determine the WireFrame state.
 */
void GLStateMachine::SetWireframe(bool flag)
{

    CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLStateMachine::SetWireframe()");


	// setting wireframe mode
	if (flag)
	{
		// if not already in wireframe mode switch otherwise 
		// do nothing.
		if(!(iEnable & WIREFRAME))
		{
			iEnable = iEnable | WIREFRAME;
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
	}
	// clearing wireframe mode
	else
	{
		if (iEnable & WIREFRAME)
		{
			iEnable = iEnable ^ WIREFRAME;
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}
}

/**
 * \fn bool GLStateMachine::GetWireframe()
 * \brief Query WireFrame mode
 */
bool GLStateMachine::GetWireframe()
{
    CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLStateMachine::GetWireframe()");

    return (iEnable & WIREFRAME);
}

/** 
 * \fn void GLStateMachine::SetZBuffer(bool flag)
 * \brief Active Z Buffer mode (Depth Test)
 * \param flag A flag to determine the ZBuffer state.
 */
void GLStateMachine::SetZBuffer(bool flag)
{
    CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLStateMachine::SetZBuffer()");

	// setting zbuffer mode
	if (flag)
	{
		// if not already in depthtest mode switch otherwise 
		// do nothing.
		if(!(iEnable & DEPTHTEST))
		{
			iEnable = iEnable | DEPTHTEST;
			glEnable(GL_DEPTH_TEST);
		}
	}
	// clearing depthtest mode
	else
	{
		if (iEnable & DEPTHTEST)
		{
			iEnable = iEnable ^ DEPTHTEST;
			glDisable(GL_DEPTH_TEST);
		}
	}

}

/** 
 * \fn bool GLStateMachine::GetZBuffer()
 * \brief Query the Z Buffer mode.
 */
bool GLStateMachine::GetZBuffer()
{
    CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLStateMachine::GetZBuffer()");

	return (iEnable & DEPTHTEST);
}

/**
 * \fn void GLStateMachine::dumpGLStates()
 * Dump all the current OpenGL States. This is
 * for debugging mainly.
 */
void GLStateMachine::dumpGLStates()
{
    CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLStateMachine::dumpGLStates()");

    GLfloat color[4];
	GLint matrixmode;
	GLboolean normalizeFlag;
	GLint shademode;
	GLboolean lightingFlag;
	GLboolean colorMaterialFlag;
	GLboolean colorMaterialParameterFlag;
	GLboolean colorMaterialFaceFlag;
	GLfloat ambientColor[4];
	GLfloat diffuseColor[4];
	GLfloat specularColor[4];
	GLfloat emissionColor[4];
	GLfloat shinness;
	GLboolean blendFlag;
	GLboolean texture1DFlag;
	GLboolean texture2DFlag;
	GLboolean depthFlag;
	GLboolean vertexArrayFlag;
	GLboolean normalArrayFlag;
	GLboolean textureCoordArrayFlag;

    CSP_LOG(CSP_OPENGL, CSP_INFO, "Dumping OpenGL States");

	glGetFloatv(GL_CURRENT_COLOR, color);

    CSP_LOG(CSP_OPENGL, CSP_INFO, "Current color (" <<
        color[0] << ", " << color[1] << ", " << color[2] << ", " << color[3] << ")");
    

	glGetIntegerv(GL_MATRIX_MODE, &matrixmode);
	std::string matrixModeString;
	if (!(matrixmode & GL_MODELVIEW))
		matrixModeString = "Model View";
	else if (!(matrixmode & GL_PROJECTION))
		matrixModeString = "Projection";
	else if (!(matrixmode & GL_TEXTURE))
		matrixModeString = "Texture";
	else
	        matrixModeString = "Unknown";

    CSP_LOG(CSP_OPENGL, CSP_INFO, "Matrix Mode " << matrixModeString);

	normalizeFlag = glIsEnabled(GL_NORMALIZE);
    CSP_LOG(CSP_OPENGL, CSP_INFO, "Normal normalization mode is " << (normalizeFlag ? "on" : "off") );

	glGetIntegerv(GL_SHADE_MODEL, &shademode);
    CSP_LOG(CSP_OPENGL, CSP_INFO, "Shade Mode = " << shademode);

	lightingFlag = glIsEnabled(GL_LIGHTING);
    CSP_LOG(CSP_OPENGL, CSP_INFO, "Lighting Mode is " << (lightingFlag ? "on" : "off") );
	
	colorMaterialFlag = glIsEnabled(GL_COLOR_MATERIAL);
    CSP_LOG(CSP_OPENGL, CSP_INFO, "Color Material mode is " << (colorMaterialFlag ? "on" : "off") );
	
	colorMaterialParameterFlag = glIsEnabled(GL_COLOR_MATERIAL_PARAMETER);
    CSP_LOG(CSP_OPENGL, CSP_INFO, "Color Material Parameter mode is " << (colorMaterialParameterFlag ? "on" : "off") );
    
	colorMaterialFaceFlag = glIsEnabled(GL_COLOR_MATERIAL_FACE);
    CSP_LOG(CSP_OPENGL, CSP_INFO, "Color Material Face mode is " << (colorMaterialFaceFlag ? "on" : "off") );

	glGetMaterialfv(GL_FRONT, GL_AMBIENT, ambientColor);
    CSP_LOG(CSP_OPENGL, CSP_INFO, "Ambient color (" << 
        ambientColor[0] << ", " << ambientColor[1] << ", " <<
        ambientColor[2] << ", " << ambientColor[3] << ")" );

	glGetMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseColor);
    CSP_LOG(CSP_OPENGL, CSP_INFO, "Diffuse color (" <<
        diffuseColor[0] << ", " << diffuseColor[1] << ", " <<
        diffuseColor[2] << ", " << diffuseColor[3] << ")" );


	glGetMaterialfv(GL_FRONT, GL_SPECULAR, specularColor);
    CSP_LOG(CSP_OPENGL, CSP_INFO, "Specular Color (" <<
        specularColor[0] << ", " <<  specularColor[1] << ", " <<
        specularColor[2] << ", " <<  specularColor[3] << ")" );


	glGetMaterialfv(GL_FRONT, GL_EMISSION, emissionColor);
    CSP_LOG(CSP_OPENGL, CSP_INFO, "Emission Color (" <<
        emissionColor[0] << ", " << emissionColor[1] << ", " <<
        emissionColor[2] << ", " << emissionColor[3] << ")" );


    glGetFloatv(GL_SHININESS, &shinness);
    CSP_LOG(CSP_OPENGL, CSP_INFO, "Shinness Value " << shinness );

	blendFlag = glIsEnabled(GL_BLEND);
    CSP_LOG(CSP_OPENGL, CSP_INFO, "Blending is " << (blendFlag ? "on" : "off") );

	texture1DFlag = glIsEnabled(GL_TEXTURE_1D);
    CSP_LOG(CSP_OPENGL, CSP_INFO, "1D texture is " << (texture1DFlag ? "on" : "off") );

	texture2DFlag = glIsEnabled(GL_TEXTURE_2D);
    CSP_LOG(CSP_OPENGL, CSP_INFO, "2D texture is " << (texture2DFlag ? "on" : "off") );

	depthFlag = glIsEnabled(GL_DEPTH_TEST);
    CSP_LOG(CSP_OPENGL, CSP_INFO, "Depth test is " << (depthFlag ? "on" : "off") );

	vertexArrayFlag = glIsEnabled(GL_VERTEX_ARRAY);
    CSP_LOG(CSP_OPENGL, CSP_INFO, "Vertex array is " << (vertexArrayFlag ? "on" : "off") );
	
	normalArrayFlag = glIsEnabled(GL_NORMAL_ARRAY);
    CSP_LOG(CSP_OPENGL, CSP_INFO, "Normal array is " << (normalArrayFlag ? "on" : "off") );

	textureCoordArrayFlag = glIsEnabled(GL_TEXTURE_COORD_ARRAY);
    CSP_LOG(CSP_OPENGL, CSP_INFO, "Texture coord array is " << (textureCoordArrayFlag ? "on" : "off") );

}

#endif
