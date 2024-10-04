#ifndef __TERRAINTEXTUREFACTORY_H__
#define __TERRAINTEXTUREFACTORY_H__


#include "Terrain.h"
#include <map>
#include <unordered_map>
#include <vector>

namespace Demeter
{

class DEMETER_EXPORT TerrainTextureFactory : public TextureFactory
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

	std::unordered_map<int,Texture*>   m_Textures;
	std::vector<Uint8*> m_BaseTextures;


};

}


#endif

