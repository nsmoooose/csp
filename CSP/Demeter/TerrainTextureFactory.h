#ifndef __TERRAINTEXTUREFACTORY_H__
#define __TERRAINTEXTUREFACTORY_H__


#include "Terrain.h"
#include <map>
#include <vector>

#if defined(__GNUC__) || defined(__INTEL_COMPILER)
  #if __GNUC__ >= 3
    #include <ext/hash_map>
    #if __GNUC_MINOR__ > 0
	#define HASH_MAP __gnu_cxx::hash_map
        #define HASH __gnu_cxx::hash
    #else
	#define HASH_MAP std::hash_map
	#define HASH std::hash
    #endif
  #else
    #include <hash_map>
    #define HASH_MAP std::hash_map
    #define HASH std::hash
  #endif
#else
  #ifdef _MSC_VER 
   #if (_MSC_VER <= 1200) && defined(_STLP_WIN32)
    #include <hash_map>
    #define HASH_MAP std::hash_map
    #define HASH std::hash
  #elif (_MSC_VER == 1300) 
    #include <hash_map>
    #define HASH_MAP std::hash_map
    #define HASH std::hash_compare
  #elif (_MSC_VER > 1300)
    #include <hash_map>
    #define HASH_MAP stdext::hash_map
    #define HASH stdext::hash_compare
  #endif
  #else
    #error "PLEASE PROVIDE CORRECT #INCLUDE<HASH_MAP> STATEMENT FOR YOUR PLATFORM!"
  #endif
#endif
   

namespace Demeter
{

class TERRAIN_API TerrainTextureFactory : public TextureFactory
{
public:

	TerrainTextureFactory();
	virtual ~TerrainTextureFactory();

	virtual Texture* GetTexture(int index,float originX,float originY,float width,float height);

	virtual void UnloadTexture(int index);

	virtual void SetTerrain(Terrain * pTerrain);
	void SetTerrainLattice(TerrainLattice * pTerrainLattice);

protected:

	Terrain* m_pTerrain;
	TerrainLattice * m_pTerrainLattice;

	HASH_MAP<int,Texture*>   m_Textures;
	std::vector<Uint8*> m_BaseTextures;


};

};


#endif

