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
	unsigned short *buffer;
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
	buffer = new unsigned short[lsize/2];

	success = _read(iFile, buffer, (lsize*sizeof(char)));

	// convert RAW 16-bit values to float
	for (unsigned int y=0; y<iHeight; y++)					
		for (unsigned int x=0; x<iWidth; x++)
		{
			m_pTerrainData->m_pHeightmap[y*iHeight+x] =  
				(float)(((unsigned short *)buffer)[y*iHeight+x])*m_pTerrainData->m_fDeltaZ;
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
	unsigned short *buffer;
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
	m_pTerrainData->m_iMeshWidth = iWidth;
	m_pTerrainData->m_pHeightmap = new float[lsize/2];
	buffer = new unsigned short[lsize/2];

	success = _read(iFile, buffer, (lsize*sizeof(char)));

	// convert RAW 16-bit values to float
	for (unsigned int y=0; y<iHeight; y++)					
		for (unsigned int x=0; x<iWidth; x++)
		{
			m_pTerrainData->m_pHeightmap[y*iHeight+x] =  
				(float)(((unsigned short *)buffer)[y*iHeight+x])*m_pTerrainData->m_fDeltaZ;
		} 
	_close(iFile);
	delete []buffer;
	srand(1);
	// Set the fractal parameters to default
	m_pTerrainData->m_fFactorD = 1.0;

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
	m_pTerrainData->m_iMeshWidth = (m_pTerrainData->m_iMeshWidth-1) * 2 + 1;
	m_pTerrainData->m_iMeshGridDistance /= 2;
	BuildVertexArray(m_pTerrainData->m_iMeshWidth);
}

void CTerrain::CalcTriangleNormal(float afNormal[], float afVec1[], float afVec2[], float afVec3[])
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
	int index = 0;
	
/*	if(m_pTerrainData->m_paNormalsL)
		delete m_pTerrainData->m_paNormalsL; */
	if(m_pTerrainData->m_paVertices)
		delete m_pTerrainData->m_paVertices; 

	m_pTerrainData->m_paNormals = new float*[MeshWidth*MeshWidth];
	m_pTerrainData->m_paVertices = new float*[MeshWidth*MeshWidth];
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

}

