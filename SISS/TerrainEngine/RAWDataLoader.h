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
//
//
//
//
///////////////////////////////////////////////////////////////////////////

#if !defined(RAWDATALOADER_H)
#define RAWDATALOADER_H

#include "TerrainDataLoader.h"
#include "TerrainData.h"

class CRAWDataLoader : CTerrainDataLoader
{
public:
	virtual CTexture* LoadTexture(int iTextureIndex, int iMipmapIndex);
	virtual int LoadData(CTerrainData *oData, char *strFilename);
private:
	CTerrainData   *m_pData;

	void SetDefaultValues();
	void CalcShades(float *afHeightmap, unsigned char *acShades);
	void LoadRAWData(char *strFilename);
	void CalcRectNormal(CVector vNormal, int iXPos, int iYPos, float *afHeightmap);
};

#endif
