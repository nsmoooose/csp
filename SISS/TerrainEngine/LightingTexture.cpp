///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   definition of CLightingTexture
//   calculates and implements a quad's lighting texture
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//	8/14/2001	created file - zzed
//
//
///////////////////////////////////////////////////////////////////////////

#include "LightingTexture.h"

#include <assert.h>
#include <math.h>

#include "MipMapQuad.h"


CLightingTexture::CLightingTexture(CQuad *pQuad)
{
	m_pQuad = pQuad;
	m_bCache = false;
}


bool CLightingTexture::Create()
{
	CalcLightingTexture();
	return true;
}


void CLightingTexture::CalcLightingTexture()
{
	CVector vSun(1.0f, 10.0f, 1.0f);

	m_usHeight = m_pQuad->m_uiWidth;
	m_usWidth = m_pQuad->m_uiWidth;
	m_ucPixeldepth = 24;
	m_pData = new uchar[m_usHeight*m_usWidth*3];
	vSun.normalize();
	uchar *pPixel = m_pData;
	for (uint y=0; y<m_usHeight; ++y)
		for (uint x=0; x<m_usWidth; ++x)
		{
			float afNormal[3] = { 0.0f, 0.0f, 0.0f };
			RectNormal(afNormal, x, y);
			// calculate scalar (0<=fScalar<=2)
			float fScalar = afNormal[0]*vSun.x+afNormal[1]*vSun.y+afNormal[2]*vSun.z+1;
			if (fScalar<0) printf("scheisse");
			m_pData[(y*m_usWidth+x)*3+0] = (uchar)(fScalar*127);
			m_pData[(y*m_usWidth+x)*3+1] = m_pData[(y*m_usWidth+x)*3+0];
			m_pData[(y*m_usWidth+x)*3+2] = m_pData[(y*m_usWidth+x)*3+0];
			
			/*pPixel[0] = (uchar)(fScalar*127);
			pPixel[1] = pPixel[0];
			pPixel[2] = pPixel[0];

			pPixel += 3;*/
		}
	/*static uint g = 0;
	char text[20];
	sprintf(text, "test\\t%d_shades.raw", g);
	FILE *f = fopen(text, "wb");
	fwrite(m_pData, m_usWidth*m_usHeight*3, 1, f);
	fclose(f);
	sprintf(text, "test\\t%d_map.raw", g++);
	f = fopen(text, "wb");
	for (uint i=0; i<(m_usHeight)*(m_usHeight); i++)
	{
		uchar b;
		b = (uchar)m_pQuad->m_pfHeightmap[i];
		fwrite(&b, 1, 1, f);
	}
	fclose(f);*/
}

// calculates the average-normal of an rectangle inside the heightmap
// upper left corner: iXPos, iYPos
void CLightingTexture::RectNormal(float afNormal[3], uint uiXPos, uint uiYPos)
{
	int i;
	float normal1[3], normal2[3];
	float *pfHeightmap = m_pQuad->m_pfHeightmap;
	ushort usWidth = m_pQuad->m_uiWidth+1;

	assert((uiXPos>=0 && uiYPos>=0 && uiXPos<usWidth-1 && uiYPos<usWidth-1));


	// left-upper, right-upper, left-lower corners
	float vec1[3] = {0, 0, pfHeightmap[uiYPos*usWidth+uiXPos]};
	float vec2[3] = {1, 0, pfHeightmap[uiYPos*usWidth+uiXPos+1]};
	float vec3[3] = {0, 1, pfHeightmap[(uiYPos+1)*usWidth+uiXPos]};
	TriangleNormal(normal1, vec1, vec2, vec3);

	// right-upper, right-lower, left-lower corners
	float vec4[3] = {1, 1, pfHeightmap[(uiYPos+1)*usWidth+uiXPos+1]};
	TriangleNormal(normal2, vec2, vec4, vec3);

	for (i=0; i<3; i++) afNormal[i] = normal1[i]+normal2[i];

	// normalize afNormal
	float length;
	length = (float)sqrt(afNormal[0]*afNormal[0]+afNormal[1]*afNormal[1]+afNormal[2]*afNormal[2]);
	for (i=0; i<3; i++) afNormal[i] /= length;
}

void CLightingTexture::TriangleNormal(float afNormal[3], float afVec1[3], float afVec2[3], float afVec3[3])
{
	float diff1[3], diff2[3];
	int i;

	for (i=0; i<3; i++)
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
