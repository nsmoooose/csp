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
#include "MipMapQuad.h"

#include <gl/gl.h>
#include <math.h>



CMipMapQuad::CMipMapQuad(int iXPos, int iYPos, int iLength, int iMaxDiv, CTerrainData *pTerrainData)
{
	m_pTerrainData	= pTerrainData;
	m_iXPos			= iXPos;
	m_iYPos			= iYPos;
	m_iMaxSteps		= iMaxDiv;
	m_fSize			= iLength/2;
	m_afD			= new float[(int)(log(m_iMaxSteps)/log(2))];	// maxsteps=8 : 2, 4, 8 -> 3
	m_bVisible		= false;
	m_iCurSteps		= 1;
	CalcMinMaxHeight();
}

CMipMapQuad::~CMipMapQuad()
{
	delete []m_afD;
	delete m_pTerrainData;
}

int CMipMapQuad::RenderQuad(int iStepTop, int iStepBottom, int iStepLeft, int iStepRight, bool bLines)
{
	float	fZ[4], fX[2], fY[2];
	int		iTriangleCount	= 0;
	int		iYStart			= 0;
	int		iYEnd			= 0;
	int		iXStart			= 0;
	int		iXEnd			= 0;
	int		iQuadLength		= m_pTerrainData->m_iWidth-1;

	m_bLines = bLines;

	if (bLines) 
		glBegin(GL_LINES);
	else
		glBegin(GL_TRIANGLES);

	if (iStepTop>m_iCurSteps)
	{
		iTriangleCount += RenderInterpolation((float)m_pTerrainData->m_iWidth-1-m_iCurSteps, (float)m_iCurSteps, m_pTerrainData->m_afHeightmap+(m_iCurSteps+1)*m_pTerrainData->m_iWidth-1-m_iCurSteps, 
			-m_iCurSteps, 0, (float)m_pTerrainData->m_iWidth-1, 0, m_pTerrainData->m_afHeightmap+m_pTerrainData->m_iWidth-1, iStepTop/m_iCurSteps,
			(float)m_iCurSteps, (float)m_iCurSteps, iStepRight<=m_iCurSteps, iStepLeft<=m_iCurSteps);
		iYStart = m_iCurSteps;
		// two last triangles are perhaps missing ...
		if (iStepLeft<=m_iCurSteps)
		{
			iTriangleCount++;
			DrawTriangle(0, 0, m_iCurSteps, m_iCurSteps, 0, m_iCurSteps);
		}
		if (iStepRight<=m_iCurSteps)
		{
			iTriangleCount++;
			DrawTriangle(m_pTerrainData->m_iWidth-1, 0, m_pTerrainData->m_iWidth-1, m_iCurSteps, m_pTerrainData->m_iWidth-1-m_iCurSteps, m_iCurSteps);
		}
	}
	if (iStepBottom>m_iCurSteps)
	{
		iTriangleCount += RenderInterpolation((float)m_iCurSteps, (float)m_pTerrainData->m_iHeight-m_iCurSteps-1, m_pTerrainData->m_afHeightmap+(m_pTerrainData->m_iHeight-1-m_iCurSteps)*m_pTerrainData->m_iWidth+m_iCurSteps, 
			m_iCurSteps, 0, 0, (float)m_pTerrainData->m_iHeight-1, m_pTerrainData->m_afHeightmap+(m_pTerrainData->m_iHeight-1)*m_pTerrainData->m_iWidth, iStepBottom/m_iCurSteps,
			(float)m_pTerrainData->m_iWidth-1-m_iCurSteps, (float)m_pTerrainData->m_iHeight-1-m_iCurSteps, iStepLeft<=m_iCurSteps, iStepRight<=m_iCurSteps);
		iYEnd = m_iCurSteps;
		if (iStepLeft<=m_iCurSteps)
		{
			iTriangleCount++;
			DrawTriangle(0, m_pTerrainData->m_iHeight-1, 0, m_pTerrainData->m_iHeight-1-m_iCurSteps, m_iCurSteps, m_pTerrainData->m_iHeight-1-m_iCurSteps);
		}
		if (iStepRight<=m_iCurSteps)
		{
			iTriangleCount++;
			DrawTriangle(m_pTerrainData->m_iWidth-1-m_iCurSteps, m_pTerrainData->m_iHeight-1-m_iCurSteps, 
				m_pTerrainData->m_iWidth-1, m_pTerrainData->m_iHeight-1-m_iCurSteps, m_pTerrainData->m_iWidth-1, m_pTerrainData->m_iHeight-1);
		}
	}
	if (iStepLeft>m_iCurSteps)
	{ 
		iTriangleCount += RenderInterpolation((float)m_iCurSteps, (float)m_iCurSteps, m_pTerrainData->m_afHeightmap+m_iCurSteps*m_pTerrainData->m_iWidth+m_iCurSteps, 
			0, m_iCurSteps, 0, 0, m_pTerrainData->m_afHeightmap, iStepLeft/m_iCurSteps,
			(float)m_iCurSteps, (float)m_pTerrainData->m_iHeight-1-m_iCurSteps, iStepBottom<=m_iCurSteps, iStepTop<=m_iCurSteps);
		iXStart = m_iCurSteps;
		if (iStepTop<=m_iCurSteps)
		{
			iTriangleCount++;
			DrawTriangle(0, 0, m_iCurSteps, 0, m_iCurSteps, m_iCurSteps);
		}
		if (iStepBottom<=m_iCurSteps)
		{
			iTriangleCount++;
			DrawTriangle(m_iCurSteps, m_pTerrainData->m_iHeight-1-m_iCurSteps, m_iCurSteps, m_pTerrainData->m_iHeight-1, 0, m_pTerrainData->m_iHeight-1);
		}
	}
	if (iStepRight>m_iCurSteps)
	{  
		iTriangleCount += RenderInterpolation((float)m_pTerrainData->m_iWidth-1-m_iCurSteps, (float)m_pTerrainData->m_iWidth-1-m_iCurSteps, m_pTerrainData->m_afHeightmap+(m_pTerrainData->m_iHeight-m_iCurSteps)*m_pTerrainData->m_iWidth-m_iCurSteps-1, 
			0, -m_iCurSteps, (float)m_pTerrainData->m_iWidth-1, (float)m_pTerrainData->m_iHeight-1, m_pTerrainData->m_afHeightmap+m_pTerrainData->m_iWidth*m_pTerrainData->m_iHeight-1, iStepRight/m_iCurSteps,
			(float)m_pTerrainData->m_iWidth-1-m_iCurSteps, (float)m_iCurSteps, iStepTop<=m_iCurSteps, iStepBottom<=m_iCurSteps);
		iXEnd = 1;
		if (iStepTop<=m_iCurSteps)
		{
			iTriangleCount++;
			DrawTriangle(m_pTerrainData->m_iWidth-1-m_iCurSteps, 0, m_pTerrainData->m_iWidth-1, 0, m_pTerrainData->m_iWidth-1-m_iCurSteps, m_iCurSteps);
		}
		if (iStepBottom<=m_iCurSteps)
		{
			iTriangleCount++;
			DrawTriangle(m_pTerrainData->m_iWidth-1, m_pTerrainData->m_iHeight-1, m_pTerrainData->m_iWidth-1-m_iCurSteps, 
				m_pTerrainData->m_iHeight-1, m_pTerrainData->m_iWidth-1-m_iCurSteps, m_pTerrainData->m_iHeight-1-m_iCurSteps);

		}
	}
	
	for (int y=iYStart; y<m_pTerrainData->m_iHeight-m_iCurSteps-iYEnd; y+=m_iCurSteps)
	{
		for (int x=iXStart; x<m_pTerrainData->m_iWidth-m_iCurSteps-iXEnd; x+=m_iCurSteps)
		{
			fZ[0] = m_pTerrainData->m_afHeightmap[y*m_pTerrainData->m_iWidth+x];
			fZ[1] = m_pTerrainData->m_afHeightmap[y*m_pTerrainData->m_iWidth+x+m_iCurSteps];
			fZ[2] = m_pTerrainData->m_afHeightmap[(y+m_iCurSteps)*m_pTerrainData->m_iWidth+x];
			fZ[3] = m_pTerrainData->m_afHeightmap[(y+m_iCurSteps)*m_pTerrainData->m_iWidth+x+m_iCurSteps];

			fX[0] = (float)x;
			fX[1] = (float)x+m_iCurSteps;
			fY[0] = (float)y;
			fY[1] = (float)y+m_iCurSteps;

			DrawTriangle(fX[0], fY[0], fX[1], fY[0], fX[0], fY[1]);
			DrawTriangle(fX[1], fY[0], fX[1], fY[1], fX[0], fY[1]);

			iTriangleCount += 2;
		}
	}

	glEnd();
	
	return iTriangleCount;
}

void CMipMapQuad::SetResolution(float fD)
{
	int iSteps = m_iMaxSteps;

	// fD = maximum pixel error
	// now searching apropriate step-distance
	for (int i=(int)(log(m_iMaxSteps)/log(2))-1; i>=0; i--)
	{
		if (m_afD[i]<fD) 
		{
			m_iCurSteps = iSteps;
			return;
		}
		iSteps /= 2;
	}
	m_iCurSteps = 1;
}

int CMipMapQuad::CalcEpsilon(float fTao, float fNearClip, float fTopCoor, int iVRes, float fDeltaH)
{
	int		iDIndex	= 0;

	// calculate maximum epsilon (pixel error) for different step-distances

	for (int i=2; i<=m_iMaxSteps; i*=2)
	{
		m_afD[iDIndex] = 0;

		// step through all quads inside the big quad
		for (int iBaseY=0; iBaseY<m_pTerrainData->m_iHeight-1; iBaseY+=i)
			for (int iBaseX=0; iBaseX<m_pTerrainData->m_iWidth-1; iBaseX+=i)
			{
				float fP1		= m_pTerrainData->m_afHeightmap[iBaseY*m_pTerrainData->m_iWidth+iBaseX];
				float fP2		= m_pTerrainData->m_afHeightmap[iBaseY*m_pTerrainData->m_iWidth+iBaseX+i];
				float fP3		= m_pTerrainData->m_afHeightmap[(iBaseY+i)*m_pTerrainData->m_iWidth+iBaseX+i];
				float fP4		= m_pTerrainData->m_afHeightmap[(iBaseY+i)*m_pTerrainData->m_iWidth+iBaseX];

				// parse horizontal lines
				for (int y=0; y<=i; y++)
				{
					float fLHeight	= fP1+(fP4-fP1)*y/i;
					float fRHeight	= fP2+(fP3-fP2)*y/i;
					for (int x=0; x<=i; x++)
					{
						float fDiff = fLHeight+(fRHeight-fLHeight)*x/i-m_pTerrainData->m_afHeightmap[(iBaseY+y)*m_pTerrainData->m_iWidth+iBaseX+x];
						if (fDiff<0) fDiff *= -1;
						if (m_afD[iDIndex]<fDiff) m_afD[iDIndex] = fDiff;
					}
				}
				// parse vertical lines
				for (int x=0; x<=i; x++)
				{
					float fUHeight	= fP1+(fP2-fP1)*x/i;
					float fLHeight	= fP4+(fP4-fP3)*x/i;
					for (int y=0; y<=i; y++)
					{
						float fDiff = fUHeight+(fLHeight-fUHeight)*y/i-m_pTerrainData->m_afHeightmap[(iBaseY+y)*m_pTerrainData->m_iWidth+iBaseX+x];
						if (fDiff<0) fDiff *= -1;
						if (m_afD[iDIndex]<fDiff) m_afD[iDIndex] = fDiff;
					}
				}
			}
/*		// horizontal
		for (int y=0; y<m_pTerrainData->m_iHeight-i; y+=i)
			for (int x=0; x<m_pTerrainData->m_iWidth-i; x+=i)
			{
				// compare all points to line between step-points
				float fFirstPoint	= m_pTerrainData->m_afHeightmap[y*m_pTerrainData->m_iWidth+x];
				float fLastPoint	= m_pTerrainData->m_afHeightmap[y*m_pTerrainData->m_iWidth+x+i];
				float iDiff			= fLastPoint - fFirstPoint;

				for (int j=1; j<i; j++)
				{
					float fDiff = fFirstPoint+(iDiff*(float)j/(float)i)-m_pTerrainData->m_afHeightmap[y*m_pTerrainData->m_iWidth+x+j];
					//fDiff *= fDeltaH;		// convert to 3d-sim
					if (fDiff<0) fDiff *= -1;
					if (m_afD[iDIndex]<fDiff) m_afD[iDIndex] = fDiff;
				}
			}
		// vertical
		for (int x=0; x<m_pTerrainData->m_iWidth-i; x+=i)
			for (int y=0; y<m_pTerrainData->m_iHeight-i; y+=i)
			{
				// compare all points to line between step-points
				float fFirstPoint	= m_pTerrainData->m_afHeightmap[y*m_pTerrainData->m_iWidth+x];
				float fLastPoint	= m_pTerrainData->m_afHeightmap[(y+i)*m_pTerrainData->m_iWidth+x];
				float iDiff		= fLastPoint - fFirstPoint;

				for (int j=1; j<i; j++)
				{
					float fDiff = fFirstPoint+(iDiff*(float)j/(float)i)-m_pTerrainData->m_afHeightmap[(y+j)*m_pTerrainData->m_iWidth+x];
					//fDiff *= fDeltaH;		// convert to 3d-sim
					if (fDiff<0) fDiff *= -1;
					if (m_afD[iDIndex]<fDiff) m_afD[iDIndex] = fDiff;
				}
			}*/

		/*for (x=i; x<m_heightmap->m_iWidth; x+=i)
		{
			// top-right to bottom-left diagonals, top beginning
			for (int iSX=x,iSY=0; iSX>0; iSX-=i,iSY+=i)
			{
				float fFirstPoint = m_heightmap->m_afHeightmap[iSY*m_heightmap->m_iWidth+iSX];
				float fLastPoint = m_heightmap->m_afHeightmap[(iSY+i)*m_heightmap->m_iWidth+iSX-i];
				float iDiff = fLastPoint - fFirstPoint;
				for (int j=1; j<i; j++)
				{
					float fDiff = fFirstPoint+((float)iDiff*(float)j/(float)i)-m_heightmap->m_afHeightmap[(iSY+j)*m_heightmap->m_iWidth+iSX-j];
					fDiff *= fDeltaH;
					if (fDiff<0) fDiff *= -1;
					if (m_afD[iDIndex]<fDiff) m_afD[iDIndex] = fDiff;
				}
			}
			// right beginning
			for (iSX=m_heightmap->m_iWidth-1,iSY=m_heightmap->m_iHeight-1-x; iSY<m_heightmap->m_iHeight-1; iSX-=i,iSY+=i)
			{
				float fFirstPoint = m_heightmap->m_afHeightmap[iSY*m_heightmap->m_iWidth+iSX];
				float fLastPoint = m_heightmap->m_afHeightmap[(iSY+i)*m_heightmap->m_iWidth+iSX-i];
				float iDiff = fLastPoint - fFirstPoint;
				for (int j=1; j<i; j++)
				{
					float fDiff = fFirstPoint+((float)iDiff*(float)j/(float)i)-m_heightmap->m_afHeightmap[(iSY+j)*m_heightmap->m_iWidth+iSX-j];
					fDiff *= fDeltaH;
					if (fDiff<0) fDiff *= -1;
					if (m_afD[iDIndex]<fDiff) m_afD[iDIndex] = fDiff;
				}
			}
		}*/
			
		iDIndex++;	
	}

	// now process this data to a minimum quad-distance from viewer for different quad-resolutions
	// tao = max. pixel difference
	
	for (i=0; i<(int)(log(m_iMaxSteps)/log(2)); i++)
	{
		m_afD[i] *= fNearClip*iVRes/(fTopCoor*2*fTao);
		m_afD[i] *= m_afD[i];
	}
	
	return 0;
}


int CMipMapQuad::RenderInterpolation(float fStartX, float fStartY, float *pStart, int iStepX, int iStepY, float fNStartX, float fNStartY, 
									 float *pNStart, int iNStepMult, float fEndX, float fEndY, bool bAddLeft, bool bAddRight)
{
	int iNStepX = iStepX*iNStepMult;
	int iNStepY = iStepY*iNStepMult;	
	int i;
	int iCount = 1;


	// draw first triangle
	DrawTriangle(fNStartX, fNStartY, fStartX, fStartY, fNStartX+iNStepX, fNStartY+iNStepY);

	// step NStart
	fNStartX += iNStepX;
	fNStartY += iNStepY;
	pNStart += iNStepY*m_pTerrainData->m_iWidth+iNStepX;

	i = 1;

	while (!(fStartX == fEndX && fStartY == fEndY))
	{
		for (; i<iNStepMult && !(fStartX==fEndX && fStartY==fEndY); i++)
		{
			DrawTriangle(fStartX, fStartY, fStartX+iStepX, fStartY+iStepY, fNStartX, fNStartY);
			fStartX += iStepX;
			fStartY += iStepY;
			pStart += iStepY*m_pTerrainData->m_iWidth+iStepX;
			iCount++;
		}
		i = 0;

		if (fStartX==fEndX && fStartY==fEndY) break;

		DrawTriangle(fNStartX, fNStartY, fStartX, fStartY, fNStartX+iNStepX, fNStartY+iNStepY);

		// step NStart
		fNStartX += iNStepX;
		fNStartY += iNStepY;
		pNStart += iNStepY*m_pTerrainData->m_iWidth+iNStepX;

		iCount++;
	}
	
	return iCount;
}

void CMipMapQuad::DrawTriangle(int iX1, int iY1, int iX2, int iY2, int iX3, int iY3)
{
	int iIndex1 = iY1*m_pTerrainData->m_iWidth+iX1;
	int iIndex2 = iY2*m_pTerrainData->m_iWidth+iX2;
	int iIndex3 = iY3*m_pTerrainData->m_iWidth+iX3;
	
	// glColor3ub(m_pTerrainData->m_acShades[iIndex1], 
	// 		   m_pTerrainData->m_acShades[iIndex1], 
	// 		   m_pTerrainData->m_acShades[iIndex1]);
	// glTexCoord2d((float)iX1/(m_pTerrainData->m_iWidth-1), 
	//			(float)iY1/(m_pTerrainData->m_iHeight-1));
	

	// glColor3ub(m_pTerrainData->m_acShades[iIndex2], 
	// 		   m_pTerrainData->m_acShades[iIndex2], 
	// 		   m_pTerrainData->m_acShades[iIndex2]);
	// glTexCoord2d((float)iX2/(m_pTerrainData->m_iWidth-1),
	// 			 (float)iY2/(m_pTerrainData->m_iHeight-1));
	

	// glColor3ub(m_pTerrainData->m_acShades[iIndex3], 
	// 		   m_pTerrainData->m_acShades[iIndex3], 
	// 		   m_pTerrainData->m_acShades[iIndex3]);
	// glTexCoord2d((float)iX3/(m_pTerrainData->m_iWidth-1),
	// 			 (float)iY3/(m_pTerrainData->m_iHeight-1));

	if (m_pTerrainData->m_afHeightmap[iIndex1]>0) glColor3ub(0, m_pTerrainData->m_acShades[iIndex1], 0);
	else glColor3ub(0, 0, m_pTerrainData->m_acShades[iIndex1]);
	glVertex3f(iX1, iY1, m_pTerrainData->m_afHeightmap[iIndex1]);
	if (m_pTerrainData->m_afHeightmap[iIndex2]>0) glColor3ub(0, m_pTerrainData->m_acShades[iIndex2], 0);
	else glColor3ub(0, 0, m_pTerrainData->m_acShades[iIndex2]);
	glVertex3f(iX2, iY2, m_pTerrainData->m_afHeightmap[iIndex2]);
	if (m_pTerrainData->m_afHeightmap[iIndex3]>0) glColor3ub(0, m_pTerrainData->m_acShades[iIndex3], 0);
	else glColor3ub(0, 0, m_pTerrainData->m_acShades[iIndex3]);
	glVertex3f(iX3, iY3, m_pTerrainData->m_afHeightmap[iIndex3]);
	if (m_bLines) glVertex3f(iX1, iY1, m_pTerrainData->m_afHeightmap[iIndex1]);
}

void CMipMapQuad::CalcMinMaxHeight()
{
	float fMaxHeight = *m_pTerrainData->m_afHeightmap;
	float fMinHeight = *m_pTerrainData->m_afHeightmap;

	for (int y=0; y<m_pTerrainData->m_iHeight; y++)
		for (int x=0; x<m_pTerrainData->m_iWidth; x++)
		{
			if (fMaxHeight<m_pTerrainData->m_afHeightmap[y*m_pTerrainData->m_iWidth+x]) fMaxHeight = m_pTerrainData->m_afHeightmap[y*m_pTerrainData->m_iWidth+x];
			if (fMinHeight>m_pTerrainData->m_afHeightmap[y*m_pTerrainData->m_iWidth+x]) fMinHeight = m_pTerrainData->m_afHeightmap[y*m_pTerrainData->m_iWidth+x];
		}
	m_fZPos = (fMaxHeight+fMinHeight)/2;
	m_fHeight = (fMaxHeight-fMinHeight)/2;
}
