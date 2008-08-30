#ifndef __TERRAINFILETEXTUREFACTORY_H__
#define __TERRAINFILETEXTUREFACTORY_H__

#include "TerrainTextureFactory.h"

namespace Demeter
{

class TERRAIN_API TerrainFileTextureFactory : public TerrainTextureFactory
{
public:
	TerrainFileTextureFactory(std::string const & latticeBaseName, std::string const & latticeTexExt);

	virtual Texture* GetTexture(int index,float originX,float originY,float width,float height);
    
protected:
    std::string const m_LatticeBaseName;
    std::string const m_LatticeTexExt;
};

}

#endif
