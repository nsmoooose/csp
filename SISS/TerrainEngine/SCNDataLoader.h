///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   declaration of CSCNDataLoader
//   interpretes given .SCN config file and reads terrain-data
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//	
//	6/13/2001	moved cfg-filereader here - zzed
//
//
///////////////////////////////////////////////////////////////////////////

#if !defined(SCNDATALOADER_H)
#define SCNDATALOADER_H

#include "TerrainDataLoader.h"
#include "TerrainData.h"

const int TE_SCN_UNDEFINED	= 0;
const int TE_SCN_8BIT		= 1;
const int TE_SCN_16BIT		= 2;

class CSCNDataLoader : CTerrainDataLoader
{
public:
	virtual CTexture* LoadTexture(int iTextureIndex, int iMipmapIndex);
	virtual int LoadData(CTerrainData *oData, char *strFilename);
private:
	CTerrainData	*m_pData;
	int				m_iRawFormat;
	char			m_cRawFile[256];
	char			*m_cLowTextureFile;
	char			*m_cHighTextureFile;
	float			m_fLowTextureAlt;
	float			m_fHighTextureAlt;

	void SetDefaultValues();
	void CalcShades(float *afHeightmap, unsigned char *acShades);
	void LoadSCNData(char *strFilename);
	void CalcRectNormal(float afNormal[], int iXPos, int iYPos, float *afHeightmap);
	void ReadConfigFile(char *strFilename);
	void CalcTriangleNormal(float afNormal[], float afVec1[], float afVec2[], float afVec3[]);
};

#endif
