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

#include "TerrainDataLoader.h"

#ifdef WIN32
#include <windows.h>
#endif

#include <stdio.h>


CTexture* CTerrainDataLoader::LoadTexture(int iTextureIndex, int iMipmapIndex)
{
	return 0;
}

int CTerrainDataLoader::LoadData(CTerrainData *oData, char *strFilename)
{
        printf("CTerrainDataLoader::LoadData - %s\n", strFilename);
	return 0;
}
