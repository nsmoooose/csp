#ifdef OGL


#include "stdio.h"
#include "gl_globals.h"
#include "string"

// Remove this when 1.2 comes out for windows
#ifdef	WIN32
PFNGLMULTITEXCOORD2FVARBPROC		glMultiTexCoord2fv;
PFNGLACTIVETEXTUREARBPROC			glActiveTextureARB;
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

int const		GLStateMachine::VERTEXARRAY		= 1 << 1;
int const		GLStateMachine::COLORARRAY		= 1 << 2;
int const		GLStateMachine::NORMALARRAY		= 1 << 3;
int const		GLStateMachine::TEXTUREARRAY0	= 1 << 4;
int const		GLStateMachine::TEXTUREARRAY1	= 1 << 5;

int const       GLStateMachine::VNTFORMAT = VERTEXARRAY | NORMALARRAY | TEXTUREARRAY0;
int const       GLStateMachine::VNFORMAT = VERTEXARRAY | NORMALARRAY;
int const       GLStateMachine::VTFORMAT = VERTEXARRAY | TEXTUREARRAY0;
int const       GLStateMachine::VFORMAT = VERTEXARRAY;

int             GLStateMachine::activeArrays = 0;

/**
	GLStateMachine Constructor
*/
GLStateMachine::GLStateMachine()
{

}

/**
	GLStateMachine Destructor
*/
GLStateMachine::~GLStateMachine()
{

}

/** 
	There must be a valid context to initialize against.
*/
void GLStateMachine::initStateMachine()
{
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
  Change array formats 
  */
void GLStateMachine::changeArrays(int newFormat)
{
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


void GLStateMachine::dumpGLStates()
{
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


	glGetFloatv(GL_CURRENT_COLOR, color);
	printf("Current color (%f, %f, %f, %f)\n", 
		color[0], color[1], color[2], color[3]);

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

	printf("Matrix mode %s\n", matrixModeString);

	normalizeFlag = glIsEnabled(GL_NORMALIZE);
	printf("Normal normalization mode is %s.\n", normalizeFlag ? "on" : "off");

	glGetIntegerv(GL_SHADE_MODEL, &shademode);
	printf("Shade mode = %d\n", shademode);

	lightingFlag = glIsEnabled(GL_LIGHTING);
	printf("Lighting mode is %s.\n", lightingFlag ? "on" : "off");
	
	colorMaterialFlag = glIsEnabled(GL_COLOR_MATERIAL);
	printf("Color Material mode is %s.\n", colorMaterialFlag ? "on" : "off");
	
	colorMaterialParameterFlag = glIsEnabled(GL_COLOR_MATERIAL_PARAMETER);
	printf("Color Material Parameter mode is %s.\n", 
		colorMaterialParameterFlag ? "on" : "off");

	colorMaterialFaceFlag = glIsEnabled(GL_COLOR_MATERIAL_FACE);
	printf("Color Material Face mode is %s.\n", colorMaterialFaceFlag ? "on" : "off");

	glGetMaterialfv(GL_FRONT, GL_AMBIENT, ambientColor);
	printf("Ambient color (%f, %f, %f, %f)\n", 
		ambientColor[0], ambientColor[1], ambientColor[2], ambientColor[3]);

	glGetMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseColor);
	printf("Diffuse color (%f, %f, %f, %f)\n", 
		diffuseColor[0], diffuseColor[1], diffuseColor[2], diffuseColor[3]);

	glGetMaterialfv(GL_FRONT, GL_SPECULAR, specularColor);
	printf("Specular color (%f, %f, %f, %f)\n", 
		specularColor[0], specularColor[1], specularColor[2], specularColor[3]);

	glGetMaterialfv(GL_FRONT, GL_EMISSION, emissionColor);
	printf("Emission color (%f, %f, %f, %f)\n", 
		emissionColor[0], emissionColor[1], emissionColor[2], emissionColor[3]);

	glGetFloatv(GL_SHININESS, &shinness);
	printf("Shinness value %f.\n", shinness);

	blendFlag = glIsEnabled(GL_BLEND);
	printf("Blending is %s.\n", blendFlag ? "on" : "off");

	texture1DFlag = glIsEnabled(GL_TEXTURE_1D);
	printf("1D texture is %s\n.", texture1DFlag ? "on" : "off");

	texture2DFlag = glIsEnabled(GL_TEXTURE_2D);
	printf("2D texture is %s\n", texture2DFlag ? "on" : "off");

	depthFlag = glIsEnabled(GL_DEPTH_TEST);
	printf("Depth test is %s\n", depthFlag ? "on" : "off");

	vertexArrayFlag = glIsEnabled(GL_VERTEX_ARRAY);
	printf("Vertex array is %s\n", vertexArrayFlag ? "on" : "off");
	
	normalArrayFlag = glIsEnabled(GL_NORMAL_ARRAY);
	printf("Normal array is %s\n", normalArrayFlag ? "on" : "off");

	textureCoordArrayFlag = glIsEnabled(GL_TEXTURE_COORD_ARRAY);
	printf("Texture coord array is %s\n", textureCoordArrayFlag ? "on" : "off");

}

#endif