///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   definition of CSCNDataLoader
//   interpretes given .SCN config file and reads terrain-data
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//	
//	6/13/2001	moved cfg-filereader here - zzed
//	6/14/2001	implemented CVector - zzed
//
//
///////////////////////////////////////////////////////////////////////////


#include "SCNDataLoader.h"
#include "Tools.h"
#include "Texture.h"
#include "ResourceManager.h"
#include "Config.h"
//TEST
//#include "TextureZoom.h"

#include <stdio.h>

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
#include <string.h>
#include <cmath>

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

int CSCNDataLoader::LoadData(CTerrainData *pData, char *strFilename)
{

        printf("CSCNDataLoader::LoadData - %s  %s\n", strFilename, m_cRawFile);
	m_pData = pData;
	
	SetDefaultValues();
	ReadConfigFile(strFilename);
	LoadSCNData(m_cRawFile);

	return 0;
}


CTexture* CSCNDataLoader::LoadTexture(int iTextureIndex, int iMipmapIndex)
{
	// Loads a 256*256 pixel RGB bitmap in RAW
#ifdef WIN32
	FILE *fInput = fileopen("Texture_erde.raw", "r");
#else
	FILE *fInput = fopen("Texture_erde.raw", "r");
#endif

	unsigned char *ucBitmap = new unsigned char[256*256*3];
	fread(ucBitmap, 3, 256*256, fInput);
	fclose(fInput);
	return 0;
}

void CSCNDataLoader::SetDefaultValues()
{
	m_pData->m_fDeltaZ					= 0.0004f;
	m_pData->m_uiQuadWidth				= 32;     
	m_pData->m_fDetailLevel				= 5.0f;
	m_pData->m_fQuadClippingDist		= 100.0f;
	m_pData->m_fMinQuadClippingDist		= m_pData->m_fQuadClippingDist;
	m_pData->m_fMaxQuadClippingDist		= 2000.0f;
	m_pData->m_fRatioQuadClippingDist	= 1.0f;
	m_pData->m_uiMaxNoTextures			= 0;
	m_pData->m_uiNoMipmaps				= 0;
	m_pData->m_fWaterLevel				= 0.0f;
}

void CSCNDataLoader::ReadConfigFile(char *strFilename)
{

        printf("CSCNDataLoader::ReadConfigFile - %s\n", strFilename);
	m_pData->m_vCameraPos.x = Config.GetFloat("m_fCameraXPos");
	m_pData->m_vCameraPos.y = Config.GetFloat("m_fCameraYPos");
	m_pData->m_vCameraPos.z = Config.GetFloat("m_fCameraZPos");
	m_pData->m_fDeltaZ = Config.GetFloat("m_fDeltaZ");
	m_pData->m_fDetailLevel = Config.GetFloat("m_fDetailLevel");
	m_pData->m_uiQuadWidth = (uint)Config.GetInteger("m_usMaxQuadLength");
	m_iRawFormat = Config.GetInteger("RawFormat");
	Config.GetString(m_cRawFile, "m_sSceneryFileName");
	m_fLowTextureAlt = Config.GetFloat("LowTextureAltitude");
	m_fHighTextureAlt = Config.GetFloat("HighTextureAltitude");
	m_pData->m_fWaterLevel = Config.GetFloat("m_fWaterLevel");
	m_pData->m_uiSectorWidth = (uint)Config.GetInteger("m_uiSectorWidth");
	m_pData->m_ucTexQuadSpan = (uchar)Config.GetInteger("m_ucTexQuadSpan");
	if (m_iRawFormat>2)
	{
		printf("ERROR: unknown constant in cfg-file for RawFormat");
		exit(-1);
	}
}

void CSCNDataLoader::LoadSCNData(char *strFilename)
{
	int success;
	long lsize, iFile;
	unsigned short *buffer;
	float fwidth;

	int iWidth, iHeight;
	float *afHeightmap;
	unsigned char *acShades;

	// Load SCN data and save to internal structure

	printf("CSCNDataLoader::LoadSCNData - %s\n", strFilename);

	m_pData->m_hTerrainFile = fopen(strFilename, "rb");

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

	if (m_iRawFormat==TE_SCN_16BIT)
	{
		lsize /= 2;									// SCN uses 16bit height values, so we have to divide by 2
													// to get the real number of height map values
													// compute terrain width from size of file
	}
	fwidth = (float)sqrt((double)lsize);
	iWidth = (int)fwidth;
	iHeight = (int)fwidth;					

	/*afHeightmap = new float[lsize];
	buffer = new unsigned short[lsize];

	success = _read(iFile, buffer, (lsize*sizeof(short)));

	if (m_iRawFormat==TE_SCN_16BIT)
	{
		// convert SCN 16bit values to float
		for (int y=0; y<iHeight; y++)					
			for (int x=0; x<iWidth; x++)
			{
				afHeightmap[y*iHeight+x] =  (float)(((unsigned short *)buffer)[y*iHeight+x])*m_pData->m_fDeltaZ;
			} 
	} else
	{
		// read 8 bit values and convert to float
		for (int y=0; y<iHeight; y++)					
			for (int x=0; x<iWidth; x++)
			{
				afHeightmap[y*iHeight+x] =  (float)(((unsigned char*)buffer)[y*iHeight+x])*m_pData->m_fDeltaZ;
			} 
	}
	
	delete []buffer; */

#ifdef WIN32
	_close(iFile);
#else
	close(iFile);
#endif
	m_pData->m_uiTerrainQuadHeight = iHeight/m_pData->m_uiQuadWidth;
	//m_pData->m_uiTerrainQuadHeight = 8;
	m_pData->m_uiTerrainQuadWidth = iWidth/m_pData->m_uiQuadWidth;
	//m_pData->m_uiTerrainQuadWidth = 8;
	m_pData->m_uiTerrainPointHeight = iHeight;
	m_pData->m_uiTerrainPointWidth = iWidth; 
	//m_pData->m_uiTerrainPointHeight = 32;
	//m_pData->m_uiTerrainPointWidth = 32; 

	// only needed for smooth-shading, is being done in CQuadTexture and CLightingTexture
	//acShades = new unsigned char[iHeight*iWidth];
	//CalcShades(afHeightmap, acShades);

	// load textures ...
	RHANDLE handle;
	char cFile[255];
	
	Config.GetString(cFile, "LowTextureFile");
	m_pData->m_pLowTex = new CTextureZoom(cFile);
	ResManager.InsertResource(&handle, m_pData->m_pLowTex);
	m_pData->m_pLowTex = reinterpret_cast<CTextureZoom*>(ResManager.Lock(handle));
	Config.GetString(cFile, "HighTextureFile");
	m_pData->m_pHighTex = new CTextureZoom(cFile);
	ResManager.InsertResource(&handle, m_pData->m_pHighTex);
	m_pData->m_pHighTex = reinterpret_cast<CTextureZoom*>(ResManager.Lock(handle)); 


	// ok, now we've got all our heightdata in afHeightmap (floats) and shades in acShades
	// let's transform it to quads ...

	unsigned short	usMaxQuadLength = m_pData->m_uiQuadWidth;
	unsigned int	uiTerrainHeight = m_pData->m_uiTerrainQuadHeight;
	unsigned int	uiTerrainWidth	= m_pData->m_uiTerrainQuadWidth;
	uint			uiMaxCount		= uiTerrainHeight*uiTerrainWidth;
	uint			uiStep			= uiMaxCount/100;
	uint			uiCounter		= 0;
	float			*afHMap;
//	unsigned char	*acQuadShades;

/*	printf("\nCreating quads (%d*%d)           ", uiTerrainWidth, uiTerrainHeight);
	m_pData->m_apQuadList = new CQuad*[uiTerrainWidth*uiTerrainHeight];
	for (unsigned int y=0; y<uiTerrainHeight; y++)
		for (unsigned int x=0; x<uiTerrainWidth; x++)
		{
			++uiCounter;
			if (uiCounter%uiStep==0) CTools::OutputProgressBar(uiCounter/uiStep);
			afHMap = new float[(usMaxQuadLength+1)*(usMaxQuadLength+1)];
			for (int i=0; i<(usMaxQuadLength+1)*(usMaxQuadLength+1); i++) afHMap[i] = -1;
//			acQuadShades = new unsigned char[(usMaxQuadLength+1)*(usMaxQuadLength+1)];
			for (int line=0; line<usMaxQuadLength+1; line++)
			{
				memcpy(&afHMap[line*(usMaxQuadLength+1)], &afHeightmap[(y*usMaxQuadLength+line)*iWidth+x*usMaxQuadLength], (usMaxQuadLength+1)*sizeof(float));
//				memcpy(acQuadShades+line*(usMaxQuadLength+1), acShades+(y*usMaxQuadLength+line)*iWidth+x*usMaxQuadLength, (usMaxQuadLength+1)*sizeof(unsigned char));
			}
			CQuad *pQuad = new CQuad(x*usMaxQuadLength, y*usMaxQuadLength, usMaxQuadLength, afHMap, m_pData);
//			pQuad->m_acShades = acQuadShades;
			m_pData->m_apQuadList[uiTerrainWidth*y+x] = pQuad;
		}*/
//	delete []afHeightmap;
}

void CSCNDataLoader::CalcShades(float *afHeightmap, unsigned char *acShades)
{
	unsigned char	*pCurVertex;
	float			afNormal[3];
	CVector			vSun(1.0f, 10.0f, 1.0f);
	int				i;
	float			fScalar;
	float			fAverage;
	int				iStep;

	int iHeight = m_pData->m_uiTerrainPointHeight;
	int iWidth = m_pData->m_uiTerrainPointWidth;
	

	printf("\nCalculating vertex-shades        ");
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
			afNormal[0] = 0;
			afNormal[1] = 0;
			afNormal[2] = 0;
			if (x==iWidth-1) myx--;
			if (y==iHeight-1) myy--;
			CalcRectNormal(afNormal, myx, myy, afHeightmap);
			
			// calculate scalar (0<=fScalar<=2)
			fScalar = afNormal[0]*vSun.x+afNormal[1]*vSun.y+afNormal[2]*vSun.z+1;

			*pCurVertex = (unsigned char)(fScalar*127); //(fScalar*127*2>255 ? 255 : fScalar*127*2);  // we wanna light up the terrain!

			pCurVertex++;
		}
	}
	// smooth the shades ...
	for (int iNoRun=0; iNoRun<2; iNoRun++)
	{
		printf("\nSmoothing shades pass #%d         ", iNoRun+1);  
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
void CSCNDataLoader::CalcRectNormal(float afNormal[], int iXPos, int iYPos, float *afHeightmap)
{
	float normal1[3], normal2[3];
    int i;

	int iWidth = m_pData->m_uiTerrainPointWidth;
	int iHeight = m_pData->m_uiTerrainPointHeight;

	if (!(iXPos>=0 && iYPos>=0 && iXPos+1<iWidth && iYPos+1<iHeight))
	{
		for (i=0; i<3; i++) afNormal[i] = 0;
		return;
	}

	// left-upper, right-upper, left-lower corners
	float vec1[3] = {0, 0, afHeightmap[iYPos*iWidth+iXPos]};
	float vec2[3] = {1, 0, afHeightmap[iYPos*iWidth+iXPos+1]};
	float vec3[3] = {0, 1, afHeightmap[(iYPos+1)*iWidth+iXPos]};
	CalcTriangleNormal(normal1, vec1, vec2, vec3);

	// right-upper, right-lower, left-lower corners
	float vec4[3] = {1, 1, afHeightmap[(iYPos+1)*iWidth+iXPos+1]};
	CalcTriangleNormal(normal2, vec2, vec4, vec3);

	for (i=0; i<3; i++) afNormal[i] = normal1[i]+normal2[i];

	// normalize afNormal
	float length;
	length = (float)sqrt(afNormal[0]*afNormal[0]+afNormal[1]*afNormal[1]+afNormal[2]*afNormal[2]);
	for (i=0; i<3; i++) afNormal[i] /= length;
}

void CSCNDataLoader::CalcTriangleNormal(float afNormal[], float afVec1[], float afVec2[], float afVec3[])
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
