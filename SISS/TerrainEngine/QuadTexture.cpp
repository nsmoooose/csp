///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   definition of CQuadTexture
//   calculates a quad's texture
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//	8/13/2001	created file - zzed
//  9/19/2001	added code to change textures' resolution dynamically in
//				a background process - zzed
//
//
///////////////////////////////////////////////////////////////////////////


#include "QuadTexture.h"

#include "MipMapQuad.h"
#include "LightingTexture.h"
#include "TerrainData.h"

#include <stdio.h>


uint CQuadTexture::m_uiCreateCount = 0;

CQuadTexture::CQuadTexture(CQuad *pQuad)
{
	m_pQuad = pQuad;
	m_usHeight = 0;
	m_usWidth = m_usHeight;
	m_ucPixeldepth = 24;
	m_pHeightTex = 0;
	m_pHighTex = 0;
	m_pLightTex = 0;
	m_pLowTex = 0;
	m_bGLHValid = false;
	m_bPValid = false;
}

CQuadTexture::~CQuadTexture()
{
	if (m_pHeightTex) delete m_pHeightTex;
	if (m_pLightTex) delete m_pLightTex;
	CGLTexture::Destroy();
}

bool CQuadTexture::Create()
{
	Init();
	
	//GetGLHandle();
	return true;
}

void CQuadTexture::Init()
{
	// is temp-file available?
	//LoadBuffer();
	// loading failed, so let's create the quad

	m_uiCreateCount++;

	// create lighting-texture for this quad
	if (!m_pLightTex)
	{
		m_pLightTex = new CLightingTexture(m_pQuad);
		m_pLightTex->Create();
	}
	//ResManager.InsertResource(&handle, m_pLightTex);
	//m_pLightTex = reinterpret_cast<CTextureZoom*>(ResManager.Lock(handle));
	// create heightmap-"texture" for this quad for heightvalue-interpolation when mixing textures
	if (!m_pHeightTex) CreateHeightmapTexture();
	// getting textures ...
	if (!m_pLowTex) m_pLowTex = m_pQuad->m_pData->m_pLowTex;
	if (!m_pHighTex) m_pHighTex = m_pQuad->m_pData->m_pHighTex;
	CalcQuadTexture();
	m_bGLHValid = false;
	//SaveBuffer();
}

void CQuadTexture::CalcQuadTexture()
{
	static int counter	= 0;
	float	fLowAlt		= 0.0f;
	float	fHighAlt	= 250.0f;
	uint	uiSpanWidth	= m_pQuad->m_pData->m_ucTexQuadSpan*m_usWidth;	// width of source textures which can cover several quads
	uint	uiSpanY		= (((uint)m_pQuad->m_vPos.y/m_pQuad->m_uiWidth)%m_pQuad->m_pData->m_ucTexQuadSpan)*m_usWidth;
	uint	uiSpanX		= (((uint)m_pQuad->m_vPos.x/m_pQuad->m_uiWidth)%m_pQuad->m_pData->m_ucTexQuadSpan)*m_usWidth;

	
	uchar *ucLowTex = m_pLowTex->GetZoomedTexture(uiSpanWidth, uiSpanWidth);
	uchar *ucHighTex = m_pHighTex->GetZoomedTexture(uiSpanWidth, uiSpanWidth);
	uchar *ucLightTex = m_pLightTex->GetZoomedTexture(m_usWidth, m_usHeight);
	uchar *ucHeightTex = m_pHeightTex->GetZoomedTexture(m_usWidth, m_usHeight);

	if (m_pData) 
	{
		delete[] m_pData;
	}
	m_pData = new uchar[m_usWidth*m_usHeight*3];
	printf("w:%d", m_usWidth);
	uint texindex = 0;
	for (uint y=0; y<m_usWidth; ++y)
	{
		uint spanindex = ((y+uiSpanY)*uiSpanWidth+uiSpanX)*3;
		for (uint x=0; x<m_usWidth; ++x)
		{
			uchar height = ucHeightTex[y*m_usWidth+x]; //m_pQuad->m_pfHeightmap[y*(m_usWidth+1)+x];
			if (height>fHighAlt)
				for (uint i=0; i<3; ++i) m_pData[texindex] = (uchar)((uint)ucHighTex[spanindex++]*ucLightTex[texindex++]/255);
			else
			{
				for (uint i=0; i<3; ++i) m_pData[texindex] = (uchar)(((uint)ucLowTex[spanindex]*(fHighAlt-height)+(uint)ucHighTex[spanindex++]*height)/fHighAlt*ucLightTex[texindex++]/255);
				/*if (m_usWidth==128)
					for (uint i=0; i<3; ++i) { m_pData[texindex-1-i] = (uchar)255; }
				if (m_usWidth==64 && texindex>1)
					for (uint i=0; i<3; ++i) { m_pData[texindex-1-i] = (uchar)128; }
				if (m_usWidth==32 && texindex>1)
					for (uint i=0; i<3; ++i) { m_pData[texindex-1-i] = (uchar)0; }*/
			}
		}
	}

	m_bPValid = true;

	/*char text[50];
	sprintf(text, "res%d.raw", counter++);
	FILE *f = fopen(text, "wb");
	fwrite(ucLightTex, 65*65*3, 1, f);
	fclose(f);*/
	/*f = fopen("light.raw", "wb");
	fwrite(ucLightTex, 65*65*3, 1, f);
	fclose(f);
	exit(0);*/
}

void CQuadTexture::CreateHeightmapTexture()
{
	uint uiWidth = m_pQuad->m_uiWidth+1;
	float fRatio = 255.0f/(m_pQuad->m_pData->m_fMaxHeight-m_pQuad->m_pData->m_fMinHeight);
	uchar *ucHeightTex = new uchar[uiWidth*uiWidth];

	for (uint i=0; i<uiWidth*uiWidth; ++i)
		ucHeightTex[i] = (uchar)((m_pQuad->m_pfHeightmap[i]-m_pQuad->m_pData->m_fMinHeight)*fRatio);
	
	m_pHeightTex = new CTextureZoom(uiWidth, uiWidth, 8, ucHeightTex);
	m_pHeightTex->Create();
	//ResManager.InsertResource(&handle, m_pHeightTex);
	//m_pHeightTex = reinterpret_cast<CTextureZoom*>(ResManager.Lock(handle));
}

bool CQuadTexture::CreateTexture(ushort usSize)
{
	if (m_usWidth==usSize && m_usHeight==usSize) return false;
	//printf("%d>%d ", m_usWidth, usSize);
	m_usWidth = usSize;
	m_usHeight = usSize;

	Init();

	return true;
}

void CQuadTexture::Bind(GLenum eTarget)
{
	//printf("m_hGLTexture: %d\n", &m_hGLTexture);
	if ((!m_bGLHValid && m_bPValid && CQuad::m_uiTexCreated<2) || !m_hGLTexture)
	{
		if (m_hGLTexture) 
		{
			DeleteGLTex();
		}
		CQuad::m_uiTexCreated++;
		GetGLHandle();
		m_bGLHValid = true;
	}

	if (m_hGLTexture) glBindTexture(eTarget, m_hGLTexture);
}
