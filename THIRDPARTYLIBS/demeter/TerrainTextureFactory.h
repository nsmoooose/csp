#ifndef __TERRAINTEXTUREFACTORY_H__
#define __TERRAINTEXTUREFACTORY_H__

#include "Terrain.h"
#include <map>
#include <vector>
#ifdef _WIN32
 #include <hash_map>
 #define STLNS std
#else
 #ifdef _GNUCXX
   #include <ext/hash_map>  
   #define STLNS __gnu_cxx
 #else
   #include <hash_map>  
   #define STLNS std
 #endif
#endif

namespace Demeter
{

class TERRAIN_API TerrainTextureFactory : public TextureFactory
{
public:

	TerrainTextureFactory();
	~TerrainTextureFactory();

	virtual Texture* GetTexture(int index,float originX,float originY,float width,float height);

	virtual void UnloadTexture(int index);

	virtual void SetTerrain(Terrain * pTerrain);
	void SetTerrainLattice(TerrainLattice * pTerrainLattice);

protected:

	Terrain* m_pTerrain;
	TerrainLattice * m_pTerrainLattice;

	STLNS::hash_map<int,Texture*>   m_Textures;
	std::vector<Uint8*> m_BaseTextures;


};

};


#endif
