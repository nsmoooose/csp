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

#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <math.h>



CMipMapRenderer::CMipMapRenderer(CTerrainData *pTerrainData)
{
	m_pTerrainData		= pTerrainData;
	m_temp = false;
	m_fDeltaXYZ			= 1.0f;
	m_fDeltaZ			= 0.3f;
	m_iQuadLength		= 32;     // the length!
	m_fDetailLevel		= 10.0f;
	m_fRenderDistance	= 20.0f;
	m_fMinRenderDistance= 400.0f;
	m_fMaxRenderDistance= 2000.0f;


	PreprocessTerrain();
	InitQuads();
	printf("\nReady!");
}

CMipMapRenderer::~CMipMapRenderer()
{
	for (int y=0; y<m_iQuadHeight; y++)
		for (int x=0; x<m_iQuadWidth; x++)
			delete m_MipmapQuads[y*m_iQuadWidth+x];
	delete []m_MipmapQuads;
}


void CMipMapRenderer::Render(float fPosX, float fPosY, float fPosZ, float fRotX, float fRotY, bool bLines)
{
	float	fQuadLength		= m_iQuadLength;
	float	fQuadXPos		= 0;
	float	fQuadYPos		= 0;
	int		iNoTriangles	= 0;
	int		i				= 0;
	int		iCount1			= 0;
	int		iCount2			= 0;


	float	fRenderDist = (fPosZ-m_fAverageTerrainHeight)*m_fRenderDistance;	
	if (fRenderDist<m_fMinRenderDistance) fRenderDist = m_fMinRenderDistance;
	if (fRenderDist>m_fMaxRenderDistance) fRenderDist = m_fMaxRenderDistance;
	//float fRenderDist = m_fMinRenderDistance;
	float	fRenderDistSqrd	= fRenderDist*fRenderDist;

	int		iXMin		= (int)((fPosX-fRenderDist)/m_pTerrainData->m_iWidth/m_fDeltaXYZ*m_iQuadWidth);
	int		iXMax		= (int)((fPosX+fRenderDist)/m_pTerrainData->m_iWidth/m_fDeltaXYZ*m_iQuadWidth);
	int		iYMin		= (int)((fPosY-fRenderDist)/m_pTerrainData->m_iHeight/m_fDeltaXYZ*m_iQuadHeight);
	int		iYMax		= (int)((fPosY+fRenderDist)/m_pTerrainData->m_iHeight/m_fDeltaXYZ*m_iQuadHeight);

	//printf("fPosZ-m_fAverageTerrainHeight: %.2f\nfRenderDist: %.2f\n", fPosZ-m_fAverageTerrainHeight, fRenderDist);
	//printf("X:%.2f, Y:%.2f, fRenderDist:%.2f\n", fPosX, fPosY, fRenderDist);

	if (iXMin<0) iXMin = 0;
	if (iXMax>m_iQuadWidth) iXMax = m_iQuadWidth;
	if (iYMin<0) iYMin = 0;
	if (iYMax>m_iQuadHeight) iYMax = m_iQuadHeight;

	glPushMatrix();
	glScalef(m_fDeltaXYZ, m_fDeltaXYZ, m_fDeltaXYZ);

	//printf("iYMin:%d, iYMax:%d, iXMin:%d, iXMax:%d\n", iYMin, iYMax, iXMin, iXMax);
	// resolution for all quads is being calculated
	m_temp = true;
	for (int y=iYMin; y<iYMax; y++)
	{
		i = y*m_iQuadWidth+iXMin;
		for (int x=iXMin; x<iXMax; x++)
		{
			//i = y*m_iQuadWidth+x;
			// is quad inside frustum?
			if (/*m_MipmapQuads[i]->m_bVisible && */m_cFrustum.QuadInFrustum(m_MipmapQuads[i]->m_iXPos, m_MipmapQuads[i]->m_iYPos, 
					m_MipmapQuads[i]->m_fZPos, m_MipmapQuads[i]->m_fSize, m_MipmapQuads[i]->m_fHeight))
			{
				float fTermX = fPosX-(float)m_MipmapQuads[i]->m_iXPos*m_fDeltaXYZ;
				float fTermY = fPosY-(float)m_MipmapQuads[i]->m_iYPos*m_fDeltaXYZ;
				float fTermZ = fPosZ-m_MipmapQuads[i]->m_fZPos;
				float fDistanceSqrd = fTermX*fTermX+fTermY*fTermY+fTermZ*fTermZ;  // square!
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
		i = y*m_iQuadWidth+iXMin;
		for (int x=iXMin; x<iXMax; x++)
		{
			// is quad inside frustum?
			if (m_MipmapQuads[i]->m_bVisible)
			{
				iCount2++;
				glPushMatrix();
				glTranslatef(fQuadXPos, fQuadYPos, 0.0f);
				iNoTriangles += m_MipmapQuads[i]->RenderQuad((y==iYMin ? 1 : m_MipmapQuads[i-m_iQuadWidth]->m_iCurSteps), 
					(y==iYMax-1 ? 1 : m_MipmapQuads[i+m_iQuadWidth]->m_iCurSteps),
					(x==iXMin ? 1 : m_MipmapQuads[i-1]->m_iCurSteps), (x==iXMax-1 ? 1 : m_MipmapQuads[i+1]->m_iCurSteps), bLines);
				glPopMatrix();
			}
			i++;
			fQuadXPos += fQuadLength;
		}
		fQuadYPos += fQuadLength;
	}
	glPopMatrix();

	fontDrawString(0, 0, "NoTriangles: %d, fRenderDistance: %.1f, %d, %d", iNoTriangles, fRenderDist, iCount1, iCount2);
}

void CMipMapRenderer::InitQuads()
{	
	CTerrainData		*pTerrainData;

	// divide the entire heightmap into several quad-chunks
	printf("\nDividing terrain ...");

	int iMaxDiv	= m_iQuadLength;
	
	m_iQuadHeight = (m_pTerrainData->m_iHeight-1)/m_iQuadLength;
	m_iQuadWidth = (m_pTerrainData->m_iWidth-1)/m_iQuadLength;

	// create quad-classes
	m_MipmapQuads = (CMipMapQuad**)new int[m_iQuadHeight*m_iQuadWidth];

	for (int y=0; y<m_iQuadHeight; y++)
	{
		for (int x=0; x<m_iQuadWidth; x++)
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
			m_MipmapQuads[y*m_iQuadWidth+x] = new CMipMapQuad(x*m_iQuadLength+m_iQuadLength/2, y*m_iQuadLength+m_iQuadLength/2, m_iQuadLength, iMaxDiv, pTerrainData);
			m_MipmapQuads[y*m_iQuadWidth+x]->CalcEpsilon(m_fDetailLevel, 0.1f, 0.08f, 480, 1);
		}
	}
}

// calculates the shading of each vertex using the angle the neighboring triangles have in relation to the sun
/*void CMipMapRenderer::CalcVertexShade()
{
	unsigned char	*pCurVertex;
//	float			afVector[3];
	float			afNormal[3];
	float			afSun[3]		= {11, 0.4, 1};
	int				i;
	float			fScalar;
	float			fAverage;
	int				iStep;

	printf("\nProcessing map data ...");
	// change height of map
	for (i=0; i<m_heightmap->m_iWidth*m_heightmap->m_iHeight; i++)
		m_heightmap->m_afHeightmap[i] *= m_fDeltaZ;

	printf("\nCalculating vertex-shades      [          ]");
	NormalizeVector(afSun);
	m_acVertexShades = new unsigned char[m_heightmap->m_iWidth*m_heightmap->m_iHeight];
	pCurVertex = m_acVertexShades;
	iStep = m_heightmap->m_iHeight/10;
	for (int y=0; y<m_heightmap->m_iHeight; y++)
	{
		if (y%iStep == 0) WriteProgressBar(y/iStep);
		for (int x=0; x<m_heightmap->m_iWidth; x++)
		{
			int myx = x;
			int myy = y;
			for (i=0; i<3; i++) afNormal[i] = 0;
			if (x==m_heightmap->m_iWidth-1) myx--;
			if (y==m_heightmap->m_iHeight-1) myy--;
			CalcRectNormal(afNormal, myx, myy);
			
			// calculate scalar (0<=fScalar<=2)
			fScalar = afNormal[0]*afSun[0]+afNormal[1]*afSun[1]+afNormal[2]*afSun[2]+1;

			*pCurVertex = (unsigned char)(fScalar*127);

			pCurVertex++;
		}
	}
	// smooth the shades ...
	for (int iNoRun=0; iNoRun<2; iNoRun++)
	{
		printf("\nSmoothing shades pass #%d       [          ]", iNoRun+1);  
		for (y=0; y<m_heightmap->m_iHeight; y++)
		{
			if (y%iStep == 0) WriteProgressBar(y/iStep);
			for (int x=0;x<m_heightmap->m_iWidth; x++)
			{
				i = 0;
				fAverage = 0;

				for (int iStep=-1; iStep<=1; iStep+=2)
				{
					if (x+iStep>=0 && x+iStep<m_heightmap->m_iWidth)
					{
						i++;
						fAverage += m_acVertexShades[y*m_heightmap->m_iWidth+x+iStep];
					}
					if (y+iStep>=0 && y+iStep<m_heightmap->m_iHeight)
					{
						i++;
						fAverage += m_acVertexShades[(y+iStep)*m_heightmap->m_iWidth+x];
					}
				}
				/*for (int myy=-2; myy<2; myy++)
					for (int myx=-2; myx<2; myx++)
						if (myy+y>=0 && myy+y<m_heightmap->m_iHeight && myx+x>=0 && myx+x<m_heightmap->m_iWidth)
						{
							i++;
							fAverage += m_acVertexShades[(y+myy)*m_heightmap->m_iWidth+x+myx];
						}
				m_acVertexShades[y*m_heightmap->m_iWidth+x] = fAverage/i;
			}
		}
	}
			/*i = 0;
			fAverage = 0;
			// left-upper
			if (x-2>=0 && y-2>=0)
			{
				i++;
				fAverage += m_acVertexShades[(y-2)*m_heightmap->m_iWidth+x-2];
			}
			// left-lower
			if (x-2>=0 && y+2<m_heightmap->m_iHeight)
			{
				i++;
				fAverage += m_acVertexShades[(y+2)*m_heightmap->m_iWidth+x-2];
			}
			// right-upper
			if (x+2<m_heightmap->m_iWidth && y-2>=0)
			{
				i++;
				fAverage += m_acVertexShades[(y-2)*m_heightmap->m_iWidth+x+2];
			}
			// right-lower
			if (x+2<m_heightmap->m_iWidth && y+2<m_heightmap->m_iHeight)
			{
				i++;
				fAverage += m_acVertexShades[(y+2)*m_heightmap->m_iWidth+x+2];
			}
			m_acVertexShades[y*m_heightmap->m_iWidth+x] = fAverage/i;
		}
	}
}*/
	

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
