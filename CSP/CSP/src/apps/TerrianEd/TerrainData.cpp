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
#include "TerrainData.h"
#include "Tools.h"




CTerrainData::CTerrainData(char* sFilename)
{
	int success;
	long lsize, iFile;
	unsigned char *buffer;
	float fwidth;

	// Load TerraGen raw data and save to internal structure

	iFile = _open(sFilename, _O_RDONLY);
	if(iFile==-1)
	{	
		MessageBox(NULL, "unable to open terrain file", NULL, MB_OK);
		char text[100];
		gets(text);
		exit(1);
	}

	lsize = _filelength(iFile); 

	m_afHeightmap = new float[lsize];
	buffer = new unsigned char[lsize];

	success = _read(iFile, buffer, (lsize*sizeof(char)));

	fwidth = sqrt((double)lsize);				// compute terrain width from size of file
	m_iWidth = m_iHeight = (int)fwidth;

	// convert RAW 8-bit values to float
	for (int y=0; y<m_iHeight; y++)					
		for (int x=0; x<m_iWidth; x++)
		{
			m_afHeightmap[y*m_iHeight+x] = 
				(float)(((unsigned char *)buffer)[y*m_iHeight+x]);
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
	unsigned char	*pCurVertex;
	float			afNormal[3];
	float			afSun[3]		= {1, 10, 1};
	int				i;
	float			fScalar;
	float			fAverage;
	int				iStep;

	

	printf("\nCalculating vertex-shades      [          ]");
	CTools::NormalizeVector(afSun);
	m_acShades = new unsigned char[m_iWidth*m_iHeight];
	pCurVertex = m_acShades;
	iStep = m_iHeight/10;
	for (int y=0; y<m_iHeight; y++)
	{
		if (y%iStep == 0) CTools::OutputProgressBar(y/iStep);
		for (int x=0; x<m_iWidth; x++)
		{
			int myx = x;
			int myy = y;
			for (i=0; i<3; i++) afNormal[i] = 0;
			if (x==m_iWidth-1) myx--;
			if (y==m_iHeight-1) myy--;
			CalcRectNormal(afNormal, myx, myy);
			
			// calculate scalar (0<=fScalar<=2)
			fScalar = afNormal[0]*afSun[0]+afNormal[1]*afSun[1]+afNormal[2]*afSun[2]+1;

			*pCurVertex = (unsigned char)(fScalar*127);

			pCurVertex++;
		}
	}
	// smooth the shades ...
	for (int iNoRun=0; iNoRun<4; iNoRun++)
	{
		printf("\nSmoothing shades pass #%d       [          ]", iNoRun+1);  
		for (y=0; y<m_iHeight; y++)
		{
			if (y%iStep == 0) CTools::OutputProgressBar(y/iStep);
			for (int x=0;x<m_iWidth; x++)
			{
				i = 0;
				fAverage = 0;

				for (int iStep=-1; iStep<=1; iStep+=2)
				{
					if (x+iStep>=0 && x+iStep<m_iWidth)
					{
						i++;
						fAverage += m_acShades[y*m_iWidth+x+iStep];
					}
					if (y+iStep>=0 && y+iStep<m_iHeight)
					{
						i++;
						fAverage += m_acShades[(y+iStep)*m_iWidth+x];
					}
				}
				m_acShades[y*m_iWidth+x] = fAverage/i;
			}
		}
	}

}

// calculates the average-normal of an rectangle inside the heightmap
// upper left corner: iXPos, iYPos
void CTerrainData::CalcRectNormal(float afNormal[], int iXPos, int iYPos)
{
	float normal1[3], normal2[3];

	if (!(iXPos>=0 && iYPos>=0 && iXPos+1<m_iWidth && iYPos+1<m_iHeight))
	{
		for (int i=0; i<3; i++) afNormal[i] = 0;
		return;
	}

	// left-upper, right-upper, left-lower corners
	float vec1[3] = {0, 0, m_afHeightmap[iYPos*m_iWidth+iXPos]};
	float vec2[3] = {1, 0, m_afHeightmap[iYPos*m_iWidth+iXPos+1]};
	float vec3[3] = {0, 1, m_afHeightmap[(iYPos+1)*m_iWidth+iXPos]};
	CalcTriangleNormal(normal1, vec1, vec2, vec3);

	// right-upper, right-lower, left-lower corners
	float vec4[3] = {1, 1, m_afHeightmap[(iYPos+1)*m_iWidth+iXPos+1]};
	CalcTriangleNormal(normal2, vec2, vec4, vec3);

	for (int i=0; i<3; i++) afNormal[i] = normal1[i]+normal2[i];

	CTools::NormalizeVector(afNormal);
}

void CTerrainData::CalcTriangleNormal(float afNormal[], float afVec1[], float afVec2[], float afVec3[])
{
	float diff1[3], diff2[3];

	for (int i=0; i<3; i++)
	{
		diff1[i] = afVec2[i] - afVec1[i];
		diff2[i] = afVec3[i] - afVec1[i];
	}

	afNormal[0] = diff1[1]*diff2[2]-diff1[2]*diff2[1];
	afNormal[1] = -diff1[2]*diff2[0]+diff1[0]*diff2[2];
	afNormal[2] = diff1[0]*diff2[1]-diff1[1]*diff2[0];

	CTools::NormalizeVector(afNormal);
}
