///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   definition of CRenderer
//   class controls the rendering process (one step above quads) and
//   updates terraindata dynamically
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

#include "MipMapRenderer.h"
#include "TerrainDataLoader.h"
#include "glextensions.h"

#include "QuadTexture.h"

#ifdef WIN32
#include <windows.h>
#include <winbase.h>
#endif

#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>


using namespace glext;


CRenderer::CRenderer(CTerrainData *pTerrainData)
{
	m_pData	= pTerrainData;
	m_eBProcStatus = bpStatIdle;
	m_uiSleep = 1;
    uint i;

	// initialize sector-list
	uint nosectors = m_pData->m_uiTerrainQuadWidth/m_pData->m_uiSectorWidth*m_pData->m_uiTerrainQuadHeight/m_pData->m_uiSectorWidth;
	m_pSectorList = new CSector*[nosectors];
	for (i=0; i<nosectors; ++i) m_pSectorList[i] = 0;
	for (i=0; i<4; ++i) m_uiVisSectors[i] = 0;

	SetBackgroundProc(bpStatIdle);
	GetClippingDistance();
	UpdateTerrain();
}

CRenderer::~CRenderer()
{
}


void CRenderer::Render()
{
	float	fQuadWidth		= m_pData->m_uiQuadWidth;
	float	fQuadXPos		= 0;
	float	fQuadYPos		= 0;
	int		iNoTriangles	= 0;
	int		i				= 0;
	int		iCount1			= 0;
	int		iCount2			= 0;
	uint    secy;

	unsigned int	uiTerrainWidth	= m_pData->m_uiTerrainQuadWidth;
	unsigned int	uiTerrainHeight	= m_pData->m_uiTerrainQuadHeight;
	CVector			vPos(m_pData->m_vCameraPos);
	float			fClippingDist	= m_pData->m_fQuadClippingDist;
	unsigned short	usMaxQuadLength	= m_pData->m_uiQuadWidth;
	uint			uiSectorCountY = m_pData->m_uiTerrainQuadWidth/m_pData->m_uiSectorWidth;	// number of sectors per terrain-line

	m_eBProcStatus = bpStatRun;  // start background-process

	// increase framecounter
	if (m_pData->m_uiFrameCount<0xFFFFFFFF) ++m_pData->m_uiFrameCount;
	else m_pData->m_uiFrameCount = 0;

	// our background-thread shouldn't work too slow, let's check that
	if (CQuadTexture::m_uiCreateCount>2)
	{
		if (m_uiSleep<20) m_uiSleep++;
		CQuadTexture::m_uiCreateCount = 0;
	}

	// reserve some time for background-work
#ifdef WIN32
	Sleep(m_uiSleep);
#endif

	m_pFrustum.Update();
	CQuad::m_uiTexCreated = 0;

	if (m_pData->m_ucDrawingStyle == TE_DS_TEXTURED)
	{
		glActiveTextureARB(GL_TEXTURE0_ARB);		// "low" texture
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		
		glActiveTextureARB(GL_TEXTURE0_ARB);

	}

/*	int		iXMin		= (int)((vPos.x-fClippingDist)/usMaxQuadLength*uiTerrainWidth);
	int		iXMax		= (int)((vPos.x+fClippingDist)/usMaxQuadLength*uiTerrainWidth);
	int		iYMin		= (int)((vPos.y-fClippingDist)/usMaxQuadLength*uiTerrainHeight);
	int		iYMax		= (int)((vPos.y+fClippingDist)/usMaxQuadLength*uiTerrainHeight);

	if (iXMin<0) iXMin = 0;
	if (iXMax>(int)uiTerrainWidth) iXMax = uiTerrainWidth;
	if (iYMin<0) iYMin = 0;
	if (iYMax>(int)uiTerrainHeight) iYMax = uiTerrainHeight;*/

	// resolution for all quads is being calculated
	for (secy=m_uiVisSectors[0]; secy<m_uiVisSectors[1]; ++secy)
		for (uint secx=m_uiVisSectors[2]; secx<m_uiVisSectors[3]; ++secx)
		{
			//CSector *sector = m_pSectorList[secy*uiSectorCountY+secx];
			CQuad **pQuadList = m_pSectorList[secy*uiSectorCountY+secx]->m_pQuadList;
			//CQuad *quad = sector->m_pQuadList[0];
			for (uint y=0; y<m_pData->m_uiSectorWidth; y++)
			{
				i = y*m_pData->m_uiSectorWidth;
				for (uint x=0; x<m_pData->m_uiSectorWidth; x++)
				{
					// is quad inside frustum?
					if (m_pFrustum.QuadInFrustum(pQuadList[i]->m_vPos, pQuadList[i]->m_fSize, pQuadList[i]->m_fHeight))
					{
						CVector vTerm(vPos-pQuadList[i]->m_vPos);
						float fDistanceSqrd = vTerm.x*vTerm.x+vTerm.y*vTerm.y+vTerm.z*vTerm.z;
						iCount1++;
						pQuadList[i]->m_bVisible = true;
						pQuadList[i]->SetResolution(fDistanceSqrd);
					} else pQuadList[i]->m_bVisible = false;
					i++;
				}
			}
		}

	i = 0;
	for (secy=m_uiVisSectors[0]; secy<m_uiVisSectors[1]; ++secy)
		for (uint secx=m_uiVisSectors[2]; secx<m_uiVisSectors[3]; ++secx)
		{
			CSector *sector = m_pSectorList[secy*uiSectorCountY+secx];
			CQuad **pQuadList = sector->m_pQuadList;
			fQuadYPos = sector->m_uiYPos;
			for (uint y=0; y<m_pData->m_uiSectorWidth; y++)
			{
				fQuadXPos = sector->m_uiXPos;
				i = y*m_pData->m_uiSectorWidth;
				for (uint x=0; x<m_pData->m_uiSectorWidth; x++)
				{
					// is quad inside frustum?
					if (pQuadList[i]->m_bVisible)
					{
						iCount2++;
						glPushMatrix();
						glTranslatef(fQuadXPos, fQuadYPos, 0.0f);
					/*	iNoTriangles += pQuadList[i]->RenderQuad((y==iYMin ? 1 : pQuadList[i-uiTerrainWidth]->m_iCurSteps), // TODO: auf anderen sektor zugreifen und daten holen ...
							(y==iYMax-1 ? 1 : pQuadList[i+uiTerrainWidth]->m_iCurSteps),
							(x==iXMin ? 1 : pQuadList[i-1]->m_iCurSteps), (x==iXMax-1 ? 1 : pQuadList[i+1]->m_iCurSteps));*/
						iNoTriangles += pQuadList[i]->RenderQuad();
						glPopMatrix();
					}
					i++;
					fQuadXPos += fQuadWidth;
				}
				fQuadYPos += fQuadWidth;
			}
		}
	//glPopMatrix();
	m_iTriangleCount = iNoTriangles;
	glDisable(GL_TEXTURE_2D);

//	fontDrawString(0, 0, "NoTriangles: %d, fRenderDistance: %.1f, %d, %d", iNoTriangles, fRenderDist, iCount1, iCount2);
}

float CRenderer::GetClippingDistance()
{
	float fClipDist		= m_pData->m_fQuadClippingDist;
	float fAvrgHeight	= m_pData->m_fAverageHeight;
	float fRatioClip	= m_pData->m_fRatioQuadClippingDist;
	float fMinClip		= m_pData->m_fMinQuadClippingDist;
	float fMaxClip		= m_pData->m_fMaxQuadClippingDist;
	float fPosZ			= m_pData->m_vCameraPos.z;
	int	  iQuadLength	= m_pData->m_uiQuadWidth;
	
	fClipDist = (fPosZ-fAvrgHeight)*fRatioClip;	
	if (fClipDist<fMinClip) fClipDist = fMinClip;
	if (fClipDist>fMaxClip) fClipDist = fMaxClip;

	m_pData->m_fQuadClippingDist = fClipDist;
	return fClipDist-iQuadLength;
}

void CRenderer::BackgroundProcess()
{
	for (;m_eBProcStatus;)
	{
		if (m_eBProcStatus==bpStatRun)
		{
			UpdateTerrain();
		}
#ifdef WIN32		
		Sleep(100);
#endif
	}
}

void CRenderer::SetBackgroundProc(CRenderer::EBProcStatus eStatus)
{
	m_eBProcStatus = eStatus;

#ifdef WIN32	
	if (eStatus==bpStatIdle || eStatus==bpStatRun)
	{
		// create thread
		HANDLE hThread = CreateThread(0, 0, CatchThread, this, 0, 0);
		SetThreadPriority(hThread, THREAD_PRIORITY_BELOW_NORMAL);
	}
#endif

}

#ifdef WIN32
unsigned long __stdcall CRenderer::CatchThread(void* lpPar)
{
	reinterpret_cast<CRenderer*>(lpPar)->BackgroundProcess();

	// never reached!
	return 0;
}
#else
unsigned long CRenderer::CatchThread(void *lpPar)
{

}
#endif



void CRenderer::RemoveSectors(const uint uiStartIndex, const uint uiDelta, const uint uiEndIndex, const int iUpdateRefIndex, const uint uiUpdateRefDirection)
{
	for (uint index=uiStartIndex; index<uiEndIndex; index+=uiDelta)
		{
			if (m_pSectorList[index])
			{
				delete m_pSectorList[index];
				m_pSectorList[index] = 0;
				if (iUpdateRefIndex) m_pSectorList[index+iUpdateRefIndex]->UpdateQuadRefs(uiUpdateRefDirection, 0);
			}
		}
}

void CRenderer::UpdateTerrain()
{
	printf("\nUpdateTerrain()\n");

	CQuadTexture::m_uiCreateCount = 0;

	uint uiSectorCountX = m_pData->m_uiTerrainQuadWidth/m_pData->m_uiSectorWidth;	// number of sectors per terrain-line
	uint uiSectorCountY = m_pData->m_uiTerrainQuadHeight/m_pData->m_uiSectorWidth;	// number of sectors per terrain-column
	
	// calculate range of visible sectors
	uint uiSectorSize = m_pData->m_uiQuadWidth*m_pData->m_uiSectorWidth;	// size of a sector (in heightpoint units!)
	int iVisSec[4];  
	iVisSec[0] = (int)((m_pData->m_vCameraPos.y-m_pData->m_fQuadClippingDist)/uiSectorSize);		// yMin
	iVisSec[1] = (int)((m_pData->m_vCameraPos.y+m_pData->m_fQuadClippingDist)/uiSectorSize+1.0f);	// yMax
	iVisSec[2] = (int)((m_pData->m_vCameraPos.x-m_pData->m_fQuadClippingDist)/uiSectorSize);		// xMin
	iVisSec[3] = (int)((m_pData->m_vCameraPos.x+m_pData->m_fQuadClippingDist)/uiSectorSize+1.0f);	// xMax
	// clamp values
	ClampValue(iVisSec[0], 0, uiSectorCountY-1);
	ClampValue(iVisSec[1], 0, uiSectorCountY-1);
	ClampValue(iVisSec[2], 0, uiSectorCountX-1);
	ClampValue(iVisSec[3], 0, uiSectorCountX-1);

	printf("\nVisSec:%d, %d, %d, %d", iVisSec[0], iVisSec[1], iVisSec[2], iVisSec[3]);
	
	// check all sectors/quads if they're redundant

	uint uiTempSec[4] = { m_uiVisSectors[0], m_uiVisSectors[1], m_uiVisSectors[2], m_uiVisSectors[3] };  // buffer old sectorconstraints
	bool sdelete = false;		// sectors deleted?
	
	if (iVisSec[0]>m_uiVisSectors[0]) { m_uiVisSectors[0] = iVisSec[0]; sdelete = true; }   // upper side
	if (iVisSec[1]<m_uiVisSectors[1]) { m_uiVisSectors[1] = iVisSec[1]; sdelete = true; }   // lower side
	if (iVisSec[2]>m_uiVisSectors[2]) { m_uiVisSectors[2] = iVisSec[2]; sdelete = true; }   // left side
	if (iVisSec[3]<m_uiVisSectors[3]) { m_uiVisSectors[3] = iVisSec[3]; sdelete = true; }   // right side

	if (sdelete)
	{	// ok, now we gotta delete some sectors
		// wait for render-thread to finish current frame (we don't want to delete sectors which are accessed at the moment!)
#ifdef WIN32	  
	  for (uint tempframe=m_pData->m_uiFrameCount; tempframe>0 && tempframe==m_pData->m_uiFrameCount;) Sleep(1);
#endif
		if (iVisSec[0]>uiTempSec[0])	// upper side
		{
			for (uint y=uiTempSec[0]; y<iVisSec[0]; ++y)
				RemoveSectors(y*uiSectorCountX+uiTempSec[2], 1, y*uiSectorCountX+uiTempSec[3], (y==iVisSec[0]-1) ? uiSectorCountX : 0, 1);
			uiTempSec[0] = iVisSec[0];
		}
		if (iVisSec[1]<uiTempSec[1])	// lower side
		{
			for (uint y=uiTempSec[1]; y>iVisSec[1]; --y)
				RemoveSectors(y*uiSectorCountX+uiTempSec[2], 1, y*uiSectorCountX+uiTempSec[3], (y==iVisSec[1]+1) ? -uiSectorCountX : 0, 3);
			uiTempSec[1] = iVisSec[1];
		}
		if (iVisSec[2]>uiTempSec[2])	// left side
		{
			for (uint x=uiTempSec[2]; x<iVisSec[2]; ++x)
				RemoveSectors(uiTempSec[0]*uiSectorCountX+x, uiSectorCountX, uiTempSec[1]*uiSectorCountX+x, (x==iVisSec[2]-1) ? 1 : 0, 0);
			uiTempSec[2] = iVisSec[2];
		}
		if (iVisSec[3]<uiTempSec[3])	// right side
		{
			for (uint x=uiTempSec[3]; x>iVisSec[3]; --x)
				RemoveSectors(uiTempSec[0]*uiSectorCountX+x, uiSectorCountX, uiTempSec[1]*uiSectorCountX+x, (x==iVisSec[3]+1) ? -1 : 0, 2);
			uiTempSec[3] = iVisSec[3];
		}
	}

	// check all sectors/quads if they're loaded
	for (int y=iVisSec[0]; y<iVisSec[1]; ++y)
		for (int x=iVisSec[2]; x<iVisSec[3]; ++x)
		{
			uint index = y*uiSectorCountX+x;
			// is current sector already loaded?
			if (m_pSectorList[index]==0) 
			{
				// create new sector
				m_pSectorList[index] = new CSector(x*uiSectorSize, y*uiSectorSize, m_pData);
				// update references of already created surrounding sectors
				if (x>0 && m_pSectorList[index-1]!=0) 
				{
					m_pSectorList[index]->UpdateQuadRefs(0, m_pSectorList[index-1]);
					m_pSectorList[index-1]->UpdateQuadRefs(2, m_pSectorList[index]);
				}
				if (y>0 && m_pSectorList[index-uiSectorCountX]!=0) 
				{
					m_pSectorList[index]->UpdateQuadRefs(1, m_pSectorList[index-uiSectorCountX]);
					m_pSectorList[index-uiSectorCountX]->UpdateQuadRefs(3, m_pSectorList[index]);
				}
				if (x<uiSectorCountX-1 && m_pSectorList[index+1]!=0) 
				{
					m_pSectorList[index]->UpdateQuadRefs(2, m_pSectorList[index+1]);
					m_pSectorList[index+1]->UpdateQuadRefs(0, m_pSectorList[index]);
				}
				if (y<uiSectorCountY-1 && m_pSectorList[index+uiSectorCountX]!=0) 
				{
					m_pSectorList[index]->UpdateQuadRefs(3, m_pSectorList[index+uiSectorCountX]);
					m_pSectorList[index+uiSectorCountX]->UpdateQuadRefs(1, m_pSectorList[index]);
				}
			}
			for (int i=0; i<m_pData->m_uiSectorWidth*m_pData->m_uiSectorWidth; ++i)
				m_pSectorList[index]->m_pQuadList[i]->CheckData();
		}


	// store updated (and drawable) quads in global class variables
	for (uint i=0; i<4; ++i) m_uiVisSectors[i] = (uint)iVisSec[i];

	if (CQuadTexture::m_uiCreateCount<3 && m_uiSleep>2) m_uiSleep--;

	printf("ccount:%d ", CQuadTexture::m_uiCreateCount);
}
