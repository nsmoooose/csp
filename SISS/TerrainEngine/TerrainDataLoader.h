///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   <file-description>
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

#if !defined(TERRAINDATALOADER_H)
#define TERRAINDATALOADER_H

#include "Texture.h"
#include "TerrainData.h"


class CTerrainDataLoader
{
public:
	virtual CTexture* LoadTexture(int iTextureIndex, int iMipmapIndex);
	virtual int LoadData(CTerrainData *oData, char *strFilename);
};


#endif
