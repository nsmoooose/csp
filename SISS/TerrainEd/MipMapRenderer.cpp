///////////////////////////////////////////////////////////////////////////
//
//   test of a terrain-engine based on GeoMipMapping
//
//   coded by zzed and Stormbringer
//   email:   zzed@gmx.li    storm_bringer@gmx.li 
//
//   7.2.2001
//
///////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MipMapRenderer.h"
#include "glTexFont.h"
#include "EngineStatus.h"

#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <math.h>
#include <stdio.h>

extern CEngineStatus	EngineStatus;

CMipMapRenderer::CMipMapRenderer(CTerrainData *pTerrainData)
{
	m_pTerrainData		= pTerrainData;
	m_temp = false;
//	m_fDeltaXYZ			= 1.0f;
	m_fDeltaZ			= 0.3f;
	m_iQuadLength		= EngineStatus.m_iQuadLength;     // the length!
	m_fDetailLevel		= 10.0f;
	m_fClippingDistance	= 20.0f;
	m_fMinClippingDistance= 400.0f;
	m_fMaxClippingDistance= 2000.0f;

	PreprocessTerrain();
	InitQuads();
}

CMipMapRenderer::~CMipMapRenderer()
{
	for (int y=0; y<m_iTerrainHeight; y++)
		for (int x=0; x<m_iTerrainWidth; x++)
			delete m_MipmapQuads[y*m_iTerrainWidth+x];
	delete []m_MipmapQuads;
}



void CMipMapRenderer::Render(bool bLines)
{
	float	fQuadLength		= m_iQuadLength;
	float	fQuadXPos		= 0;
	float	fQuadYPos		= 0;
	int		iNoTriangles	= 0;
	int		i				= 0;
	int		iCount1			= 0;
	int		iCount2			= 0;


	//float fRenderDist = m_fMinRenderDistance;
	//float	fClippingDistSqrd	= fClippingDist*fClippingDist;

	int		iXMin		= (int)((m_fPosX-m_fClippingDistance)/m_pTerrainData->m_iWidth*m_iTerrainWidth);
	int		iXMax		= (int)((m_fPosX+m_fClippingDistance)/m_pTerrainData->m_iWidth*m_iTerrainWidth);
	int		iYMin		= (int)((m_fPosY-m_fClippingDistance)/m_pTerrainData->m_iHeight*m_iTerrainHeight);
	int		iYMax		= (int)((m_fPosY+m_fClippingDistance)/m_pTerrainData->m_iHeight*m_iTerrainHeight);

	//printf("fPosZ-m_fAverageTerrainHeight: %.2f\nfRenderDist: %.2f\n", fPosZ-m_fAverageTerrainHeight, fRenderDist);
	//printf("X:%.2f, Y:%.2f, fRenderDist:%.2f\n", fPosX, fPosY, fRenderDist);

	if (iXMin<0) iXMin = 0;
	if (iXMax>m_iTerrainWidth) iXMax = m_iTerrainWidth;
	if (iYMin<0) iYMin = 0;
	if (iYMax>m_iTerrainHeight) iYMax = m_iTerrainHeight;

	//printf("iYMin:%d, iYMax:%d, iXMin:%d, iXMax:%d\n", iYMin, iYMax, iXMin, iXMax);
	// resolution for all quads is being calculated
	m_temp = true;
	for (int y=iYMin; y<iYMax; y++)
	{
		i = y*m_iTerrainWidth+iXMin;
		for (int x=iXMin; x<iXMax; x++)
		{
			//i = y*m_iQuadWidth+x;
			// is quad inside frustum?
			if (m_cFrustum.QuadInFrustum(m_MipmapQuads[i]->m_iXPos, m_MipmapQuads[i]->m_iYPos, 
					m_MipmapQuads[i]->m_fZPos, m_MipmapQuads[i]->m_fSize, m_MipmapQuads[i]->m_fHeight))
			{
				float fTermX = m_fPosX-(float)m_MipmapQuads[i]->m_iXPos;
				float fTermY = m_fPosY-(float)m_MipmapQuads[i]->m_iYPos;
				float fTermZ = m_fPosZ-m_MipmapQuads[i]->m_fZPos;
				float fDistanceSqrd = fTermX*fTermX+fTermY*fTermY+fTermZ*fTermZ;  // square!
				if (sqrt(fDistanceSqrd)>2000) printf("zu hoch: %.2f", fDistanceSqrd);
				iCount1++;
				m_MipmapQuads[i]->m_bVisible = true;
				m_MipmapQuads[i]->SetResolution(fDistanceSqrd);
			} else m_MipmapQuads[i]->m_bVisible = false;
			i++;
		}
	}

	m_cFrustum.Update();

	i = 0;
	fQuadYPos = iYMin*fQuadLength;
	for (y=iYMin; y<iYMax; y++)
	{
		fQuadXPos = iXMin*fQuadLength;
		i = y*m_iTerrainWidth+iXMin;
		for (int x=iXMin; x<iXMax; x++)
		{
			// is quad inside frustum?
			if (m_MipmapQuads[i]->m_bVisible)
			{
				iCount2++;
				glPushMatrix();
				glTranslatef(fQuadXPos, fQuadYPos, 0.0f);
				iNoTriangles += m_MipmapQuads[i]->RenderQuad((y==iYMin ? 1 : m_MipmapQuads[i-m_iTerrainWidth]->m_iCurSteps), 
					(y==iYMax-1 ? 1 : m_MipmapQuads[i+m_iTerrainWidth]->m_iCurSteps),
					(x==iXMin ? 1 : m_MipmapQuads[i-1]->m_iCurSteps), (x==iXMax-1 ? 1 : m_MipmapQuads[i+1]->m_iCurSteps), bLines);
				glPopMatrix();
			}
			i++;
			fQuadXPos += fQuadLength;
		}
		fQuadYPos += fQuadLength;
	}
	//glPopMatrix();
	m_iTriangleCount = iNoTriangles;

	fontDrawString(0, 0, "NoTriangles: %d, fRenderDistance: %.1f, %d, %d", iNoTriangles, m_fClippingDistance, iCount1, iCount2);
}

void CMipMapRenderer::InitQuads()
{	
	CTerrainData		*pTerrainData;

	// divide the entire heightmap into several quad-chunks
	printf("\nDividing terrain ...");

	int iMaxDiv	= m_iQuadLength;
	
	m_iTerrainHeight = (m_pTerrainData->m_iHeight-1)/m_iQuadLength;
	m_iTerrainWidth = (m_pTerrainData->m_iWidth-1)/m_iQuadLength;

	// create quad-classes
	m_MipmapQuads = (CMipMapQuad**)new int[m_iTerrainHeight*m_iTerrainWidth];

	for (int y=0; y<m_iTerrainHeight; y++)
	{
		for (int x=0; x<m_iTerrainWidth; x++)
		{
			// write heightdata in new array for quad
			pTerrainData = new CTerrainData();
			pTerrainData->m_iHeight = m_iQuadLength+1;
			pTerrainData->m_iWidth = m_iQuadLength+1;
			pTerrainData->m_afHeightmap = new float[(m_iQuadLength+1)*(m_iQuadLength+1)];
			for (int line=0; line<=m_iQuadLength; line++)
			{
				memcpy(pTerrainData->m_afHeightmap+line*(m_iQuadLength+1), m_pTerrainData->m_afHeightmap+(y*m_iQuadLength+line)*m_pTerrainData->m_iWidth+x*m_iQuadLength, sizeof(float)*(m_iQuadLength+1));
			}
			// now convert vertex-shades to suitable data-format for quad
			pTerrainData->m_acShades = new unsigned char[(m_iQuadLength+1)*(m_iQuadLength+1)];
			for (line=0; line<=m_iQuadLength; line++)
				memcpy(pTerrainData->m_acShades+line*(m_iQuadLength+1), m_pTerrainData->m_acShades+(y*m_iQuadLength+line)*m_pTerrainData->m_iWidth+x*m_iQuadLength, sizeof(unsigned char)*(m_iQuadLength+1));
//			m_MipmapQuads[y*m_iQuadWidth+x] = new CMipMapQuad(x*m_iQuadLength+m_iQuadLength/2, y*m_iQuadLength+m_iQuadLength/2, m_iQuadLength, iMaxDiv, pTerrainData);
//			m_MipmapQuads[y*m_iQuadWidth+x]->CalcEpsilon(m_fDetailLevel, 0.1f, 0.08f, 480, 1); */
//			m_MipmapQuads[y*m_iQuadWidth+x]->CalcEpsilon(m_fDetailLevel, EngineStatus.m_fEpsilonNearClip,EngineStatus.m_fEpsilonTopCoor, 480);
			m_MipmapQuads[y*m_iTerrainWidth+x] = new CMipMapQuad(x*m_iQuadLength+m_iQuadLength/2, y*m_iQuadLength+m_iQuadLength/2, m_iQuadLength, iMaxDiv, pTerrainData);
			m_MipmapQuads[y*m_iTerrainWidth+x]->CalcEpsilon(m_fDetailLevel, EngineStatus.m_fEpsilonNearClip, EngineStatus.m_fEpsilonTopCoor, 480);
									 

		}
	}
}
	

void CMipMapRenderer::PreprocessTerrain()
{
	printf("\nProcessing map data ...");
	// change height of map and calculate average-height
	m_fAverageTerrainHeight = 0;
	for (int i=0; i<m_pTerrainData->m_iWidth*m_pTerrainData->m_iHeight; i++)
	{
		m_pTerrainData->m_afHeightmap[i] *= m_fDeltaZ;
		m_fAverageTerrainHeight += m_pTerrainData->m_afHeightmap[i]/(m_pTerrainData->m_iWidth*m_pTerrainData->m_iHeight);
	}
	m_pTerrainData->CalcVertexShade();
}

float CMipMapRenderer::GetClippingDistance()
{
	m_fClippingDistance= (m_fPosZ-m_fAverageTerrainHeight)*m_fRatioClippingDistance;	
	if (m_fClippingDistance<m_fMinClippingDistance) m_fClippingDistance = m_fMinClippingDistance;
	if (m_fClippingDistance>m_fMaxClippingDistance) m_fClippingDistance = m_fMaxClippingDistance;
	return m_fClippingDistance-m_iQuadLength;
}

void CMipMapRenderer::SetPosition(float fPosX, float fPosY, float fPosZ)
{
	m_fPosX = fPosX;
	m_fPosY = fPosY;
	m_fPosZ = fPosZ;
}

void CMipMapRenderer::SetRotation(float fRotHor, float fRotVer)
{
	m_fRotHor = fRotHor;
	m_fRotVer = fRotVer;
}

void CMipMapRenderer::UpdateSettings()
{
	InitQuads();
}