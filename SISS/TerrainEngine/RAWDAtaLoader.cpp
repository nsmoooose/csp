///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   declaration of CRAWDataLoader
//   loads a plain 8bit or 16bit terrain from a .RAW file
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	14/6/2001	implemented CVector - zzed
//
//
//
///////////////////////////////////////////////////////////////////////////


#include "RAWDataLoader.h"
#include "Tools.h"

#include <stdio.h>
#include <math.h>

#ifdef WIN32
#include <io.h>
#else
#include <sys/types.h>   
#include <sys/stat.h>  
#include <unistd.h>
#endif

#include <stdlib.h>
#include <fcntl.h>
#include <memory.h>

int CRAWDataLoader::LoadData(CTerrainData *pData, char *strFilename)
{
	m_pData = pData;
	
	SetDefaultValues();
	LoadRAWData(strFilename);

	return 0;
}


CTexture* CRAWDataLoader::LoadTexture(int iTextureIndex, int iMipmapIndex)
{
	// Loads a 256*256 pixel RGB bitmap in RAW
	FILE *fInput = fileopen("Texture_erde.raw", "r");
	unsigned char *ucBitmap = new unsigned char[256*256*3];
	fread(ucBitmap, 3, 256*256, fInput);
	fclose(fInput);
	return 0;
}

void CRAWDataLoader::SetDefaultValues()
{
	m_pData->m_fDeltaZ					= 0.3f;
	m_pData->m_uiWidth					= 32;     
	m_pData->m_fDetailLevel				= 2.0f;
	m_pData->m_fCurQuadClippingDist		= 200.0f;
	m_pData->m_fMinQuadClippingDist		= 2000.0f;
	m_pData->m_fMaxQuadClippingDist		= 2000.0f;
	m_pData->m_fRatioQuadClippingDist	= 1.0f;
	m_pData->m_uiMaxNoTextures			= 0;
	m_pData->m_uiNoMipmaps				= 0;
}

void CRAWDataLoader::LoadRAWData(char *strFilename)
{
	int success;
	long lsize, iFile;
	unsigned char *buffer;
	float fwidth;

	unsigned int iWidth, iHeight;
	float *afHeightmap;
	unsigned char *acShades;

	// Load TerraGen raw data and save to internal structure

#ifdef WIN32
	iFile = _open(strFilename, _O_BINARY);
#else
	iFile = open(strFilename, O_RDONLY);
#endif

	if(iFile==-1)
	{	
		printf("unable to open '%s'!\n", strFilename);
		char text[100];
		gets(text);
		exit(1);
	}

#ifdef WIN32
	lsize = _filelength(iFile);
#else
	struct stat statBuff;
	fstat(iFile, &statBuff);
	lsize = statBuff.st_size;
#endif


	fwidth = (float)sqrt((float)lsize);				// compute terrain width from size of file
	iWidth = (int)fwidth;
	iHeight = (int)fwidth;

	afHeightmap = new float[lsize];
	buffer = new unsigned char[lsize];

#ifdef WIN32
	success = _read(iFile, buffer, (lsize*sizeof(char)));
#else
	success = read(iFile, buffer, (lsize*sizeof(char)));
#endif	

	// convert RAW 8-bit values to float
	for (unsigned int y=0; y<iHeight; y++)					
		for (unsigned int x=0; x<iWidth; x++)
		{
			afHeightmap[y*iHeight+x] =  (float)(((unsigned char *)buffer)[y*iHeight+x])*m_pData->m_fDeltaZ;
		} 
#ifdef WIN32
	_close(iFile);
#else
	close(iFile);
#endif
	delete []buffer;

	m_pData->m_uiTerrainQuadHeight = iHeight/m_pData->m_usMaxQuadLength;
	m_pData->m_uiTerrainQuadWidth = iWidth/m_pData->m_usMaxQuadLength;
	m_pData->m_uiTerrainPointHeight = iHeight;
	m_pData->m_uiTerrainPointWidth = iWidth;

	acShades = new unsigned char[iHeight*iWidth];
	CalcShades(afHeightmap, acShades);

	// ok, now we've got all our heightdata in afHeightmap (floats) and shades in acShades
	// let's transform it to quads ...

	unsigned short	usMaxQuadLength = m_pData->m_usMaxQuadLength;
	unsigned int	uiTerrainHeight = m_pData->m_uiTerrainQuadHeight;
	unsigned int	uiTerrainWidth	= m_pData->m_uiTerrainQuadWidth;
	float			*afHMap;

	m_pData->m_apQuadList = (CQuad**)new int[uiTerrainWidth*uiTerrainHeight];
	for (int y=0; y<uiTerrainHeight; y++)
		for (unsigned int x=0; x<uiTerrainWidth; x++)
		{
			afHMap = new float[(usMaxQuadLength+1)*(usMaxQuadLength+1)];
			for (int i=0; i<(usMaxQuadLength+1)*(usMaxQuadLength+1); i++) afHMap[i] = -1;
			for (int line=0; line<usMaxQuadLength+1; line++)
			{
				memcpy(&afHMap[line*(usMaxQuadLength+1)], &afHeightmap[(y*usMaxQuadLength+line)*iWidth+x*usMaxQuadLength], (usMaxQuadLength+1)*sizeof(float));
//				memcpy(acQuadShades+line*(usMaxQuadLength+1), acShades+(y*usMaxQuadLength+line)*iWidth+x*usMaxQuadLength, (usMaxQuadLength+1)*sizeof(unsigned char));
			}
			CQuad *pQuad = new CQuad(x*usMaxQuadLength, y*usMaxQuadLength, usMaxQuadLength, afHMap, m_pData);
			m_pData->m_apQuadList[uiTerrainWidth*y+x] = pQuad;
		}
	delete []afHeightmap;
	delete []acShades;
}

void CRAWDataLoader::CalcShades(float *afHeightmap, unsigned char *acShades)
{
	unsigned char	*pCurVertex;
	CVector			vNormal;
	CVector			vSun(1, 10, 1);
	int				i;
	float			fScalar;
	float			fAverage;
	int				iStep;

	int iHeight = m_pData->m_uiTerrainPointHeight;
	int iWidth = m_pData->m_uiTerrainPointWidth;
	

	printf("\nCalculating vertex-shades      ");
	vSun.normalize();
	pCurVertex = acShades;
	iStep = iHeight/100;
	for (int y=0; y<iHeight; y++)
	{
		if (y%iStep == 0) CTools::OutputProgressBar(y/iStep);
		for (int x=0; x<iWidth; x++)
		{
			int myx = x;
			int myy = y;
			vNormal.set(0.0f, 0.0f, 0.0f);
			if (x==iWidth-1) myx--;
			if (y==iHeight-1) myy--;
			CalcRectNormal(vNormal, myx, myy, afHeightmap);
			
			// calculate scalar (0<=fScalar<=2)
			fScalar = vNormal.x*vSun.x+vNormal.y*vSun.y+vNormal.z*vSun.z+1;

			*pCurVertex = (unsigned char)(fScalar*127);

			pCurVertex++;
		}
	}
	// smooth the shades ...
	for (int iNoRun=0; iNoRun<2; iNoRun++)
	{
		printf("\nSmoothing shades pass #%d   ", iNoRun+1);  
		for (int y=0; y<iHeight; y++)
		{
			if (y%iStep == 0) CTools::OutputProgressBar(y/iStep);
			for (int x=0;x<iWidth; x++)
			{
				i = 0;
				fAverage = 0;

				for (int iStep=-1; iStep<=1; iStep+=2)
				{
					if (x+iStep>=0 && x+iStep<iWidth)
					{
						i++;
						fAverage += acShades[y*iWidth+x+iStep];
					}
					if (y+iStep>=0 && y+iStep<iHeight)
					{
						i++;
						fAverage += acShades[(y+iStep)*iWidth+x];
					}
				}
				acShades[y*iWidth+x] = uchar(fAverage/i);
			}
		}
	}

}

// calculates the average-normal of an rectangle inside the heightmap
// upper left corner: iXPos, iYPos
void CRAWDataLoader::CalcRectNormal(CVector vNormal, int iXPos, int iYPos, float *afHeightmap)
{
	CVector vNormal1, vNormal2;

	int iWidth = m_pData->m_uiTerrainPointWidth;
	int iHeight = m_pData->m_uiTerrainPointHeight;

	if (!(iXPos>=0 && iYPos>=0 && iXPos+1<iWidth && iYPos+1<iHeight))
	{
		vNormal.set(0.0f, 0.0f, 0.0f);
		return;
	}

	// left-upper, right-upper, left-lower corners
	CVector vVec1(0, 0, afHeightmap[iYPos*iWidth+iXPos]);
	CVector vVec2(1, 0, afHeightmap[iYPos*iWidth+iXPos+1]);
	CVector vVec3(0, 1, afHeightmap[(iYPos+1)*iWidth+iXPos]);
	CTools::CalcTriangleNormal(vNormal1, vVec1, vVec2, vVec3);

	// right-upper, right-lower, left-lower corners
	CVector vVec4(1, 1, afHeightmap[(iYPos+1)*iWidth+iXPos+1]);
	CTools::CalcTriangleNormal(vNormal2, vVec2, vVec4, vVec3);

	vNormal = vNormal1 + vNormal2;

	//for (int i=0; i<3; i++) afNormal[i] = normal1[i]+normal2[i];

	vNormal.normalize();
}



/*
void CTerrainDataManager::CalcAverageHeight()
{
	m_oData->m_fAverageHeight = 0;
	for (int i=0; i<m_oData->m_iHeight*m_oData->m_iWidth; i++)
	{
		for (int j=0; j<m_oData->m_pQuadList[i]->m_usLength)
		{
			
		}
	}

	for (int i=0; i<m_pTerrainData->m_iWidth*m_pTerrainData->m_iHeight; i++)
	{
		m_pTerrainData->m_afHeightmap[i] *= m_fDeltaZ;
		m_fAverageTerrainHeight += m_pTerrainData->m_afHeightmap[i]/(m_pTerrainData->m_iWidth*m_pTerrainData->m_iHeight);
}
}

  */
