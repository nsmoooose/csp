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

#include "BaseCode.h"
#include "Terrain.h"
#include "TerrainData.h"
#include "Fractal.h"
#include "Camera.h"

extern CTerrain		*m_pTerrain;
extern CTerrainData *m_pTerrainData;
extern CCamera		*m_pCamera;

CTerrain::CTerrain()
{
	// Terrain Data Instance contains all parameters for terrain rendering
	m_pTerrainData = new CTerrainData();
}

CTerrain::~CTerrain()
{
	if(m_pTerrainData)				
	   delete m_pTerrainData;		
}

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
	// Set the fractal parameters to default
	m_pTerrainData->m_fFactorD = 0.2;

	BuildVertexArray(m_pTerrainData->m_iMeshWidth);

	return;
}

void CTerrain::LoadDefaultMesh()
{
	int index = 0;
	float m_fDefaultMesh1[] =
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
	m_pTerrainData->m_iMeshWidth = 9;
	m_pTerrainData->m_iMeshGridDistance = m_pTerrainData->m_iMeshGridDistanceBasis;
	m_pTerrainData->m_fFactorD = 1.0;
	srand(1);

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
	int iVertexIndex = 0, iHeightmapIndex = 0, iMeshWidthOld;

	// check if there's already a Vertex Array, and delete it
	if(m_pTerrainData->m_paNormals)
	{
		iMeshWidthOld = m_pTerrainData->m_iMeshWidthOld;
/*		for(int y=0; y<iMeshWidthOld; y++)
			for(int x=0; x<iMeshWidthOld; x++)
			{
				index = y*iMeshWidthOld+x;
				delete m_pTerrainData->m_paNormals[index];
				delete m_pTerrainData->m_paVertices[index];
				delete m_pTerrainData->m_paVertexColours[index];
			} 
*/		delete[] m_pTerrainData->m_paNormals;
		delete[] m_pTerrainData->m_paVertices;
		delete[] m_pTerrainData->m_paVertexColours; 
	}

	m_pTerrainData->m_paNormals = new float[MeshWidth*MeshWidth*3];
	m_pTerrainData->m_paVertices = new float[MeshWidth*MeshWidth*3];
	m_pTerrainData->m_paVertexColours = new float[MeshWidth*MeshWidth*3];
	
	float vec1[3] = { 0, 0, 0 };		// left lower
	float vec2[3] = { 1, 0, 0 };		// right lower
	float vec3[3] = { 0, 1, 0 };		// left upper
	float vec4[3] = { 1, 1, 0 };		// right upper

	for(int y=0; y<MeshWidth; y++)
		for(int x=0; x<MeshWidth; x++)
		{
			iVertexIndex = (y*MeshWidth+x) * 3;
			iHeightmapIndex = y*MeshWidth+x;

/*			m_pTerrainData->m_paVertices[index] = new float[3];
			m_pTerrainData->m_paNormals[index] = new float[3];
			m_pTerrainData->m_paVertexColours[index] = new float[3];
			
*/			m_pTerrainData->m_paVertices[iVertexIndex] = (float)x * m_pTerrainData->m_iMeshGridDistance;
			m_pTerrainData->m_paVertices[iVertexIndex+1] = (float)y * m_pTerrainData->m_iMeshGridDistance;
			m_pTerrainData->m_paVertices[iVertexIndex+2] = m_pTerrainData->m_pHeightmap[iHeightmapIndex];
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

}

/*void CTerrain::BuildVertexArray(int MeshWidth)
{
	int index = 0, iMeshWidthOld;

	// check if there's already a Vertex Array, and delete it
	if(m_pTerrainData->m_paNormals)
	{
		iMeshWidthOld = m_pTerrainData->m_iMeshWidthOld;
		for(int y=0; y<iMeshWidthOld; y++)
			for(int x=0; x<iMeshWidthOld; x++)
			{
				index = y*iMeshWidthOld+x;
				delete m_pTerrainData->m_paNormals[index];
				delete m_pTerrainData->m_paVertices[index];
				delete m_pTerrainData->m_paVertexColours[index];
			} 
		delete[] m_pTerrainData->m_paNormals;
		delete[] m_pTerrainData->m_paVertices;
		delete[] m_pTerrainData->m_paVertexColours; 
	}

	m_pTerrainData->m_paNormals = new float*[MeshWidth*MeshWidth];
	m_pTerrainData->m_paVertices = new float*[MeshWidth*MeshWidth];
	m_pTerrainData->m_paVertexColours = new float*[MeshWidth*MeshWidth];
	
	float vec1[3] = { 0, 0, 0 };		// left lower
	float vec2[3] = { 1, 0, 0 };		// right lower
	float vec3[3] = { 0, 1, 0 };		// left upper
	float vec4[3] = { 1, 1, 0 };		// right upper

	for(int y=0; y<MeshWidth; y++)
		for(int x=0; x<MeshWidth; x++)
		{
			index = y*MeshWidth+x;
			m_pTerrainData->m_paVertices[index] = new float[3];
			m_pTerrainData->m_paNormals[index] = new float[3];
			m_pTerrainData->m_paVertexColours[index] = new float[3];
			m_pTerrainData->m_paVertices[index][0] = (float)x * m_pTerrainData->m_iMeshGridDistance;
			m_pTerrainData->m_paVertices[index][1] = (float)y * m_pTerrainData->m_iMeshGridDistance;
			m_pTerrainData->m_paVertices[index][2] = m_pTerrainData->m_pHeightmap[index];
			if(y<MeshWidth-1 && x<MeshWidth-1)
			{
				vec1[2] = m_pTerrainData->m_pHeightmap[index];
				vec2[2] = m_pTerrainData->m_pHeightmap[index+1];
				vec3[2] = m_pTerrainData->m_pHeightmap[index+MeshWidth];
				CalcTriangleNormal(m_pTerrainData->m_paNormals[index], vec1, vec2, vec3);
			}
			else if(x == MeshWidth-1)
			{
				m_pTerrainData->m_paNormals[index][0] = m_pTerrainData->m_paNormals[index-1][0];
				m_pTerrainData->m_paNormals[index][1] = m_pTerrainData->m_paNormals[index-1][1];
				m_pTerrainData->m_paNormals[index][2] = m_pTerrainData->m_paNormals[index-1][2];
			}
			else if(y == MeshWidth-1)
			{
				m_pTerrainData->m_paNormals[index][0] = m_pTerrainData->m_paNormals[index-MeshWidth][0];
				m_pTerrainData->m_paNormals[index][1] = m_pTerrainData->m_paNormals[index-MeshWidth][1];
				m_pTerrainData->m_paNormals[index][2] = m_pTerrainData->m_paNormals[index-MeshWidth][2];
			}
		}

}*/

void CTerrain::RenderShaded()
{
	short x, y, gd;
	int tricount = 0, vertcount = 0, index = 0;

	float mat_specular[] = { 0, 0, 0, 0 };
	float mat_diffuse[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	float white_light[] = { 1.0, 1.0, 1.0, 1.0 };
	float col_ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };

	gd = m_pTerrainData->m_iMeshGridDistance;

	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_diffuse);

	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
	glLightfv(GL_LIGHT0, GL_AMBIENT, col_ambient);

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer
	glLoadIdentity ();											// Reset The Modelview Matrix
	gluLookAt(0, 0, 0, 0, 1, 0, 0, 0, 1);

	glRotatef(m_pCamera->m_fRotY, -1.0f, 0.0f, 0.0f);
	glRotatef(m_pCamera->m_fRotX, 0.0f, 0.0f, -1.0f); 
	glTranslatef (-m_pCamera->m_fPosX, -m_pCamera->m_fPosY, -m_pCamera->m_fPosZ);

	// this is used to show the position of the light source
	glDisable(GL_LIGHTING);
	glPointSize(1000.0f);
	glBegin(GL_POINTS);
	glColor3f(1.0, 1.0, 0.0);
	glVertex3fv(m_pTerrainData->m_faLightPos);
	glEnd();

	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_POSITION, m_pTerrainData->m_faLightPos);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, m_pTerrainData->m_paVertices);
	glNormalPointer(GL_FLOAT, 0, m_pTerrainData->m_paNormals);

	for(y=0; y<m_pTerrainData->m_iMeshWidth-1;y++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for(x=0; x<m_pTerrainData->m_iMeshWidth-1; x++)
		{
			glArrayElement(y*m_pTerrainData->m_iMeshWidth+x);
			glArrayElement(y*m_pTerrainData->m_iMeshWidth+x+1);
			glArrayElement((y+1)*m_pTerrainData->m_iMeshWidth+x);
			glArrayElement((y+1)*m_pTerrainData->m_iMeshWidth+x+1);
			tricount+=2;
			vertcount+=6; 
		}
		glEnd();

	}
			
	glDisable(GL_LIGHTING);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	glColor3f(0.0, 0.0, 0.0);
	OGLText.Print(0, 0, "Iteration: %i", m_pTerrainData->m_iIterationLevel);
	OGLText.Print(0, 1, "FactorD:	%f", m_pTerrainData->m_fFactorD);
	OGLText.Print(0, 2, "FPS:		%i, Vertices: %i, Triangles: %i", m_pTerrainData->m_BenchRes.iFPSnow, vertcount, tricount);
	OGLText.Print(0, 3, "Sun X:		%i, Sun Y:	  %i, Sun Z		%i", 
				  (int)m_pTerrainData->m_faLightPos[0], (int)m_pTerrainData->m_faLightPos[1],
				  (int)m_pTerrainData->m_faLightPos[2]);

	glFlush ();													// Flush The GL Rendering Pipeline
}

void CTerrain::RenderSquares()
{
	short x, y, gd;
	int tricount = 0, vertcount = 0;

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
	glTranslatef (-m_pCamera->m_fPosX, -m_pCamera->m_fPosY, -m_pCamera->m_fPosZ);

/*	for(y=0; y<m_pTerrainData->m_iMeshWidth-1;y++)
		for(x=0; x<m_pTerrainData->m_iMeshWidth-1; x++)
		{
			glBegin(GL_LINE_STRIP);
			glVertex3fv(m_pTerrainData->m_paVertices[y*m_pTerrainData->m_iMeshWidth+x]);		// left lower 
			glVertex3fv(m_pTerrainData->m_paVertices[y*m_pTerrainData->m_iMeshWidth+x+1]);		// right lower
			glVertex3fv(m_pTerrainData->m_paVertices[(y+1)*m_pTerrainData->m_iMeshWidth+x+1]);    // left upper
			glVertex3fv(m_pTerrainData->m_paVertices[(y+1)*m_pTerrainData->m_iMeshWidth+x]);    // left upper
			glEnd();

			tricount+=1;
			vertcount+=4; 
		}
*/
	glColor3i(0, 0, 0);
	OGLText.Print(0, 0, "Iteration: %i", m_pTerrainData->m_iIterationLevel);
	OGLText.Print(0, 1, "FactorD:	%f", m_pTerrainData->m_fFactorD);
	OGLText.Print(0, 2, "FPS:		%i, Vertices: %i, Triangles: %i", m_pTerrainData->m_BenchRes.iFPSnow, vertcount, tricount);

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
	glTranslatef (-m_pCamera->m_fPosX, -m_pCamera->m_fPosY, -m_pCamera->m_fPosZ);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, m_pTerrainData->m_paVertices);

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

	glColor3f(0.0, 0.0, 0.0);
	OGLText.Print(0, 0, "Iteration: %i", m_pTerrainData->m_iIterationLevel);
	OGLText.Print(0, 1, "FactorD:	%f", m_pTerrainData->m_fFactorD);
	OGLText.Print(0, 2, "FPS:		%i, Vertices: %i, Triangles: %i", m_pTerrainData->m_BenchRes.iFPSnow, vertcount, tricount);
	OGLText.Print(0, 3, "Sun X:		%i, Sun Y:	  %i, Sun Z		%i", 
				  (int)m_pTerrainData->m_faLightPos[0], (int)m_pTerrainData->m_faLightPos[1],
				  (int)m_pTerrainData->m_faLightPos[2]);

	glFlush ();													// Flush The GL Rendering Pipeline
}

