///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   definition of CQuad
//   encapsulates one part of the terrain - a single quad
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	7/30/2001	updated documentation - zzed
//  9/19/2001	added code to change textures' resolution dynamically in
//				a background process - zzed
//
//
///////////////////////////////////////////////////////////////////////////

/**
 * Represents a quad inside the whole terrain - it's a square with 2^x+1 points on one side.
 */

#include "MipMapQuad.h"
#include "TerrainData.h"
#include "glextensions.h"
#include "QuadTexture.h"

#include "../../Test/TerrainTest/OGLText.h"

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>

#ifdef WIN32
#include <io.h>
#endif

#include <assert.h>

#ifndef TRUE
#define TRUE 1
#endif

using namespace glext;


uint CQuad::m_uiMaxSteps = 1;
uint CQuad::m_uiTexCreated = 0;


CQuad::CQuad(int iXPos, int iYPos, CTerrainData *pData)
{
	m_pData			= pData;
	m_vPos.x		= iXPos;
	m_vPos.y		= iYPos;
	m_uiWidth		= m_pData->m_uiQuadWidth;
	m_afD			= new float[(uint)(log(m_uiWidth)/log(2))];	
	m_bVisible		= false;
	m_uiCurSteps	= 1;
	m_fSize			= (float)m_pData->m_uiQuadWidth;
	for (int i=0; i<4; ++i) m_pNeighbors[i] = &m_uiMaxSteps;
	LoadQuadData();
	m_pTexture		= new CQuadTexture(this);
	CalcMinMaxHeight();
	CalcEpsilon(m_pData->m_fDetailLevel, 0.1f, 0.08f, 480);
	CheckData();
}

CQuad::~CQuad()
{
	//delete []m_afD;
	delete []m_pfHeightmap;
	if (m_pTexture) delete m_pTexture;
}

int CQuad::RenderQuad()
{
	int		fX[2], fY[2];
	int		iTriangleCount	= 0;   // to count the triangles which are drawn
	int		iYStart			= 0;   // should the first line of triangles be drawn? or is it used by crack-resolving?
	int		iYEnd			= 0;   // last line
	int		iXStart			= 0;   // left line
	int		iXEnd			= 0;   // right line


	switch (m_pData->m_ucDrawingStyle) 
	{
		case TE_DS_TEXTURED:
			glActiveTextureARB(GL_TEXTURE0_ARB);
			m_pTexture->Bind(GL_TEXTURE_2D);
		case TE_DS_SMOOTH:
			glBegin(GL_TRIANGLES);
			break;
		case TE_DS_WIREFRAME:
		default:
			glBegin(GL_LINES);
			break;
	}

	unsigned int iNoQuadPoints = m_uiWidth+1;
	// now all the cracks between the neighboring quads are resolved
	if (*m_pNeighbors[1]>m_uiCurSteps)	// has the upper quad a lower resolution than this one?
	{
		// yes, obviously!
		iTriangleCount += RenderInterpolation(iNoQuadPoints-1-m_uiCurSteps, m_uiCurSteps, m_pfHeightmap+(m_uiCurSteps+1)*iNoQuadPoints-1-m_uiCurSteps, 
			-m_uiCurSteps, 0, iNoQuadPoints-1, 0, m_pfHeightmap+iNoQuadPoints-1, *m_pNeighbors[1]/m_uiCurSteps,
			m_uiCurSteps, m_uiCurSteps);
		iYStart = m_uiCurSteps;		// the first "line" of triangles mustn't be drawed again (where the crack-resolving took place)
		// two triangles on each side are perhaps missing (when two crack-resolving sides meet in one point, there have to be drawn missing triangles
		if (*m_pNeighbors[0]<=m_uiCurSteps) // were cracks resolved on the left side?
		{
			iTriangleCount++; 
			m_pData->m_pQuadRenderer->DrawTriangle(0, 0, m_uiCurSteps, m_uiCurSteps, 0, m_uiCurSteps, this); // yes? then let's fill the resulting gap
		}
		// the same procedure again on the other side
		if (*m_pNeighbors[2]<=m_uiCurSteps)
		{
			iTriangleCount++;
			m_pData->m_pQuadRenderer->DrawTriangle(iNoQuadPoints-1, 0, iNoQuadPoints-1, m_uiCurSteps, iNoQuadPoints-1-m_uiCurSteps, m_uiCurSteps, this);
		}
	}
	// the same functionality repeats on each side of the quad (and we have 4 - so there are another 3 crack-resolvers!
	if (*m_pNeighbors[3]>m_uiCurSteps)
	{
		iTriangleCount += RenderInterpolation(m_uiCurSteps, iNoQuadPoints-m_uiCurSteps-1, m_pfHeightmap+(iNoQuadPoints-1-m_uiCurSteps)*iNoQuadPoints+m_uiCurSteps, 
			m_uiCurSteps, 0, 0, iNoQuadPoints-1, m_pfHeightmap+(iNoQuadPoints-1)*iNoQuadPoints, *m_pNeighbors[3]/m_uiCurSteps,
			iNoQuadPoints-1-m_uiCurSteps, iNoQuadPoints-1-m_uiCurSteps);
		iYEnd = m_uiCurSteps;
		if (*m_pNeighbors[0]<=m_uiCurSteps)
		{
			iTriangleCount++;
			m_pData->m_pQuadRenderer->DrawTriangle(0, iNoQuadPoints-1, 0, iNoQuadPoints-1-m_uiCurSteps, m_uiCurSteps, iNoQuadPoints-1-m_uiCurSteps, this);
		}
		if (*m_pNeighbors[2]<=m_uiCurSteps)
		{
			iTriangleCount++;
			m_pData->m_pQuadRenderer->DrawTriangle(iNoQuadPoints-1-m_uiCurSteps, iNoQuadPoints-1-m_uiCurSteps, 
				iNoQuadPoints-1, iNoQuadPoints-1-m_uiCurSteps, iNoQuadPoints-1, iNoQuadPoints-1, this);
		}
	}
	if (*m_pNeighbors[0]>m_uiCurSteps)
	{ 
		iTriangleCount += RenderInterpolation(m_uiCurSteps, m_uiCurSteps, m_pfHeightmap+m_uiCurSteps*iNoQuadPoints+m_uiCurSteps, 
			0, m_uiCurSteps, 0, 0, m_pfHeightmap, *m_pNeighbors[0]/m_uiCurSteps,
			m_uiCurSteps, iNoQuadPoints-1-m_uiCurSteps);
		iXStart = m_uiCurSteps;
		if (*m_pNeighbors[1]<=m_uiCurSteps)
		{
			iTriangleCount++;
			m_pData->m_pQuadRenderer->DrawTriangle(0, 0, m_uiCurSteps, 0, m_uiCurSteps, m_uiCurSteps, this);
		}
		if (*m_pNeighbors[3]<=m_uiCurSteps)
		{
			iTriangleCount++;
			m_pData->m_pQuadRenderer->DrawTriangle(m_uiCurSteps, iNoQuadPoints-1-m_uiCurSteps, m_uiCurSteps, iNoQuadPoints-1, 0, iNoQuadPoints-1, this);
		}
	}
	if (*m_pNeighbors[2]>m_uiCurSteps)
	{  
		iTriangleCount += RenderInterpolation(iNoQuadPoints-1-m_uiCurSteps, iNoQuadPoints-1-m_uiCurSteps, m_pfHeightmap+(iNoQuadPoints-m_uiCurSteps)*iNoQuadPoints-m_uiCurSteps-1, 
			0, -m_uiCurSteps, iNoQuadPoints-1, iNoQuadPoints-1, m_pfHeightmap+iNoQuadPoints*iNoQuadPoints-1, *m_pNeighbors[2]/m_uiCurSteps,
			iNoQuadPoints-1-m_uiCurSteps, m_uiCurSteps);
		iXEnd = 1;
		if (*m_pNeighbors[1]<=m_uiCurSteps)
		{
			iTriangleCount++;
			m_pData->m_pQuadRenderer->DrawTriangle(iNoQuadPoints-1-m_uiCurSteps, 0, iNoQuadPoints-1, 0, iNoQuadPoints-1-m_uiCurSteps, m_uiCurSteps, this);
		}
		if (*m_pNeighbors[3]<=m_uiCurSteps)
		{
			iTriangleCount++;
			m_pData->m_pQuadRenderer->DrawTriangle(iNoQuadPoints-1, iNoQuadPoints-1, iNoQuadPoints-1-m_uiCurSteps, 
				iNoQuadPoints-1, iNoQuadPoints-1-m_uiCurSteps, iNoQuadPoints-1-m_uiCurSteps, this);

		}
	}
	
	// finally let's draw the whole quad
	for (int y=iYStart; y<m_uiWidth-m_uiCurSteps-iYEnd+1; y+=m_uiCurSteps)
	{
		for (int x=iXStart; x<m_uiWidth-m_uiCurSteps-iXEnd+1; x+=m_uiCurSteps)
		{
			// calculate all height-values of the four points which surround the two triangles which are to be drawn
			/*fZ[0] = m_pfHeightmap[y*m_usLength+x];
			fZ[1] = m_pfHeightmap[y*m_usLength+x+m_uiCurSteps];
			fZ[2] = m_pfHeightmap[(y+m_uiCurSteps)*m_usLength+x];
			fZ[3] = m_pfHeightmap[(y+m_uiCurSteps)*m_usLength+x+m_uiCurSteps];*/

			// X and Y values of the triangles ...
			fX[0] = x;
			fX[1] = x+m_uiCurSteps;
			fY[0] = y;
			fY[1] = y+m_uiCurSteps;

			// finally draw 'em
			m_pData->m_pQuadRenderer->DrawTriangle(fX[0], fY[0], fX[1], fY[0], fX[0], fY[1], this);
			m_pData->m_pQuadRenderer->DrawTriangle(fX[1], fY[0], fX[1], fY[1], fX[0], fY[1], this);

			iTriangleCount += 2;
		}
	}

	glEnd();

	// Draw a water quad at sea level, but only in textured mode
	if(m_pData->m_ucDrawingStyle == TE_DS_TEXTURED)
	{
		glPushAttrib(GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH);
		glBindTexture(GL_TEXTURE_2D, m_pData->m_hTextureWaterQuad);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(0.0f, 0.0f, m_pData->m_fWaterLevel);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(m_uiWidth, 0.0f, m_pData->m_fWaterLevel);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(m_uiWidth, m_uiWidth, m_pData->m_fWaterLevel);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(0.0f, m_uiWidth, m_pData->m_fWaterLevel);
		glEnd(); 
		glPopAttrib();
	}
	glFlush();
	
	return iTriangleCount;
}

void CQuad::SetResolution(float fD)
{
	int iSteps = m_uiWidth;

	m_fDistSqrd = fD;

	// step through all available resolutions and its maximum distances from the eye
	for (int i=(int)(log(m_uiWidth)/log(2))-1; i>=0; i--)
	{
		if (m_afD[i]<fD) // when the allowed distance is lower than the actual - take the resolution!
		{
			m_uiCurSteps = iSteps;
			return;
		}
		iSteps /= 2;
	}
	m_uiCurSteps = 1;
}

int CQuad::CalcEpsilon(float fTao, float fNearClip, float fTopCoor, int iVRes)
{
	int		iDIndex	= 0;
	int		iNoPoints = m_uiWidth+1;
    int i;

	// calculate maximum epsilon (pixel error) for different step-distances

	for (i=2; i<=m_uiWidth; i*=2)
	{
		m_afD[iDIndex] = 0;

		// step through all quads inside the big quad
		for (int iBaseY=0; iBaseY<iNoPoints-1; iBaseY+=i)
			for (int iBaseX=0; iBaseX<iNoPoints-1; iBaseX+=i)
			{
				float fP1		= m_pfHeightmap[iBaseY*iNoPoints+iBaseX];
				float fP2		= m_pfHeightmap[iBaseY*iNoPoints+iBaseX+i];
				float fP3		= m_pfHeightmap[(iBaseY+i)*iNoPoints+iBaseX+i];
				float fP4		= m_pfHeightmap[(iBaseY+i)*iNoPoints+iBaseX];

				// parse horizontal lines
				for (int y=0; y<=i; y++)
				{
					float fLHeight	= fP1+(fP4-fP1)*y/i;
					float fRHeight	= fP2+(fP3-fP2)*y/i;
					for (int x=0; x<=i; x++)
					{
						float fDiff = fLHeight+(fRHeight-fLHeight)*x/i-m_pfHeightmap[(iBaseY+y)*iNoPoints+iBaseX+x];
						if (fDiff<0) fDiff *= -1;
						if (m_afD[iDIndex]<fDiff)
						{
							//printf("\nhormaxdiff: x(%d) y(%d) i(%d) iBaseX(%d) iBaseY(%d) diff(%.2f) hmap(%.2f)", x, y, i, iBaseX, iBaseY, fDiff, m_pfHeightmap[(iBaseY+y)*iNoPoints+iBaseX+x]);
							m_afD[iDIndex] = fDiff;
						}
					}
				}
				// parse vertical lines
				for (int x=0; x<=i; x++)
				{
					float fUHeight	= fP1+(fP2-fP1)*x/i;
					float fLHeight	= fP4+(fP3-fP4)*x/i;
					for (int y=0; y<=i; y++)
					{
						float fDiff = fUHeight+(fLHeight-fUHeight)*y/i-m_pfHeightmap[(iBaseY+y)*iNoPoints+iBaseX+x];
						if (fDiff<0) fDiff *= -1;
						if (m_afD[iDIndex]<fDiff) 
						{
							//printf("\nvertmaxdiff: x(%d) y(%d) i(%d) iBaseX(%d) iBaseY(%d) diff(%.2f) fp1(%.2f) fp2(%.2f) fp3(%.2f) fp4(%.2f) hmap(%.2f)", x, y, i, iBaseX, iBaseY, fDiff, fP1, fP2, fP3, fP4, m_pfHeightmap[(iBaseY+y)*iNoPoints+iBaseX+x]);
							m_afD[iDIndex] = fDiff;
						}
					}
				}
			}
		iDIndex++;	
	}

	// now process this data to a minimum quad-distance from viewer for different quad-resolutions
	// tao = max. pixel difference

	for (i=0; i<(int)(log(m_uiWidth)/log(2)+1); i++)	// f***ing rounding error
	{ 
		
		//printf("i:%d, mafd: %f", i, m_afD[i]);
		m_afD[i] *= fNearClip*iVRes/(fTopCoor*2*fTao);
		m_afD[i] *= m_afD[i];
	}

	/* debugging stuff
	bool ok = true;

	for (i=0; i<(int)(log(m_usLength)/log(2)+1); i++) 
		if (i>0) if (m_afD[i]<m_afD[i-1]) ok = false;

	if (!ok)
	{
		printf("\nerror in m_afd chain ... debug output\nm_afd:: ");
		for (i=0; i<(int)(log(m_usLength)/log(2)+1); i++)
		{
			printf(" [%d]:(%.2f/%.2f)", i, m_afD[i], sqrt(m_afD[i])/fNearClip/iVRes*(fTopCoor*2*fTao));
		}			
		printf("\nquad dump: (m_usLength=%d)\n", m_usLength);
		for (int y=0; y<m_usLength+1; y++)
		{
			for (int x=0; x<m_usLength+1; x++)
				printf(" %4.2f", m_pfHeightmap[y*(m_usLength+1)+x]);
			printf("\n");
		}
		Sleep(5000);
	}*/
	
	return 0;
}


int CQuad::RenderInterpolation(int iStartX, int iStartY, float *pStart, int iStepX, int iStepY, int iNStartX, int iNStartY, 
									 float *pNStart, int iNStepMult, int iEndX, int iEndY)
{
	int iNStepX = iStepX*iNStepMult;
	int iNStepY = iStepY*iNStepMult;	
	int i;
	int iCount = 1;


	// draw first triangle
	m_pData->m_pQuadRenderer->DrawTriangle(iNStartX, iNStartY, iStartX, iStartY, iNStartX+iNStepX, iNStartY+iNStepY, this);

	// step NStart
	iNStartX += iNStepX;
	iNStartY += iNStepY;
	pNStart += iNStepY*m_uiWidth+iNStepX;

	i = 1;

	while (!(iStartX == iEndX && iStartY == iEndY))
	{
		for (; i<iNStepMult && !(iStartX==iEndX && iStartY==iEndY); i++)
		{
			m_pData->m_pQuadRenderer->DrawTriangle(iStartX, iStartY, iStartX+iStepX, iStartY+iStepY, iNStartX, iNStartY, this);
			iStartX += iStepX;
			iStartY += iStepY;
			pStart += iStepY*m_uiWidth+iStepX;
			iCount++;
		}
		i = 0;

		if (iStartX==iEndX && iStartY==iEndY) break;

		m_pData->m_pQuadRenderer->DrawTriangle(iNStartX, iNStartY, iStartX, iStartY, iNStartX+iNStepX, iNStartY+iNStepY, this);

		// step NStart
		iNStartX += iNStepX;
		iNStartY += iNStepY;
		pNStart += iNStepY*m_uiWidth+iNStepX;

		iCount++;
	}
	
	return iCount;
}


/*void CQuad::DrawTriangle(int iX1, int iY1, int iX2, int iY2, int iX3, int iY3)
{
	int usPoints = m_usLength+1;
	int iIndex1 = iY1*usPoints+iX1;
	int iIndex2 = iY2*usPoints+iX2;
	int iIndex3 = iY3*usPoints+iX3;

	/*if (iX1>32) printf("iX1: %d", iX1);
	if (iX2>32) printf("iX2: %d", iX2);
	if (iX3>32) printf("iX3: %d", iX3);
	if (iY1>32) printf("iY1: %d", iY1);
	if (iY2>32) printf("iY2: %d", iY2);
	if (iY3>32) printf("iY3: %d", iY3);
	if (iX1<0) printf("iX1: %d", iX1);
	if (iX2<0) printf("iX2: %d", iX2);
	if (iX3<0) printf("iX3: %d", iX3);
	if (iY1<0) printf("iY1: %d", iY1);
	if (iY2<0) printf("iY2: %d", iY2);
	if (iY3<0) printf("iY3: %d", iY3);
	if (m_pfHeightmap[iIndex1]<0) printf("height: %.2f", m_pfHeightmap[iIndex1]);
	if (m_pfHeightmap[iIndex2]<0) printf("height: %.2f", m_pfHeightmap[iIndex2]);
	if (m_pfHeightmap[iIndex3]<0) {
printf("height: %.2f", m_pfHeightmap[iIndex3]);
	}
	//printf("%.2f ", m_pfHeightmap[iIndex1]);
	glTexCoord2f((float)iX1/m_usLength, (float)iY1/m_usLength);
	if (m_pfHeightmap[iIndex1]>0) glColor3ub(0, m_acShades[iIndex1], 0);
	else glColor3ub(0, 0, m_acShades[iIndex1]);
	glVertex3f((float)iX1, (float)iY1, m_pfHeightmap[iIndex1]);
	glTexCoord2f((float)iX2/m_usLength, (float)iY2/m_usLength);
	if (m_pfHeightmap[iIndex2]>0) glColor3ub(0, m_acShades[iIndex2], 0);
	else glColor3ub(0, 0, m_acShades[iIndex2]);
	glVertex3f((float)iX2, (float)iY2, m_pfHeightmap[iIndex2]);
	glTexCoord2f((float)iX3/m_usLength, (float)iY3/m_usLength);
	if (m_pfHeightmap[iIndex3]>0) glColor3ub(0, m_acShades[iIndex3], 0);
	else glColor3ub(0, 0, m_acShades[iIndex3]);
	glVertex3f((float)iX3, (float)iY3, m_pfHeightmap[iIndex3]);
	//if (m_bLines) glVertex3f(iX1, iY1, m_pfHeightmap[iIndex1]);
}*/

void CQuad::CalcMinMaxHeight()
{
	float fMaxHeight = *m_pfHeightmap;
	float fMinHeight = *m_pfHeightmap;

	for (int i=0; i<(m_uiWidth+1)*(m_uiWidth+1); i++)
	{
		if (fMaxHeight<m_pfHeightmap[i]) fMaxHeight = m_pfHeightmap[i];
		if (fMinHeight>m_pfHeightmap[i]) fMinHeight = m_pfHeightmap[i];
	}

	if (m_pData->m_fMaxHeight<m_pData->m_fMinHeight)
	{
		m_pData->m_fMaxHeight = fMaxHeight;
		m_pData->m_fMinHeight = fMinHeight;
	}
	if (m_pData->m_fMaxHeight<fMaxHeight) m_pData->m_fMaxHeight = fMaxHeight;
	if (m_pData->m_fMinHeight>fMinHeight) m_pData->m_fMinHeight = fMinHeight;

	m_vPos.z = (fMaxHeight+fMinHeight)/2;
	m_fHeight = (fMaxHeight-fMinHeight)/2;
}

void CQuad::CreateTexture()
{
	// function disabled; moved code to CheckData


	//float fDist = sqrt((m_vPos.x-m_pData->m_vCameraPos.x)*(m_vPos.x-m_pData->m_vCameraPos.x)+(m_vPos.y-m_pData->m_vCameraPos.y)*(m_vPos.y-m_pData->m_vCameraPos.y));
	  // scene not rendered yet -> no distance to camera available
	//m_hGLTexture = m_pTexture->GetGLHandle();
	//ResManager.InsertResource(&m_hTexture, tex);
	//m_pTexture = reinterpret_cast<CGLTexture*>(ResManager.GetResource(m_hTexture));
	//ResManager.Lock(m_hTexture);
	//m_hGLTexture = m_pTexture->GetGLHandle(); // opengl not yet initialized -> see RenderQuad()
}

bool CQuad::CheckData()
{
	CVector vTerm(m_pData->m_vCameraPos-m_vPos);
	float fDist = vTerm.length();

	uint uiTexSize;
	if (fDist<100) uiTexSize = 128;
	else if (fDist<200) uiTexSize = 64;
	else uiTexSize = 32;

	//uint uiTexSize = pow(2, (uint)(1500-fDistSqrd)/210);
	//if (uiTexSize<=4) uiTexSize = 8;

	return m_pTexture->CreateTexture(uiTexSize);
	//else
//		m_pTexture->CreateTexture(8);
}







void CQuad::CQuadSmoothRenderer::DrawTriangle(int iX1, int iY1, int iX2, int iY2, int iX3, int iY3, CQuad *pQuad)
{
	int usPoints = pQuad->m_uiWidth+1;
	int iIndex1 = iY1*usPoints+iX1;
	int iIndex2 = iY2*usPoints+iX2;
	int iIndex3 = iY3*usPoints+iX3;

	//printf("%.2f ", m_pfHeightmap[iIndex1]);
	glColor3ub(0, pQuad->m_pfHeightmap[iIndex1], 0);
	glVertex3f((float)iX1, (float)iY1, pQuad->m_pfHeightmap[iIndex1]);
	glColor3ub(0, pQuad->m_pfHeightmap[iIndex2], 0);
	glVertex3f((float)iX2, (float)iY2, pQuad->m_pfHeightmap[iIndex2]);
	glColor3ub(0, pQuad->m_pfHeightmap[iIndex3], 0);
	glVertex3f((float)iX3, (float)iY3, pQuad->m_pfHeightmap[iIndex3]);
}

void CQuad::CQuadWireframeRenderer::DrawTriangle(int iX1, int iY1, int iX2, int iY2, int iX3, int iY3, CQuad *pQuad)
{
	int usPoints = pQuad->m_uiWidth+1;
	int iIndex1 = iY1*usPoints+iX1;
	int iIndex2 = iY2*usPoints+iX2;
	int iIndex3 = iY3*usPoints+iX3;
	
	glColor3f(1, 1, 1);
	glVertex3f((float)iX1, (float)iY1, pQuad->m_pfHeightmap[iIndex1]);
	glVertex3f((float)iX2, (float)iY2, pQuad->m_pfHeightmap[iIndex2]);
	glVertex3f((float)iX3, (float)iY3, pQuad->m_pfHeightmap[iIndex3]);
	glVertex3f((float)iX1, (float)iY1, pQuad->m_pfHeightmap[iIndex1]);
}

void CQuad::CQuadMTexRenderer::DrawTriangle(int iX1, int iY1, int iX2, int iY2, int iX3, int iY3, CQuad *pQuad)
{
	int usPoints = pQuad->m_uiWidth+1;
	int iIndex1 = iY1*usPoints+iX1;
	int iIndex2 = iY2*usPoints+iX2;
	int iIndex3 = iY3*usPoints+iX3;


	glTexCoord2f((float)iX1/pQuad->m_uiWidth, (float)iY1/pQuad->m_uiWidth);
	glVertex3f((float)iX1, (float)iY1, pQuad->m_pfHeightmap[iIndex1]);

	glTexCoord2f((float)iX2/pQuad->m_uiWidth, (float)iY2/pQuad->m_uiWidth);
	glVertex3f((float)iX2, (float)iY2, pQuad->m_pfHeightmap[iIndex2]);

	glTexCoord2f((float)iX3/pQuad->m_uiWidth, (float)iY3/pQuad->m_uiWidth);
	glVertex3f((float)iX3, (float)iY3, pQuad->m_pfHeightmap[iIndex3]);
}

bool CQuad::LoadQuadData()
{
	int success, i;
	long lFileOffset;
	uchar *buffer;

	uint uiQuadWidth = m_pData->m_uiQuadWidth;
	uint uiMapWidth = m_pData->m_uiTerrainPointWidth;

	m_pfHeightmap = new float[(uiQuadWidth+1)*(uiQuadWidth+1)];
	buffer = new uchar[uiMapWidth];

	// set the file pointer to the right position
	lFileOffset = (uint)m_vPos.y*uiMapWidth+(uint)m_vPos.x;
	fseek(m_pData->m_hTerrainFile, lFileOffset, SEEK_SET);

	for (uint y=0; y<uiQuadWidth+1; ++y)
	{
		success = fread(buffer, 1, uiMapWidth, m_pData->m_hTerrainFile);
		assert(success>=uiQuadWidth+1);
		

		// convert the unsigned short values to float
		for(i=0; i<uiQuadWidth+1; i++)
			m_pfHeightmap[y*(uiQuadWidth+1)+i] = (float)buffer[i]*m_pData->m_fDeltaZ;
	}

	rewind(m_pData->m_hTerrainFile);

	delete []buffer; 
	return TRUE; 
}

