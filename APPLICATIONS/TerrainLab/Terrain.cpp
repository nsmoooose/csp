///////////////////////////////////////////////////////////////////////////
//
//   TerrainLab  - CSP - http://csp.homeip.net
//
//   coded by Stormbringer and		zzed
//   email:   storm_bringer@gmx.li	zzed@gmx.li
//
///////////////////////////////////////////////////////////////////////////
//
//   Implementation of Class CTerrain
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	09/25/2001	file created		-	Stormbringer
//	09/27/2001  Iterate modified	-	Stormbringer
//
//
///////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <math.h>
#include <io.h>
#include <stdlib.h>
#include <fcntl.h>
#include <memory.h>
#include <windows.h>
#include <gl\gl.h>										// Header File For The OpenGL32 Library
#include <gl\glu.h>										// Header File For The GLu32 Library
#include <gl\glaux.h>
												// Header File For The OpenGL32 Library
#include "glext.h"
//#include "glh_genext.h"

#include "BaseCode.h"
#include "Terrain.h"
#include "TerrainData.h"
#include "Fractal.h"
#include "Camera.h"
#include "EcoSystem.h"
#include "Texture.h"

extern CTerrain		*m_pTerrain;
extern CTerrainData *m_pTerrainData;
extern CCamera		*m_pCamera;
extern short		 texturesAvailable;

CTerrain::CTerrain()
{
	texturesAvailable = UNKNOWN;
	// Terrain Data Instance contains all parameters for terrain rendering
	m_pTerrainData = new CTerrainData();
	InitEcosystems();
}

CTerrain::~CTerrain()
{
	if(m_pTerrainData)				
	   delete m_pTerrainData;		
}

/*
void CTerrain::LoadRAWData(char *filename)
{
	int success;
	long lsize, iFile;
	short *buffer;
	float fwidth;

	unsigned int iWidth, iHeight;

	if(m_pTerrainData->m_pHeightmap)
		delete m_pTerrainData->m_pHeightmap;

	// Load TerraGen raw data and save to internal structure
	iFile = _open(filename, _O_BINARY);
	if(iFile==-1)
	{	
		printf("unable to open '%s'!\n", filename);
		char text[100];
		gets(text);
		exit(1);
	}

	lsize = _filelength(iFile);

	fwidth = (float)sqrt((float)lsize/2);			// compute terrain width from size of file
	iWidth = (int)fwidth;
	iHeight = (int)fwidth;

	m_pTerrainData->m_iMeshGridDistance = m_pTerrainData->m_iMeshGridDistanceBasis;
	m_pTerrainData->m_iMeshWidth = iWidth;
	m_pTerrainData->m_pHeightmap = new float[lsize/2];
	buffer = new short[lsize/2];

	success = _read(iFile, buffer, (lsize*sizeof(char)));

	// convert RAW 16-bit values to float
	for (unsigned int y=0; y<iHeight; y++)					
		for (unsigned int x=0; x<iWidth; x++)
		{
			m_pTerrainData->m_pHeightmap[y*iHeight+x] =  
				(float)(((short *)buffer)[y*iHeight+x])*m_pTerrainData->m_fDeltaZ;
		} 
	_close(iFile);
	delete []buffer;
	srand(1);

	return;
}
*/

void CTerrain::LoadRAWData()
{
	int success;
	long lsize, iFile;
	short *buffer;
	float fwidth;

	unsigned int iWidth, iHeight;

	if(m_pTerrainData->m_pHeightmap)
		delete m_pTerrainData->m_pHeightmap;

	// Load TerraGen raw data and save to internal structure
	iFile = _open(m_pTerrainData->m_sMeshFileName, _O_BINARY);
	if(iFile==-1)
	{	
		printf("unable to open '%s'!\n", m_pTerrainData->m_sMeshFileName);
		char text[100];
		gets(text);
		exit(1);
	}

	lsize = _filelength(iFile);

	fwidth = (float)sqrt((float)lsize/2);			// compute terrain width from size of file
	iWidth = (int)fwidth;
	iHeight = (int)fwidth;

	m_pTerrainData->m_iMeshGridDistance = m_pTerrainData->m_iMeshGridDistanceBasis;
	m_pTerrainData->m_iMeshWidthOld = m_pTerrainData->m_iMeshWidth;
	m_pTerrainData->m_iMeshWidth = iWidth;
	m_pTerrainData->m_pHeightmap = new float[lsize/2];
	m_pTerrainData->m_fMaxAlt = 0;
	buffer = new short[lsize/2];

	success = _read(iFile, buffer, (lsize*sizeof(char)));

	// convert RAW 16-bit values to float
	for (unsigned int y=0; y<iHeight; y++)					
		for (unsigned int x=0; x<iWidth; x++)
		{
			m_pTerrainData->m_pHeightmap[y*iHeight+x] =  
				(float)(((short *)buffer)[y*iHeight+x])*m_pTerrainData->m_fDeltaZ;
			if(m_pTerrainData->m_pHeightmap[y*iHeight+x] > m_pTerrainData->m_fMaxAlt)
				m_pTerrainData->m_fMaxAlt = m_pTerrainData->m_pHeightmap[y*iHeight+x];
			else if(m_pTerrainData->m_pHeightmap[y*iHeight+x] < m_pTerrainData->m_fMinAlt)
				m_pTerrainData->m_fMinAlt = m_pTerrainData->m_pHeightmap[y*iHeight+x];
		}
	_close(iFile);
	delete []buffer;
	srand(1);
	// Set the fractal parameters to default
	m_pTerrainData->m_fFactorD = 0.2;

	ScaleHeightmap();
	BuildVertexArray(m_pTerrainData->m_iMeshWidth);

	return;
}

void CTerrain::ScaleHeightmap()
{
	/*  this function scales the heightmap values read from a file
		to match up with the max/min height values used in the ecotope 
		definitions. the final goal should be defining the ecotope 
		parameters according to real world data, so both sources should 
		match without modifications
 
	
	there might be changes to max/min altitude by the smoothing function,
	but we'll ignore them at the moment */

	float fRangeEcotopeHeight, fRangeHeightmap, fScalingFactor;
	float fMaxEcotopeHeight, fMinEcotopeHeight, fMaxHeightmap, fMinHeightmap;
	int iNumVertices;

	fMaxEcotopeHeight = 350.0;	// later these values should be calculated from actual data 
	fMinEcotopeHeight = -50.0;

	fMaxHeightmap = m_pTerrainData->m_fMaxAlt;
	fMinHeightmap = m_pTerrainData->m_fMinAlt;

	fRangeEcotopeHeight = fMaxEcotopeHeight - fMinEcotopeHeight;
	fRangeHeightmap = m_pTerrainData->m_fMaxAlt - m_pTerrainData->m_fMinAlt;

	fScalingFactor = fRangeEcotopeHeight / fRangeHeightmap;
	m_pTerrainData->m_fScalingFactor = fScalingFactor;

	iNumVertices = m_pTerrainData->m_iMeshWidth * m_pTerrainData->m_iMeshWidth;

	m_pTerrainData->m_iMeshGridDistance *= fScalingFactor;

	for(int i=0; i<iNumVertices; i++)
		m_pTerrainData->m_pHeightmap[i] = fScalingFactor * (m_pTerrainData->m_pHeightmap[i] -
										  fMinHeightmap) + fMinEcotopeHeight;

	return;
}

void CTerrain::LoadDefaultMesh()
{
	int index = 0;

	float m_fDefaultMesh1[] =
	{
		000, 100, 300, 400, 500, 400, 300, 100, 000,
		000, 075, 175, 375, 475, 375, 175, 075, 000,
		000, 050, 150, 250, 350, 250, 150, 050, 000,
		000, 025, 125, 225, 325, 225, 125, 025, 000,
		000, 000, 100, 200, 300, 200, 100, 000, 100,
		000, 000, 075, 175, 275, 175, 075, 000, 000,
		000, 000, 025, 100, 200, 100, 025, 000, 000,
		000, 000, 000, 050, 100, 050, 000, 000, 000,
		000, 000, 000, 000, 050, 000, 000, 000, 000,
		000, 000, 000, 000, 000, 000, 000, 000, 000,
	};  

	/*	float m_fDefaultMesh1[] =
	{
		100, 200, 100, 200, 100, 200, 100, 200, 100,
		200, 100, 200, 100, 200, 100, 200, 100, 200,
		100, 200, 100, 200, 100, 200, 100, 200, 100,
		200, 100, 200, 100, 200, 100, 200, 100, 200,
		100, 200, 100, 200, 100, 200, 100, 200, 100,
		200, 100, 200, 100, 200, 100, 200, 100, 200,
		100, 200, 100, 200, 100, 200, 100, 200, 100,
		200, 100, 200, 100, 200, 100, 200, 100, 200,
		100, 200, 100, 200, 100, 200, 100, 200, 100
	};  
*/
/*	float m_fDefaultMesh1[] =
	{
		100, 200, 100,
		200, 100, 200,
		100, 200, 100
	};*/

/*	float m_fDefaultMesh1[] =
	{
		100, 100, 100, 100, 100, 100, 100, 100, 100,
		100, 100, 100, 100, 100, 100, 100, 100, 100,
		100, 100, 100, 100, 100, 100, 100, 100, 100,
		100, 100, 100, 100, 100, 100, 100, 100, 100,
		100, 100, 100, 100, 100, 100, 100, 100, 100,
		100, 100, 100, 100, 100, 100, 100, 100, 100,
		100, 100, 100, 100, 100, 100, 100, 100, 100,
		100, 100, 100, 100, 100, 100, 100, 100, 100,
		100, 100, 100, 100, 100, 100, 100, 100, 100,
	}; */

	if(m_pTerrainData->m_pHeightmap)
		delete m_pTerrainData->m_pHeightmap;

	m_pTerrainData->m_pHeightmap = new float[sizeof(m_fDefaultMesh1)/sizeof(float)];
	memcpy(m_pTerrainData->m_pHeightmap, m_fDefaultMesh1, sizeof(m_fDefaultMesh1));
	
	// Set fractal and mesh parameters to default, reinitialize the random generator
	m_pTerrainData->m_iMeshWidthOld = m_pTerrainData->m_iMeshWidth;
	m_pTerrainData->m_fMaxAlt = 400;
	m_pTerrainData->m_iMeshWidth = 9;
	m_pTerrainData->m_iMeshGridDistance = m_pTerrainData->m_iMeshGridDistanceBasis;
	m_pTerrainData->m_fFactorD = 1.0;
	srand(1);

	ScaleHeightmap();
	BuildVertexArray(m_pTerrainData->m_iMeshWidth);

	return;
}

void CTerrain::Iterate(float fFactorD)
{
	Fractal.DiamondSquare(fFactorD);
	// Adjust the Width and Distance values to keep the mesh at the same size
	m_pTerrainData->m_iMeshWidthOld = m_pTerrainData->m_iMeshWidth;
	m_pTerrainData->m_iMeshWidth = (m_pTerrainData->m_iMeshWidth-1) * 2 + 1;
	m_pTerrainData->m_iMeshGridDistance /= 2;
	BuildVertexArray(m_pTerrainData->m_iMeshWidth);
	CalcTextureCover();
	return;
}

void CTerrain::CalcTriangleNormal(float *afNormal, float afVec1[], float afVec2[], float afVec3[])
{
	float diff1[3], diff2[3];

	for (int i=0; i<3; i++)
	{
		diff1[i] = afVec2[i] - afVec1[i];
		diff2[i] = afVec3[i] - afVec1[i];
	}

	afNormal[0] = diff1[1]*diff2[2]-diff1[2]*diff2[1];
	afNormal[1] = -diff1[2]*diff2[0]+diff1[0]*diff2[2];
	afNormal[2] = diff1[0]*diff2[1]-diff1[1]*diff2[0];

	// normalize afNormal
	float length;
	length = (float)sqrt(afNormal[0]*afNormal[0]+afNormal[1]*afNormal[1]+afNormal[2]*afNormal[2]);
	for (i=0; i<3; i++) afNormal[i] /= length;
}


void CTerrain::BuildVertexArray(int MeshWidth)
{
	int iVertexIndex = 0, iHeightmapIndex = 0, iTexCoordIndex = 0, iMeshWidthOld;
	float fScalingFactor;
	char* normalMap=NULL;

	fScalingFactor = m_pTerrainData->m_fScalingFactor;

	// check if there are already Vertex, Normal etc. arrays, delete them
	if(m_pTerrainData->m_paNormals)
	{
		iMeshWidthOld = m_pTerrainData->m_iMeshWidthOld;
		delete[] m_pTerrainData->m_paNormals;

	}

	if(m_pTerrainData->m_paVertices)					delete[] m_pTerrainData->m_paVertices;
	if(m_pTerrainData->m_paVertexColoursSmoothEdges)	delete[] m_pTerrainData->m_paVertexColoursSmoothEdges; 
	if(m_pTerrainData->m_paVertexColoursSharpEdges)		delete[] m_pTerrainData->m_paVertexColoursSharpEdges; 
	if(m_pTerrainData->m_paRelElevVertexColours)		delete[] m_pTerrainData->m_paRelElevVertexColours;
	if(m_pTerrainData->m_paSlopeVertexColours)			delete[] m_pTerrainData->m_paSlopeVertexColours;
	if(m_pTerrainData->m_paSlope)						delete[] m_pTerrainData->m_paSlope;
	if(m_pTerrainData->m_paTextureCoords)				delete[] m_pTerrainData->m_paTextureCoords;
	if(m_pTerrainData->m_pcColorArrayForCoverageFactorsSmooth)	delete[] m_pTerrainData->m_pcColorArrayForCoverageFactorsSmooth;
	if(m_pTerrainData->m_pcColorArrayForCoverageFactorsSharp)	delete[] m_pTerrainData->m_pcColorArrayForCoverageFactorsSharp;
	if(m_pTerrainData->m_paTextureForCoverageFactorsSmooth) delete [] m_pTerrainData->m_paTextureForCoverageFactorsSmooth;
	if(m_pTerrainData->m_paTextureForCoverageFactorsSharp) delete [] m_pTerrainData->m_paTextureForCoverageFactorsSharp;
	if(m_pTerrainData->m_paAlphaMapTexture5) delete [] m_pTerrainData->m_paAlphaMapTexture5;
	
	if(m_pTerrainData->m_paCoverageFactorsSmooth)
	{
		for(int i=0;i<m_pTerrainData->m_iNumEcosystems;i++)
		{
			if(m_pTerrainData->m_paCoverageFactorsSmooth[i])
				delete[] m_pTerrainData->m_paCoverageFactorsSmooth[i];
		}
		delete[] m_pTerrainData->m_paCoverageFactorsSmooth;
	}

	if(m_pTerrainData->m_paCoverageFactorsSharp)
	{
		for(int i=0;i<m_pTerrainData->m_iNumEcosystems;i++)
		{
			if(m_pTerrainData->m_paCoverageFactorsSharp[i])
				delete[] m_pTerrainData->m_paCoverageFactorsSharp[i];
		}
		delete[] m_pTerrainData->m_paCoverageFactorsSharp;
	}

	// alloc memory for coverage maps. we need two different maps for smooth 
	// and sharp covers. each texture has its individual map
	m_pTerrainData->m_paCoverageFactorsSmooth = new float*[m_pTerrainData->m_iNumEcosystems];
	m_pTerrainData->m_paCoverageFactorsSharp = new float*[m_pTerrainData->m_iNumEcosystems];
	for(int i=0;i<m_pTerrainData->m_iNumEcosystems;i++)
	{
		m_pTerrainData->m_paCoverageFactorsSmooth[i] = new float[MeshWidth*MeshWidth];
		m_pTerrainData->m_paCoverageFactorsSharp[i] = new float[MeshWidth*MeshWidth];	
	}
	// fill coverage maps with 0
	for(i=0;i<m_pTerrainData->m_iNumEcosystems;i++)
	{
		for(int k=0;k<(MeshWidth*MeshWidth);k++)
		{
			m_pTerrainData->m_paCoverageFactorsSmooth[i][k] = 0.0;
			m_pTerrainData->m_paCoverageFactorsSharp[i][k] = 0.0;
		}
	}

	// alloc memory for all the various arrays we need in this demo
	m_pTerrainData->m_paNormals = new float[MeshWidth*MeshWidth*3];
	m_pTerrainData->m_paVertices = new float[MeshWidth*MeshWidth*3];
	m_pTerrainData->m_paVertexColoursSmoothEdges = new float[MeshWidth*MeshWidth*3];
	m_pTerrainData->m_paVertexColoursSharpEdges = new float[MeshWidth*MeshWidth*3];
	m_pTerrainData->m_paRelElevVertexColours = new float[MeshWidth*MeshWidth*3];
	m_pTerrainData->m_paSlopeVertexColours = new float[MeshWidth*MeshWidth*3];
	m_pTerrainData->m_paSlope = new float[MeshWidth*MeshWidth];
	m_pTerrainData->m_paTextureCoords = new float[MeshWidth*MeshWidth*2];
	m_pTerrainData->m_pcColorArrayForCoverageFactorsSharp = new float[MeshWidth*MeshWidth*4];
	m_pTerrainData->m_pcColorArrayForCoverageFactorsSmooth = new float[MeshWidth*MeshWidth*4];
	m_pTerrainData->m_paTextureForCoverageFactorsSmooth = new float[MeshWidth*MeshWidth*4];
	m_pTerrainData->m_paTextureForCoverageFactorsSharp = new float[MeshWidth*MeshWidth*4];
	m_pTerrainData->m_paAlphaMapTexture5 = new float[MeshWidth*MeshWidth*4];
	
	float vec1[3] = { 0, 0, 0 };		// left lower
	float vec2[3] = { 1, 0, 0 };		// right lower
	float vec3[3] = { 0, 1, 0 };		// left upper
	float vec4[3] = { 1, 1, 0 };		// right upper

	for(int y=0; y<MeshWidth; y++)
		for(int x=0; x<MeshWidth; x++)
		{
			iVertexIndex = (y*MeshWidth+x) * 3;
			iTexCoordIndex = (y*MeshWidth+x) * 2;
			iHeightmapIndex = y*MeshWidth+x;

			m_pTerrainData->m_paVertices[iVertexIndex] = (float)x * m_pTerrainData->m_iMeshGridDistance;
			m_pTerrainData->m_paVertices[iVertexIndex+1] = (float)y * m_pTerrainData->m_iMeshGridDistance;
			m_pTerrainData->m_paVertices[iVertexIndex+2] = m_pTerrainData->m_pHeightmap[iHeightmapIndex];

			m_pTerrainData->m_paTextureCoords[iTexCoordIndex] = (float)x / (float)MeshWidth;
			m_pTerrainData->m_paTextureCoords[iTexCoordIndex+1] = (float)y / (float)MeshWidth;

			if(y<MeshWidth-1 && x<MeshWidth-1)
			{
				vec1[2] = m_pTerrainData->m_pHeightmap[iHeightmapIndex];
				vec2[2] = m_pTerrainData->m_pHeightmap[iHeightmapIndex+1];
				vec3[2] = m_pTerrainData->m_pHeightmap[iHeightmapIndex+MeshWidth];
				CalcTriangleNormal(&m_pTerrainData->m_paNormals[iVertexIndex], vec1, vec2, vec3);
			}
			else if(x == MeshWidth-1)
			{
				m_pTerrainData->m_paNormals[iVertexIndex] = m_pTerrainData->m_paNormals[iVertexIndex-1];
				m_pTerrainData->m_paNormals[iVertexIndex+1] = m_pTerrainData->m_paNormals[iVertexIndex];
				m_pTerrainData->m_paNormals[iVertexIndex+2] = m_pTerrainData->m_paNormals[iVertexIndex+1];
			}
			else if(y == MeshWidth-1)
			{
				m_pTerrainData->m_paNormals[iVertexIndex] = m_pTerrainData->m_paNormals[iVertexIndex-(MeshWidth*3)];
				m_pTerrainData->m_paNormals[iVertexIndex+1] = m_pTerrainData->m_paNormals[iVertexIndex-(MeshWidth*3)+1];
				m_pTerrainData->m_paNormals[iVertexIndex+2] = m_pTerrainData->m_paNormals[iVertexIndex-(MeshWidth*3)+2];
			}
		}

		// first call of SetupTextures() checks if textures are available. if not, it's never called again.
		// Texture images are loaded only on demand within the RenderTextured() function
		if(texturesAvailable == UNKNOWN) 
			SetupTextures();

		BuildRelativeElevationGrid();
		BuildSlopeArray();
		BuildEcosystemsGrid();
		BuildRelElevAndSlopeColourArray();
		CalcTextureCover();
}

void CTerrain::ConfigureMultiPass()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LEQUAL);

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glClientActiveTextureARB(GL_TEXTURE1_ARB);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glClientActiveTextureARB(GL_TEXTURE0_ARB);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);
}

void CTerrain::RenderShaded()
{
	short x, y, gd;
	int tricount = 0, vertcount = 0, index = 0, iNumTextureUnits;

	float mat_specular[] = { 0, 0, 0, 0 };
	float mat_diffuse[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	float white_light[] = { 1.0, 1.0, 1.0, 1.0 };
	float col_ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };

	gd = m_pTerrainData->m_iMeshGridDistance;

	// how many texture units are available?
	glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &iNumTextureUnits);

	glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

//	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
//	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_diffuse);

//	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
//	glLightfv(GL_LIGHT0, GL_AMBIENT, col_ambient);

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer
	glLoadIdentity ();											// Reset The Modelview Matrix
	gluLookAt(0, 0, 0, 0, 1, 0, 0, 0, 1);
	
	glRotatef(m_pCamera->m_fRotY, -1.0f, 0.0f, 0.0f);
	glRotatef(m_pCamera->m_fRotX, 0.0f, 0.0f, -1.0f); 
	glTranslatef (-m_pCamera->m_fPosX, -m_pCamera->m_fPosY, -m_pCamera->m_fPosZ);
	
	// this is needed to upscale the terrain after downscaling it for
	// ecosystem calculation
	glScalef(1.0 / m_pTerrainData->m_fScalingFactor, 1.0 / m_pTerrainData->m_fScalingFactor, 
			 1.0 / m_pTerrainData->m_fScalingFactor);

	// this is used to show the position of the light source
	glDisable(GL_LIGHTING);
	glPointSize(1000.0f);
	glBegin(GL_POINTS);
	glColor3f(1.0, 1.0, 0.0);
	glVertex3fv(m_pTerrainData->m_faLightPos);
	glEnd();

//	glEnable(GL_LIGHTING);
//	glLightfv(GL_LIGHT0, GL_POSITION, m_pTerrainData->m_faLightPos);
//	glEnable(GL_COLOR_MATERIAL);
//	glEnable(GL_RESCALE_NORMAL);

	glEnableClientState(GL_VERTEX_ARRAY);
//	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glNormalPointer(GL_FLOAT, 0, m_pTerrainData->m_paNormals);
	glVertexPointer(3, GL_FLOAT, 0, m_pTerrainData->m_paVertices);

	switch(m_pTerrainData->m_bShadingMode)
	{
	case SHADING_MODE_ECO_COLOR:
		if(m_pTerrainData->m_bEcosystemMode == ECOSYSTEM_MODE_SHARP)
			glColorPointer(3, GL_FLOAT, 0, m_pTerrainData->m_paVertexColoursSharpEdges);
		else
			glColorPointer(3, GL_FLOAT, 0, m_pTerrainData->m_paVertexColoursSmoothEdges);
		break;
	case SHADING_MODE_REL_ELEVATION:
		glColorPointer(3, GL_FLOAT, 0, m_pTerrainData->m_paRelElevVertexColours);
		break;
	case SHADING_MODE_SLOPE:
		glColorPointer(3, GL_FLOAT, 0, m_pTerrainData->m_paSlopeVertexColours);
		break;
	}


	for(y=0; y<m_pTerrainData->m_iMeshWidth-1;y++)
	{
		// the triangle strip to draw the terrain mesh
		glBegin(GL_TRIANGLE_STRIP);
		for(x=0; x<m_pTerrainData->m_iMeshWidth-1; x++)
		{
			glArrayElement(y*m_pTerrainData->m_iMeshWidth+x);
			glArrayElement((y+1)*m_pTerrainData->m_iMeshWidth+x);
			vertcount += 2;
		}
		tricount += m_pTerrainData->m_iMeshWidth*2;
		glEnd();
	}
			
//	glDisable(GL_LIGHTING);
//	glDisable(GL_COLOR_MATERIAL);
//	glDisable(GL_RESCALE_NORMAL);
//	glDisable(GL_CULL_FACE);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glColor3f(0.0, 0.0, 0.0);
	OGLText.Print(0, 0, "Iteration: %i", m_pTerrainData->m_iIterationLevel);
	OGLText.Print(0, 1, "FactorD:	%f", m_pTerrainData->m_fFactorD);
	OGLText.Print(0, 2, "FPS:		%i, Vertices: %i, Triangles: %i", m_pTerrainData->m_BenchRes.iFPSnow, vertcount, tricount);
	OGLText.Print(0, 3, "Sun X:		%i, Sun Y:	  %i, Sun Z		%i", 
				  (int)m_pTerrainData->m_faLightPos[0], (int)m_pTerrainData->m_faLightPos[1],
				  (int)m_pTerrainData->m_faLightPos[2]);

	glFlush ();													// Flush The GL Rendering Pipeline
}

void CTerrain::RenderTextured()
{
	short x, y, gd, Width;
	int tricount = 0, vertcount = 0, index = 0, iNumTextureUnits, iNumTextures, icount;
	float *pcColArraySmooth, *pcColArraySharp, *paTexArraySharp, *paTexArraySmooth;
	float *paAlphaMapTexture5;
	float w1, w2, w3, a1, a2;
	float **pfCoverageFactorsSmooth, **pfCoverageFactorsSharp;
	CTexture *pTextures, *pCoverageTexture;

	static float mat_specular[] = { 0, 0, 0, 0 };
	static float mat_diffuse[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	static float white_light[] = { 1.0, 1.0, 1.0, 1.0 };
	static float col_ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	
	static GLfloat xequalzero[] = { 1.0, 0.0, 0.0, 0.0 };
	static GLfloat yequalzero[] = { 0.0, 1.0, 0.0, 0.0 };

	iNumTextures = m_pTerrainData->m_iNumEcosystems;		// each ecosystem has one texture, this will change in the future
	gd = m_pTerrainData->m_iMeshGridDistance;
	Width = m_pTerrainData->m_iMeshWidth;
	icount = Width*Width;
	pcColArraySmooth = m_pTerrainData->m_pcColorArrayForCoverageFactorsSmooth;
	pcColArraySharp = m_pTerrainData->m_pcColorArrayForCoverageFactorsSharp;
	paTexArraySharp = m_pTerrainData->m_paTextureForCoverageFactorsSharp;
	paTexArraySmooth = m_pTerrainData->m_paTextureForCoverageFactorsSmooth;
	pfCoverageFactorsSmooth = m_pTerrainData->m_paCoverageFactorsSmooth;
	pfCoverageFactorsSharp = m_pTerrainData->m_paCoverageFactorsSharp;
	paAlphaMapTexture5 = m_pTerrainData->m_paAlphaMapTexture5;
	
	// how many texture units are available? (only used for debugging)
	glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &iNumTextureUnits);

	pTextures = m_pTerrainData->m_paTextures;
	pCoverageTexture = m_pTerrainData->m_paCoverageTexture;
	ConfigureMultiPass();

	glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

	glEnable(GL_DEPTH_TEST);

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer
	glLoadIdentity ();											// Reset The Modelview Matrix
	gluLookAt(0, 0, 0, 0, 1, 0, 0, 0, 1);
	
	glRotatef(m_pCamera->m_fRotY, -1.0f, 0.0f, 0.0f);
	glRotatef(m_pCamera->m_fRotX, 0.0f, 0.0f, -1.0f); 
	glTranslatef (-m_pCamera->m_fPosX, -m_pCamera->m_fPosY, -m_pCamera->m_fPosZ);
	
	// this is needed to upscale the terrain after downscaling it for
	// ecosystem calculation
	glScalef(1.0 / m_pTerrainData->m_fScalingFactor, 1.0 / m_pTerrainData->m_fScalingFactor, 
			 1.0 / m_pTerrainData->m_fScalingFactor);

	// this is used to show the position of the light source
/*	glDisable(GL_LIGHTING);
	glPointSize(1000.0f);
	glBegin(GL_POINTS);
	glColor3f(1.0, 1.0, 0.0);
	glVertex3fv(m_pTerrainData->m_faLightPos);
	glEnd();
*/
	// dynamic lighting
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_diffuse);

	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
	glLightfv(GL_LIGHT0, GL_AMBIENT, col_ambient);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glLightfv(GL_LIGHT0, GL_POSITION, m_pTerrainData->m_faLightPos);
	glEnable(GL_RESCALE_NORMAL);

	glNormalPointer(GL_FLOAT, 0, m_pTerrainData->m_paNormals);
	glVertexPointer(3, GL_FLOAT, 0, m_pTerrainData->m_paVertices);

	if(m_pTerrainData->m_bEcosystemMode == ECOSYSTEM_MODE_SHARP)
		glColorPointer(4, GL_FLOAT, 0, pcColArraySharp);
	else
		glColorPointer(4, GL_FLOAT, 0, pcColArraySmooth);
	
	// Pass 1: LERP(C0.RGB, T1, T2)
	// the amount of coverage of tex1 and tex2 is stored as the RGB
	// value in the vertex color array (R == G == B == coverage)
	// texture0 and texture1 are blended in this pass
	glDisable(GL_BLEND);

	// texture 1
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glClientActiveTextureARB(GL_TEXTURE1_ARB);
	glTexCoordPointer(2, GL_FLOAT, 0, m_pTerrainData->m_paTextureCoords);	

	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, pTextures[1].iGLTexHandle);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_INTERPOLATE_ARB);

	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_PREVIOUS_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR);

	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_SRC_COLOR);

	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB_ARB, GL_PRIMARY_COLOR_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB_ARB, GL_SRC_COLOR);

	// texture 0 
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glClientActiveTextureARB(GL_TEXTURE0_ARB);
	glTexCoordPointer(2, GL_FLOAT, 0, m_pTerrainData->m_paTextureCoords);
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, pTextures[0].iGLTexHandle);
	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	for(y=0; y<m_pTerrainData->m_iMeshWidth-1;y++)
	{
		// the triangle strip to draw the terrain mesh
		glBegin(GL_TRIANGLE_STRIP);
		for(x=0; x<m_pTerrainData->m_iMeshWidth-1; x++)
		{
			glArrayElement(y*m_pTerrainData->m_iMeshWidth+x);
			glArrayElement((y+1)*m_pTerrainData->m_iMeshWidth+x);
			vertcount += 2;
		}
		tricount += m_pTerrainData->m_iMeshWidth*2;
		glEnd();
	}

	// Pass 2: LERP(C0.A, T3, FB)
	// texture3 blended with the result of the previous rendering pass
	// blending map is stored as alpha in the vertex color array


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D, pTextures[2].iGLTexHandle);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);

	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR);

	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, GL_PRIMARY_COLOR_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_ARB, GL_ONE_MINUS_SRC_ALPHA);

	
	for(y=0; y<m_pTerrainData->m_iMeshWidth-1;y++)
	{
		// the triangle strip to draw the terrain mesh
		glBegin(GL_TRIANGLE_STRIP);
		for(x=0; x<m_pTerrainData->m_iMeshWidth-1; x++)
		{
			glArrayElement(y*m_pTerrainData->m_iMeshWidth+x);
			glArrayElement((y+1)*m_pTerrainData->m_iMeshWidth+x);
			vertcount += 2;
		}
		tricount += m_pTerrainData->m_iMeshWidth*2;
		glEnd();
	}

	// Pass 3: LERP(AlphaMapTexture4, T4, FB)
	// Alpha Map is stored in ColorPointer
	glColorPointer(4, GL_FLOAT, 0, paTexArraySharp);
	
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D, pTextures[3].iGLTexHandle);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);

	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR);

	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, GL_PRIMARY_COLOR_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_ARB, GL_SRC_ALPHA);

	
	for(y=0; y<m_pTerrainData->m_iMeshWidth-1;y++)
	{
		// the triangle strip to draw the terrain mesh
		glBegin(GL_TRIANGLE_STRIP);
		for(x=0; x<m_pTerrainData->m_iMeshWidth-1; x++)
		{
			glArrayElement(y*m_pTerrainData->m_iMeshWidth+x);
			glArrayElement((y+1)*m_pTerrainData->m_iMeshWidth+x);
			vertcount += 2;
		}
		tricount += m_pTerrainData->m_iMeshWidth*2;
		glEnd();
	}

	// Pass 4: LERP(AlphaMapTexture5, T5, FB)
	// Alpha Map is stored in ColorPointer
	glColorPointer(4, GL_FLOAT, 0, paAlphaMapTexture5);
	
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D, pTextures[4].iGLTexHandle);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);

	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR);

	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, GL_PRIMARY_COLOR_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_ARB, GL_SRC_ALPHA);

	
	for(y=0; y<m_pTerrainData->m_iMeshWidth-1;y++)
	{
		// the triangle strip to draw the terrain mesh
		glBegin(GL_TRIANGLE_STRIP);
		for(x=0; x<m_pTerrainData->m_iMeshWidth-1; x++)
		{
			glArrayElement(y*m_pTerrainData->m_iMeshWidth+x);
			glArrayElement((y+1)*m_pTerrainData->m_iMeshWidth+x);
			vertcount += 2;
		}
		tricount += m_pTerrainData->m_iMeshWidth*2;
		glEnd();
	}

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_2D);

/*	glDisable(GL_BLEND);		
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_RESCALE_NORMAL);
	glDisable(GL_CULL_FACE);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
*/

	glLoadIdentity();
	glColor4f(0.0, 0.0, 0.0, 1.0);
	OGLText.Print(0, 0, "Iteration: %i", m_pTerrainData->m_iIterationLevel);
	OGLText.Print(0, 1, "FactorD:	%f", m_pTerrainData->m_fFactorD);
	OGLText.Print(0, 2, "FPS:		%i, Vertices: %i, Triangles: %i", m_pTerrainData->m_BenchRes.iFPSnow, vertcount, tricount);
	OGLText.Print(0, 3, "Sun X:		%i, Sun Y:	  %i, Sun Z		%i", 
				  (int)m_pTerrainData->m_faLightPos[0], (int)m_pTerrainData->m_faLightPos[1],
				  (int)m_pTerrainData->m_faLightPos[2]);

	glFlush ();													// Flush The GL Rendering Pipeline
}

void CTerrain::RenderTriangles()
{
	short x, y, gd;
	int tricount = 0, vertcount = 0, iVertexIndex = 0;

	float mat_specular[] = { 0, 0, 0, 0 };
	float mat_diffuse[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	float white_light[] = { 1.0, 1.0, 1.0, 1.0 };
	float col_ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };

	gd = m_pTerrainData->m_iMeshGridDistance;

	glDisable(GL_LIGHTING);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer
	glLoadIdentity ();											// Reset The Modelview Matrix
	gluLookAt(0, 0, 0, 0, 1, 0, 0, 0, 1);

	glColor3i(255, 255, 255);
	glRotatef(m_pCamera->m_fRotY, -1.0f, 0.0f, 0.0f);
	glRotatef(m_pCamera->m_fRotX, 0.0f, 0.0f, -1.0f); 
	glTranslatef(-m_pCamera->m_fPosX, -m_pCamera->m_fPosY, -m_pCamera->m_fPosZ);

	// this is needed to upscale the terrain after downscaline it for
	// ecosystem calculation
	glScalef(1.0 / m_pTerrainData->m_fScalingFactor, 1.0 / m_pTerrainData->m_fScalingFactor, 
			 1.0 / m_pTerrainData->m_fScalingFactor);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, m_pTerrainData->m_paVertices);
	glNormalPointer(GL_FLOAT, 0, m_pTerrainData->m_paNormals);
	glColorPointer(3, GL_FLOAT, 0, m_pTerrainData->m_paVertexColoursSharpEdges);

	for(y=0; y<m_pTerrainData->m_iMeshWidth-1;y++)
		for(x=0; x<m_pTerrainData->m_iMeshWidth-1; x++)
		{
			glBegin(GL_LINE_STRIP);
			glArrayElement(y*m_pTerrainData->m_iMeshWidth+x);		// left lower 
			glArrayElement(y*m_pTerrainData->m_iMeshWidth+x+1);		// right lower
			glArrayElement((y+1)*m_pTerrainData->m_iMeshWidth+x);    // left upper
			glEnd();
	
			glBegin(GL_LINE_STRIP);
			glArrayElement(y*m_pTerrainData->m_iMeshWidth+x+1);		// right lower 
			glArrayElement((y+1)*m_pTerrainData->m_iMeshWidth+x+1);  // right upper
			glArrayElement((y+1)*m_pTerrainData->m_iMeshWidth+x);	// left upper
			glEnd();

			tricount+=2;
			vertcount+=6; 
		}
			
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	
	glColor3f(0.0, 0.0, 0.0);
	OGLText.Print(0, 0, "Iteration: %i", m_pTerrainData->m_iIterationLevel);
	OGLText.Print(0, 1, "FactorD:	%f", m_pTerrainData->m_fFactorD);
	OGLText.Print(0, 2, "FPS:		%i, Vertices: %i, Triangles: %i", m_pTerrainData->m_BenchRes.iFPSnow, vertcount, tricount);
	OGLText.Print(0, 3, "Sun X:		%i, Sun Y:	  %i, Sun Z		%i", 
				  (int)m_pTerrainData->m_faLightPos[0], (int)m_pTerrainData->m_faLightPos[1],
				  (int)m_pTerrainData->m_faLightPos[2]);

	glFlush ();													// Flush The GL Rendering Pipeline
}

void CTerrain::InitEcosystems()
{
	m_pTerrainData->m_iNumEcosystems = 5;

	mEcoSystem[9].eco_param.cName = "default";
	mEcoSystem[9].eco_param.fEcosystemColour[0] = 1.0;
	mEcoSystem[9].eco_param.fEcosystemColour[1] = 1.0;
	mEcoSystem[9].eco_param.fEcosystemColour[2] = 1.0;

	mEcoSystem[0].eco_param.cName = "Marshland (light blue)";
	mEcoSystem[0].eco_param.cTextureFileName = "textures/farmland_low.bmp";
//	mEcoSystem[0].eco_param.cTextureFileName = "textures/farmland.bmp";
//	mEcoSystem[0].eco_param.cTextureFileName = "textures/farmland_test.bmp";
	mEcoSystem[0].eco_param.fElevationMax = 100;
	mEcoSystem[0].eco_param.fElevationMin = -50;
	mEcoSystem[0].eco_param.fElevationSharpness = 4;
	mEcoSystem[0].eco_param.fRelativeElevationMax = 0.2;
	mEcoSystem[0].eco_param.fRelativeElevationMin = -1;
	mEcoSystem[0].eco_param.fRelativeElevationSharpness = 4;
	mEcoSystem[0].eco_param.fSlopeMax = 0.2;
	mEcoSystem[0].eco_param.fSlopeMin = 0.0;
	mEcoSystem[0].eco_param.fSlopeSharpness = 1;
	mEcoSystem[0].eco_param.fEcosystemColour[0] = 0.5;
	mEcoSystem[0].eco_param.fEcosystemColour[1] = 0.5;
	mEcoSystem[0].eco_param.fEcosystemColour[2] = 1.0;

	mEcoSystem[1].eco_param.cName = "Small bushes and grass (brown/yellow)";
	mEcoSystem[1].eco_param.cTextureFileName = "textures/bushes_grass_low.bmp";
//	mEcoSystem[1].eco_param.cTextureFileName = "textures/bushes_grass.bmp";
//	mEcoSystem[1].eco_param.cTextureFileName = "textures/bushes_grass_test.bmp";
	mEcoSystem[1].eco_param.fElevationMax = 270;
	mEcoSystem[1].eco_param.fElevationMin = -50;
	mEcoSystem[1].eco_param.fElevationSharpness = 2;
	mEcoSystem[1].eco_param.fRelativeElevationMax = 0.7;
	mEcoSystem[1].eco_param.fRelativeElevationMin = -0.7;
	mEcoSystem[1].eco_param.fRelativeElevationSharpness = 1;
	mEcoSystem[1].eco_param.fSlopeMax = 0.4;
	mEcoSystem[1].eco_param.fSlopeMin = 0.0;
	mEcoSystem[1].eco_param.fSlopeSharpness = 1;
	mEcoSystem[1].eco_param.fEcosystemColour[0] = 1.0;
	mEcoSystem[1].eco_param.fEcosystemColour[1] = 0.8;
	mEcoSystem[1].eco_param.fEcosystemColour[2] = 0.5;

	mEcoSystem[2].eco_param.cName = "Grass on steep slopes (light green)";
	mEcoSystem[2].eco_param.cTextureFileName = "textures/grass_steep_slopes_low.bmp";
//	mEcoSystem[2].eco_param.cTextureFileName = "textures/grass_steep_slopes.bmp";
//	mEcoSystem[2].eco_param.cTextureFileName = "textures/grass_steep_slopes_test.bmp";
	mEcoSystem[2].eco_param.fElevationMax = 350;
	mEcoSystem[2].eco_param.fElevationMin = -50;
	mEcoSystem[2].eco_param.fElevationSharpness = 2;
	mEcoSystem[2].eco_param.fRelativeElevationMax = 1;
	mEcoSystem[2].eco_param.fRelativeElevationMin = 0;
	mEcoSystem[2].eco_param.fRelativeElevationSharpness = 1;
	mEcoSystem[2].eco_param.fSlopeMax = 2.5;
	mEcoSystem[2].eco_param.fSlopeMin = 0.8;
	mEcoSystem[2].eco_param.fSlopeSharpness = 2;
	mEcoSystem[2].eco_param.fEcosystemColour[0] = 0.2;
	mEcoSystem[2].eco_param.fEcosystemColour[1] = 0.7;
	mEcoSystem[2].eco_param.fEcosystemColour[2] = 0.2;

	mEcoSystem[3].eco_param.cName = "Dense bush (medium green)";
	mEcoSystem[3].eco_param.cTextureFileName = "textures/dense_bush_low.bmp";
//	mEcoSystem[3].eco_param.cTextureFileName = "textures/dense_bush.bmp";
//	mEcoSystem[3].eco_param.cTextureFileName = "textures/dense_bush_test.bmp";
	mEcoSystem[3].eco_param.fElevationMax = 300;
	mEcoSystem[3].eco_param.fElevationMin = 0;
	mEcoSystem[3].eco_param.fElevationSharpness = 2;
	mEcoSystem[3].eco_param.fRelativeElevationMax = 0.5;
	mEcoSystem[3].eco_param.fRelativeElevationMin = -0.5;
	mEcoSystem[3].eco_param.fRelativeElevationSharpness = 3.0;
	mEcoSystem[3].eco_param.fSlopeMax = 0.8;
	mEcoSystem[3].eco_param.fSlopeMin = 0.0;
	mEcoSystem[3].eco_param.fSlopeSharpness = 2;
	mEcoSystem[3].eco_param.fEcosystemColour[0] = 0.0;
	mEcoSystem[3].eco_param.fEcosystemColour[1] = 0.5;
	mEcoSystem[3].eco_param.fEcosystemColour[2] = 0.0;

	mEcoSystem[4].eco_param.cName = "Exposed Rock (orange)";
	mEcoSystem[4].eco_param.cTextureFileName = "textures/stone3.bmp";
//	mEcoSystem[4].eco_param.cTextureFileName = "textures/stone3_test.bmp";
	mEcoSystem[4].eco_param.fElevationMax = 400;
	mEcoSystem[4].eco_param.fElevationMin = -50;
	mEcoSystem[4].eco_param.fElevationSharpness = 1;
	mEcoSystem[4].eco_param.fRelativeElevationMax = 1;
	mEcoSystem[4].eco_param.fRelativeElevationMin = -1;
	mEcoSystem[4].eco_param.fRelativeElevationSharpness = 1;
	mEcoSystem[4].eco_param.fSlopeMax = 10.0;
	mEcoSystem[4].eco_param.fSlopeMin = 1.2;
	mEcoSystem[4].eco_param.fSlopeSharpness = 3;
	mEcoSystem[4].eco_param.fEcosystemColour[0] = 0.95;
	mEcoSystem[4].eco_param.fEcosystemColour[1] = 0.25;
	mEcoSystem[4].eco_param.fEcosystemColour[2] = 0.25;


/*	mEcoSystem[0].eco_param.cName = "blue low";
	mEcoSystem[0].eco_param.fElevationMin = -50;
	mEcoSystem[0].eco_param.fElevationMax = 50;
	mEcoSystem[0].eco_param.fElevationSharpness = 2;
	mEcoSystem[0].eco_param.fRelativeElevationMax = 0.0;
	mEcoSystem[0].eco_param.fRelativeElevationMin = -1.0;
	mEcoSystem[0].eco_param.fRelativeElevationSharpness = 1.0;
	mEcoSystem[0].eco_param.fSlopeMax = 0.7;
	mEcoSystem[0].eco_param.fSlopeMin = 0.0;
	mEcoSystem[0].eco_param.fSlopeSharpness = 2;
	mEcoSystem[0].eco_param.fEcosystemColour[0] = 0.0;
	mEcoSystem[0].eco_param.fEcosystemColour[1] = 0.0;
	mEcoSystem[0].eco_param.fEcosystemColour[2] = 1.0;

	mEcoSystem[5].eco_param.cName = "blue high";
	mEcoSystem[5].eco_param.fElevationMin = -50;
	mEcoSystem[5].eco_param.fElevationMax = 50;
	mEcoSystem[5].eco_param.fElevationSharpness = 2;
	mEcoSystem[5].eco_param.fRelativeElevationMin = 0.0;
	mEcoSystem[5].eco_param.fRelativeElevationMax = 1.0;
	mEcoSystem[5].eco_param.fRelativeElevationSharpness = 1.0;
	mEcoSystem[5].eco_param.fSlopeMax = 0.7;
	mEcoSystem[5].eco_param.fSlopeMin = 0.0;
	mEcoSystem[5].eco_param.fSlopeSharpness = 2;
	mEcoSystem[5].eco_param.fEcosystemColour[0] = 0.5;
	mEcoSystem[5].eco_param.fEcosystemColour[1] = 0.5;
	mEcoSystem[5].eco_param.fEcosystemColour[2] = 1.0;

	mEcoSystem[1].eco_param.cName = "green low";
	mEcoSystem[1].eco_param.fElevationMin= 50;
	mEcoSystem[1].eco_param.fElevationMax = 150;
	mEcoSystem[1].eco_param.fElevationSharpness = 2.0;
	mEcoSystem[1].eco_param.fRelativeElevationMin = -1.0;
	mEcoSystem[1].eco_param.fRelativeElevationMax = 0.0;
	mEcoSystem[1].eco_param.fRelativeElevationSharpness = 2.0;
	mEcoSystem[1].eco_param.fSlopeMax = 0.3;
	mEcoSystem[1].eco_param.fSlopeMin = -0.2;
	mEcoSystem[1].eco_param.fSlopeSharpness = 2;
	mEcoSystem[1].eco_param.fEcosystemColour[0] = 0.5;
	mEcoSystem[1].eco_param.fEcosystemColour[1] = 1.0;
	mEcoSystem[1].eco_param.fEcosystemColour[2] = 0.5;

	mEcoSystem[6].eco_param.cName = "green high";
	mEcoSystem[6].eco_param.fElevationMin= 50;
	mEcoSystem[6].eco_param.fElevationMax = 150;
	mEcoSystem[6].eco_param.fElevationSharpness = 2.0;
	mEcoSystem[6].eco_param.fRelativeElevationMin = 0.0;
	mEcoSystem[6].eco_param.fRelativeElevationMax = 1.0;
	mEcoSystem[6].eco_param.fRelativeElevationSharpness = 2.0;
	mEcoSystem[6].eco_param.fSlopeMax = 0.3;
	mEcoSystem[6].eco_param.fSlopeMin = -0.2;
	mEcoSystem[6].eco_param.fSlopeSharpness = 2;
	mEcoSystem[6].eco_param.fEcosystemColour[0] = 0.0;
	mEcoSystem[6].eco_param.fEcosystemColour[1] = 1.0;
	mEcoSystem[6].eco_param.fEcosystemColour[2] = 0.0;

	mEcoSystem[2].eco_param.cName = "red low";
	mEcoSystem[2].eco_param.fElevationMin = 150;
	mEcoSystem[2].eco_param.fElevationMax = 250;
	mEcoSystem[2].eco_param.fElevationSharpness = 2.0;
	mEcoSystem[2].eco_param.fRelativeElevationMin = -1.0;
	mEcoSystem[2].eco_param.fRelativeElevationMax = 0.0;
	mEcoSystem[2].eco_param.fRelativeElevationSharpness = 1;
	mEcoSystem[2].eco_param.fSlopeMax = 0.8;
	mEcoSystem[2].eco_param.fSlopeMin = -0.2;
	mEcoSystem[2].eco_param.fSlopeSharpness = 2;
	mEcoSystem[2].eco_param.fEcosystemColour[0] = 1.0;
	mEcoSystem[2].eco_param.fEcosystemColour[1] = 0.0;
	mEcoSystem[2].eco_param.fEcosystemColour[2] = 0.0;

	mEcoSystem[7].eco_param.cName = "red high";
	mEcoSystem[7].eco_param.fElevationMin = 150;
	mEcoSystem[7].eco_param.fElevationMax = 250;
	mEcoSystem[7].eco_param.fElevationSharpness = 2.0;
	mEcoSystem[7].eco_param.fRelativeElevationMax = 1.0;
	mEcoSystem[7].eco_param.fRelativeElevationMin = 0.0;
	mEcoSystem[7].eco_param.fRelativeElevationSharpness = 1;
	mEcoSystem[7].eco_param.fSlopeMax = 0.8;
	mEcoSystem[7].eco_param.fSlopeMin = -0.2;
	mEcoSystem[7].eco_param.fSlopeSharpness = 2;
	mEcoSystem[7].eco_param.fEcosystemColour[0] = 1.0;
	mEcoSystem[7].eco_param.fEcosystemColour[1] = 0.5;
	mEcoSystem[7].eco_param.fEcosystemColour[2] = 0.5;

	mEcoSystem[3].eco_param.cName = "yellow low";
	mEcoSystem[3].eco_param.fElevationMin = 250;
	mEcoSystem[3].eco_param.fElevationMax = 350;
	mEcoSystem[3].eco_param.fElevationSharpness = 2;
	mEcoSystem[3].eco_param.fRelativeElevationMin = -1;
	mEcoSystem[3].eco_param.fRelativeElevationMax = 0;
	mEcoSystem[3].eco_param.fRelativeElevationSharpness = 1;
	mEcoSystem[3].eco_param.fSlopeMax = 1.2;
	mEcoSystem[3].eco_param.fSlopeMin = 0.7;
	mEcoSystem[3].eco_param.fSlopeSharpness = 2;
	mEcoSystem[3].eco_param.fEcosystemColour[0] = 1.0;
	mEcoSystem[3].eco_param.fEcosystemColour[1] = 1.0;
	mEcoSystem[3].eco_param.fEcosystemColour[2] = 0.5;

	mEcoSystem[8].eco_param.cName = "yellow high";
	mEcoSystem[8].eco_param.fElevationMin = 250;
	mEcoSystem[8].eco_param.fElevationMax = 350;
	mEcoSystem[8].eco_param.fElevationSharpness = 2;
	mEcoSystem[8].eco_param.fRelativeElevationMax = 1;
	mEcoSystem[8].eco_param.fRelativeElevationMin = 0;
	mEcoSystem[8].eco_param.fRelativeElevationSharpness = 1;
	mEcoSystem[8].eco_param.fSlopeMax = 1.2;
	mEcoSystem[8].eco_param.fSlopeMin = 0.7;
	mEcoSystem[8].eco_param.fSlopeSharpness = 2;
	mEcoSystem[8].eco_param.fEcosystemColour[0] = 1.0;
	mEcoSystem[8].eco_param.fEcosystemColour[1] = 1.0;
	mEcoSystem[8].eco_param.fEcosystemColour[2] = 0.0;

	mEcoSystem[4].eco_param.cName = "white";
	mEcoSystem[4].eco_param.fElevationMin = 300;
	mEcoSystem[4].eco_param.fElevationMax = 400;
	mEcoSystem[4].eco_param.fElevationSharpness = 2;
	mEcoSystem[4].eco_param.fRelativeElevationMax = 1;
	mEcoSystem[4].eco_param.fRelativeElevationMin = -1;
	mEcoSystem[4].eco_param.fRelativeElevationSharpness = 1;
	mEcoSystem[4].eco_param.fSlopeMax = 4.2;
	mEcoSystem[4].eco_param.fSlopeMin = 1.4;
	mEcoSystem[4].eco_param.fSlopeSharpness = 8;
	mEcoSystem[4].eco_param.fEcosystemColour[0] = 1.0;
	mEcoSystem[4].eco_param.fEcosystemColour[1] = 1.0;
	mEcoSystem[4].eco_param.fEcosystemColour[2] = 1.0;
*/
	return;
}

void CTerrain::BuildRelativeElevationGrid()
{
	float fLeftUpper, fLeftLower, fRightUpper, fRightLower;
	float fIntervalLeftY, fIntervalRightY, fIntervalX;
	float fRelElevMax = 0, fRelElevMin = 0, fRelElevAverage, fInverseRelElevHalfRange;
	float fStartHeightLeft, fStartHeightRight, fInterpolatedHeight;
	float *pHeightMap, *pRelElevMap;
	int iIndex, iWidth;

	// check for an existing grid and delete it
	if(m_pTerrainData->m_paRelativeElevationGrid)
		delete[] m_pTerrainData->m_paRelativeElevationGrid;

	iWidth = m_pTerrainData->m_iMeshWidth;
	m_pTerrainData->m_paRelativeElevationGrid = new float[iWidth*iWidth];
	pRelElevMap = m_pTerrainData->m_paRelativeElevationGrid;

	pHeightMap = m_pTerrainData->m_pHeightmap;

	// get the height of the four corners of the heightfield
	fLeftLower = pHeightMap[0];
	fRightLower = pHeightMap[iWidth-1];
	fLeftUpper = pHeightMap[(iWidth-1)*iWidth];
	fRightUpper = pHeightMap[(iWidth*iWidth)-1];

	// vertical interval of the left and right heigtmap edge
	fIntervalLeftY = (fLeftUpper - fLeftLower) / (iWidth-1);
	fIntervalRightY = (fRightUpper - fRightLower) / (iWidth-1);

	for(int y=0;y<iWidth;y++)
	{
		iIndex = y*iWidth;
		fStartHeightLeft = fLeftLower + (y*fIntervalLeftY);
		fStartHeightRight = fRightLower + (y*fIntervalRightY);
		fIntervalX = (fStartHeightRight - fStartHeightLeft) / (iWidth-1);

		for(int x=0;x<iWidth;x++)
		{
			fInterpolatedHeight = fStartHeightLeft + (x * fIntervalX);
			pRelElevMap[iIndex+x] = pHeightMap[iIndex+x] - fInterpolatedHeight;
			if(pRelElevMap[iIndex+x] < fRelElevMin)
				fRelElevMin = pRelElevMap[iIndex+x];
			if(pRelElevMap[iIndex+x] > fRelElevMax)
				fRelElevMax = pRelElevMap[iIndex+x];
		}
	}

	/* normalize relative elevation */
	fRelElevAverage = (fRelElevMin + fRelElevMax) / 2.0; 
	if((fRelElevMax - fRelElevMin) != 0)
		fInverseRelElevHalfRange = 2.0 / (fRelElevMax - fRelElevMin); // check non 0
	else
		fInverseRelElevHalfRange = 0.0001;

	for (unsigned int i = 0; i< iWidth*iWidth;++i)
	{
		pRelElevMap[i] -= fRelElevAverage;
		pRelElevMap[i] *=  fInverseRelElevHalfRange;
	}
	
	return;
}

void CTerrain::BuildEcosystemsGrid()
{
	float *pHeightMap, *pRelElevMap, *pColourMap;
	int *pEcoMap;
	int iIndex, iColourMapIndex, iWidth;
	float **pCoverageFactors;

	if(m_pTerrainData->m_pEcosystems)
		delete[] m_pTerrainData->m_pEcosystems;

	iWidth = m_pTerrainData->m_iMeshWidth;
	m_pTerrainData->m_pEcosystems = new int[iWidth*iWidth];
	pRelElevMap = m_pTerrainData->m_paRelativeElevationGrid;
	pEcoMap = m_pTerrainData->m_pEcosystems;
	pCoverageFactors = m_pTerrainData->m_paCoverageFactorsSharp;

	// Colour map for smooth edges is build in CTerrain::CalculateEcosystem()
	pColourMap = m_pTerrainData->m_paVertexColoursSharpEdges;
	pHeightMap = m_pTerrainData->m_pHeightmap;

	for(int y=0;y<iWidth;y++)
	{
		iIndex = y*iWidth;
		for(int x=0;x<iWidth;x++)
		{
			iColourMapIndex = (iIndex+x) * 3;
			pEcoMap[iIndex+x] = CalculateEcosystem(y,x);
			// each ecosystem has a representative colour assigned, this will be replaced
			// by a representative texture later. here sharp edges of ecosystems are drawn.
			// to draw smooth transitions between ecosystems use the colour calculation code
			// change to the appropriate mode with "e" key.
//			if(m_pTerrainData->m_bEcosystemMode == ECOSYSTEM_MODE_SHARP)
//			{
				pColourMap[iColourMapIndex] = mEcoSystem[pEcoMap[iIndex+x]].eco_param.fEcosystemColour[0];
				pColourMap[iColourMapIndex+1] = mEcoSystem[pEcoMap[iIndex+x]].eco_param.fEcosystemColour[1];
				pColourMap[iColourMapIndex+2] = mEcoSystem[pEcoMap[iIndex+x]].eco_param.fEcosystemColour[2];
				pCoverageFactors[pEcoMap[iIndex+x]][iIndex+x] = 1.0;
//			}
		}
	}
}

int CTerrain::CalculateEcosystem(int iYPos, int iXPos)
{
	int iNumEcosystems, iResultingEcosystem, iWidth, iIndex, iColourMapIndex;
	float *fProb, fElevation, fRelativeElevation, fSlope, fSlopeNorm;
	float *pHeightMap, *pRelElevMap, *pColourMap, fProbMax = 0;
	float **pCoverageFactors;
	float fDelX, fDelY, fDelXNorm, fDelYNorm, fSlopeMax, fDeltaMin;
	float s1, s2, s3, s4, s5, s6, s7, s8, fGridDist;

	iNumEcosystems = m_pTerrainData->m_iNumEcosystems;
	fProb = new float[iNumEcosystems];

	iWidth = m_pTerrainData->m_iMeshWidth;
	iIndex = iYPos*iWidth + iXPos;
	pHeightMap = m_pTerrainData->m_pHeightmap;
	pRelElevMap = m_pTerrainData->m_paRelativeElevationGrid;
	pCoverageFactors = m_pTerrainData->m_paCoverageFactorsSmooth;

	// Color map for sharp edges is build in CTerrain::BuildEcosystemsGrid()
	pColourMap = m_pTerrainData->m_paVertexColoursSmoothEdges;
	iColourMapIndex = iIndex * 3;
	fGridDist = m_pTerrainData->m_iMeshGridDistance;

	pColourMap[iColourMapIndex] = 0;
	pColourMap[iColourMapIndex+1] = 0;
	pColourMap[iColourMapIndex+2] = 0;

	fElevation = pHeightMap[iIndex];
	fRelativeElevation = pRelElevMap[iIndex];
	fSlope = m_pTerrainData->m_paSlope[iIndex];
	
	for (int i=0;i<iNumEcosystems;i++)
	{
	//	CalculateSlopeSkew();

		fProb[i] = mEcoSystem[i].CalcEcoWeight(fElevation, fRelativeElevation, fSlope);

		if(fProb[i] > fProbMax)
		{
			fProbMax = fProb[i];
			iResultingEcosystem = i;
		}

		// calculate the colour of the vertex by adding RGB components of all ecosystem
		// considering their probability
//		if(m_pTerrainData->m_bEcosystemMode == ECOSYSTEM_MODE_SMOOTH)
//		{
			pColourMap[iColourMapIndex] += mEcoSystem[i].eco_param.fEcosystemColour[0] * fProb[i];
			pColourMap[iColourMapIndex+1] += mEcoSystem[i].eco_param.fEcosystemColour[1] * fProb[i];
			pColourMap[iColourMapIndex+2] += mEcoSystem[i].eco_param.fEcosystemColour[2] * fProb[i];
			pCoverageFactors[i][iIndex] = fProb[i];
//		}
	}

	// use 9 as default ecosystem
	if(fProbMax == 0)
	{
		iResultingEcosystem = 9;
	}
	
	delete[] fProb;
	return iResultingEcosystem;
}

void CTerrain::BuildRelElevAndSlopeColourArray()
{
	float *pRelElevMap, *pSlopeMap, *pSlopeColorMap, *pRelElevColorMap;
	int iIndex, iColourMapIndex, iWidth;

	iWidth = m_pTerrainData->m_iMeshWidth;
	pRelElevMap = m_pTerrainData->m_paRelativeElevationGrid;
	pSlopeMap = m_pTerrainData->m_paSlope;
	pRelElevColorMap = m_pTerrainData->m_paRelElevVertexColours;
	pSlopeColorMap = m_pTerrainData->m_paSlopeVertexColours;
	
	// we use red and blue color to illustrate what is above and below the average
	// at a certain heightmap point
	for(int y=0;y<iWidth;y++)
	{
		iIndex = y*iWidth;
		for(int x=0;x<iWidth;x++)
		{
			iColourMapIndex = (iIndex+x) * 3;
			if(pRelElevMap[iIndex+x] >= 0)
			{
				pRelElevColorMap[iColourMapIndex] = (pRelElevMap[iIndex+x] + 1.0) / 2;
				pRelElevColorMap[iColourMapIndex+1] = (pRelElevMap[iIndex+x] + 1.0) / 2;
				pRelElevColorMap[iColourMapIndex+2] = 1.0;
			}
			else
			{
				pRelElevColorMap[iColourMapIndex] = 1.0;
				pRelElevColorMap[iColourMapIndex+1] = (pRelElevMap[iIndex+x] + 1.0) / 2;
				pRelElevColorMap[iColourMapIndex+2] = (pRelElevMap[iIndex+x] + 1.0) / 2;
			}

			if(pSlopeMap[iIndex+x] >= 0)
			{
				pSlopeColorMap[iColourMapIndex] = (pSlopeMap[iIndex+x] + 1.0) / 2;
				pSlopeColorMap[iColourMapIndex+1] = (pSlopeMap[iIndex+x] + 1.0) / 2;
				pSlopeColorMap[iColourMapIndex+2] = 1.0;
			}
			else
			{
				pSlopeColorMap[iColourMapIndex] = (pSlopeMap[iIndex+x] + 1.0) / 2;
				pSlopeColorMap[iColourMapIndex+1] = (pSlopeMap[iIndex+x] + 1.0) / 2;
				pSlopeColorMap[iColourMapIndex+2] = 1.0;
			}
		}
	}
}

void CTerrain::BuildSlopeArray()
{
	float *pHeightMap, *pSlopeArray;
	float fDelX, fDelY, fGridDist, fSlope;
	float fSlopeMin = 10, fSlopeMax = -10;
	int iIndex, iWidth;

	pSlopeArray = m_pTerrainData->m_paSlope;
	iWidth = m_pTerrainData->m_iMeshWidth;
	fGridDist = m_pTerrainData->m_iMeshGridDistance;
	pHeightMap = m_pTerrainData->m_pHeightmap;

	for(int iYPos=0;iYPos<iWidth;iYPos++)
	{
		for(int iXPos=0;iXPos<iWidth;iXPos++)
		{
			iIndex = (iYPos*iWidth)+iXPos;

			// first in row
			if(iXPos == 0)
			fDelX = (pHeightMap[iIndex+1] - pHeightMap[iIndex]) / fGridDist / 2;
			// last in row
			else if(iXPos == iWidth-1)
				fDelX = (pHeightMap[iIndex] - pHeightMap[iIndex-1]) / fGridDist / 2;
			// somewhere in the middle
			else
				fDelX = (pHeightMap[iIndex+1] - pHeightMap[iIndex])/ fGridDist;
		
			// first in column
			if(iYPos == 0)
				fDelY = (pHeightMap[iIndex+iWidth] - pHeightMap[iIndex]) / fGridDist / 2;
			// last in column
			else if(iYPos == iWidth-1)
				fDelY = (pHeightMap[iIndex] - pHeightMap[iIndex-iWidth]) / fGridDist / 2;
			else
				fDelY = (pHeightMap[iIndex+iWidth] - pHeightMap[iIndex-iWidth]) / fGridDist;

			fDelX = fabs(fDelX);
			fDelY = fabs(fDelY);
//			fSlope = max(fDelX, fDelY);
			fSlope = fDelX+fDelY;
			if(fSlope < fSlopeMin) fSlopeMin = fSlope;
			else if(fSlope > fSlopeMax) fSlopeMax = fSlope;

			pSlopeArray[iIndex] = fSlope;
		}
	}
	return;
}

void CTerrain::SetupTextures()
{
	CTexture *pTextures, *pCoverageTexture;
	AUX_RGBImageRec *Image=NULL;
	int iNumTextures, i;
	char *cTexFileName, *alpha=NULL;

	iNumTextures = m_pTerrainData->m_iNumEcosystems + 1; // one more texture for coverage map storage
	pTextures = new CTexture[iNumTextures];
	m_pTerrainData->m_paTextures = pTextures;

	pCoverageTexture = new CTexture;

	for(i=0;i<iNumTextures-1;i++) // initialize the "real" textures from bmp images
	{
		if(!(Image=auxDIBImageLoad(mEcoSystem[i].eco_param.cTextureFileName)))
		{
			texturesAvailable = FALSE;
			return;
		}
		else
		{
			alpha=new char[4*Image->sizeX*Image->sizeY];		// Create Memory For RGBA8-Texture
			for (int a=0; a<Image->sizeX*Image->sizeY; a++)
			{
				alpha[4*a]=Image->data[a*3];					// R
				alpha[4*a+1]=Image->data[a*3+1];				// G
				alpha[4*a+2]=Image->data[a*3+2];				// B
				alpha[4*a+3]=Image->data[a*3];					// Pick Only Red Value As Alpha!
			}

			glGenTextures(1, &pTextures[i].iGLTexHandle);
			glBindTexture(GL_TEXTURE_2D, pTextures[i].iGLTexHandle);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, Image->sizeX, Image->sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, alpha);
			gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA8, Image->sizeX, Image->sizeY, GL_RGBA, GL_UNSIGNED_BYTE, alpha);
			pTextures[i].bIsAvailable = TRUE;
		}

		if (Image) 
		{													// If Texture Exists
			if (Image->data) delete Image->data;			// If Texture Image Exists
			delete Image;
			Image=NULL;
		}
	}

	// create the texture needed for coverage maps
	glGenTextures(1, &pCoverageTexture->iGLTexHandle);
	pCoverageTexture->bIsAvailable = TRUE;
	m_pTerrainData->m_paCoverageTexture = pCoverageTexture;

	texturesAvailable = TRUE;

	return;
}

void CTerrain::CalcTextureCover()
{
	float *pcColArraySmooth, *pcColArraySharp, *paTexArraySharp, *paTexArraySmooth;
	float **pfCoverageFactorsSmooth, **pfCoverageFactorsSharp;
	float *paAlphaMapTexture5;
	float w1, w2, w3, a1, a2;
	int icount, Width, iNumTextures, i;
	CTexture *pCoverageTexture;

	Width = m_pTerrainData->m_iMeshWidth;
	icount = Width*Width;
	iNumTextures = m_pTerrainData->m_iNumEcosystems;
	pcColArraySmooth = m_pTerrainData->m_pcColorArrayForCoverageFactorsSmooth;
	pcColArraySharp = m_pTerrainData->m_pcColorArrayForCoverageFactorsSharp;
	paTexArraySharp = m_pTerrainData->m_paTextureForCoverageFactorsSharp;
	paTexArraySmooth = m_pTerrainData->m_paTextureForCoverageFactorsSmooth;
	pfCoverageFactorsSmooth = m_pTerrainData->m_paCoverageFactorsSmooth;
	pfCoverageFactorsSharp = m_pTerrainData->m_paCoverageFactorsSharp;
	pCoverageTexture = m_pTerrainData->m_paCoverageTexture;
	paAlphaMapTexture5 = m_pTerrainData->m_paAlphaMapTexture5;

	// copy coverage factors to the color array as if they were "real" colors
	// they are used as blending factors with GL_INTERPOLATE further down
	// we have two different arrays for distinct ecosystem and mixed ecosystems
	// per heightmap point so it's possible to switch between the at runtime
	// (key 'e') to illustrate the difference
	
	// coverage values as blending factors for textures 1,2 and 3 are copied to the 
	// vertex color/alpha array. Be aware that the color array isn't used for
	// actual COLOR or ALPHA information!

	// array for smooth terrain transitions
	for(i=0;i<icount;i++)
	{
		w1 = pfCoverageFactorsSmooth[0][i];
		w3 = pfCoverageFactorsSmooth[2][i];
		if(w3 == 1) w3 = 0.999;

		a1 = w1/(1.0-w3);
		a2 = 1.0-w3;

		if(a1 > 1) a1 = 1.0;
		if(a2 > 1) a2 = 1.0;

		pcColArraySmooth[4*i] = a1;
		pcColArraySmooth[4*i+1] = a1;
		pcColArraySmooth[4*i+2] = a1;
		pcColArraySmooth[4*i+3] = a2;
	}
	
	// array for sharp terrain transitions
	for(i=0;i<icount;i++)
	{
		w1 = pfCoverageFactorsSharp[0][i];
		w3 = pfCoverageFactorsSharp[2][i];
		if(w3 == 1) w3 = 0.999;

		a1 = w1/(1.0-w3);
		a2 = 1.0-w3;

		if(a1 > 1) a1 = 1.0;
		if(a2 > 1) a2 = 1.0;

		pcColArraySharp[4*i] = a1;
		pcColArraySharp[4*i+1] = a1;
		pcColArraySharp[4*i+2] = a1;
		pcColArraySharp[4*i+3] = a2;
	}

	// the covering factors for textures 4 and 5 are stored
	// as texture color RGB information and alpha

	for(i=0;i<icount;i++)
	{
		// texture 4
		paTexArraySharp[4*i] = pfCoverageFactorsSharp[3][i];
		paTexArraySharp[4*i+1] = pfCoverageFactorsSharp[3][i];
		paTexArraySharp[4*i+2] = pfCoverageFactorsSharp[3][i];
		paTexArraySharp[4*i+3] = pfCoverageFactorsSharp[3][i];

		// texture 5
		paAlphaMapTexture5[4*i] = pfCoverageFactorsSharp[4][i];
		paAlphaMapTexture5[4*i+1] = pfCoverageFactorsSharp[4][i];
		paAlphaMapTexture5[4*i+2] = pfCoverageFactorsSharp[4][i];
		paAlphaMapTexture5[4*i+3] = pfCoverageFactorsSharp[4][i];
	}


	// finally copy the array to the texture image
/*	glBindTexture(GL_TEXTURE_2D, pCoverageTexture->iGLTexHandle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, Width, Width, 0, GL_RGBA, GL_FLOAT, paTexArraySharp);
*/
	return;
}