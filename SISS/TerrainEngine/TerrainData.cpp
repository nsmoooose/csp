///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   declaration of class CTerrainData
//   encapsulates all data for displaying the terrain (except textures)
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	06/14/2001	implemented CVector - zzed
//	09/03/2001  added water values - stormbringer
//
//
///////////////////////////////////////////////////////////////////////////

#include "TerrainData.h"
//#include "Tools.h"

//#include <stdio.h>
//#include <math.h>
//#include <io.h>
//#include <stdlib.h>
//#include <fcntl.h>

#include "TextureZoom.h"
#include "ResourceManager.h"


CTerrainData::CTerrainData()
{
	m_vCameraPos.set(0.0f, 0.0f, 0.0f);
	m_fDeltaZ				= 0;
	m_uiMaxNoTextures		= 0;
	m_uiNoMipmaps			= 0;
	m_uiTerrainQuadHeight	= 0;
	m_uiTerrainQuadWidth	= 0;
	m_uiTerrainPointHeight	= 0;
	m_uiTerrainPointWidth	= 0;
	m_fMinHeight			= 1.0f;
	m_fMaxHeight			= 0.0f;
	m_fAverageHeight		= 0.0f;
	m_ucDrawingStyle		= TE_DS_WIREFRAME;
	m_pQuadRenderer			= reinterpret_cast<CQuad::CQuadRenderer*>(new CQuad::CQuadWireframeRenderer());
	m_hTextureWaterQuad		= false;
	m_fWaterLevel			= 0.0f;
	m_hTerrainFile			= 0;
	m_uiFrameCount			= 0;


/*	m_pLowTex = new CTextureZoom("../../SISS/TerrainData/HITW-TS2-farm-mixed.jpg");
	ResManager.InsertResource(&handle, m_pLowTex);
	m_pLowTex = reinterpret_cast<CTextureZoom*>(ResManager.Lock(handle));
	m_pHighTex = new CTextureZoom("../../SISS/TerrainData/HITW-TS2-forest-mixed-grass-yellow.jpg");
	ResManager.InsertResource(&handle, m_pHighTex);
	m_pHighTex = reinterpret_cast<CTextureZoom*>(ResManager.Lock(handle)); */

}


CTerrainData::~CTerrainData()
{
	if (m_hTerrainFile) fclose(m_hTerrainFile);
}


/*CTerrainData::CTerrainData(char* sFilename, bool b16bit)
{
	int success;
	long lsize, iFile;
	unsigned char *buffer;
	float fwidth;

	// Load TerraGen raw data and save to internal structure

	iFile = _open(sFilename, _O_RDONLY);
	if(iFile==-1)
	{	
		printf("unable to open '%s'!\n", sFilename);
		char text[100];
		gets(text);
		exit(1);
	}

	lsize = _filelength(iFile);

	if (b16bit) lsize /= 2;

	fwidth = sqrt((double)lsize);				// compute terrain width from size of file
	m_iWidth = (int)fwidth;
	m_iHeight = (int)fwidth;

	if (b16bit) lsize *= 2;

	m_afHeightmap = new float[lsize];
	buffer = new unsigned char[lsize];

	success = _read(iFile, buffer, (lsize*sizeof(char)));


	if (b16bit)
	{
		// ocnvert RAW 16-bit values to float
		for (int y=0; y<m_iHeight; y++)
			for (int x=0; x<m_iWidth; x++)
			{
				if (((short *)buffer)[y*m_iHeight+x]==65036) m_afHeightmap[y*m_iHeight+x] = 0;
				else m_afHeightmap[y*m_iHeight+x] = (float)(((short *)buffer)[y*m_iHeight+x]);
			}
	} else
	{
		// convert RAW 8-bit values to float
		for (int y=0; y<m_iHeight; y++)					
			for (int x=0; x<m_iWidth; x++)
			{
				m_afHeightmap[y*m_iHeight+x] = 
					(float)(((unsigned char *)buffer)[y*m_iHeight+x]);
			} 
	}

	_close(iFile);
	delete []buffer;
}

CTerrainData::~CTerrainData()
{
	delete []m_afHeightmap;
	delete []m_acShades;
}


CTerrainData::CTerrainData() : m_iHeight(0), m_iWidth(0)
{

}

void CTerrainData::CalcVertexShade()
{
*/
