#include "TerrainFileTextureFactory.h"
#include <fstream>

const int MAX_FILENAME_LENGTH = 2048;

#undef LoadImage
void LoadImage(const char* szFilename,int& pWidth,int& pHeight,Uint8** pBuffer,bool bAlpha = false);

extern std::ofstream m_Logfile;

namespace Demeter
{

TerrainFileTextureFactory::TerrainFileTextureFactory(std::string const & latticeBaseName, std::string const & latticeTexExt)
    : m_LatticeBaseName( latticeBaseName )
    , m_LatticeTexExt( latticeTexExt )
{
	if (Settings::GetInstance()->IsVerbose())
	{
		m_Logfile << "TerrainFileTextureFactory::TerrainFileTextureFactory()" << endl;
	}
}

Texture* TerrainFileTextureFactory::GetTexture(int index,float originX,float originY,float width,float height)
{
	int latticeX, latticeY;
	m_pTerrain->GetLatticePosition(latticeX, latticeY);

	int combinedIndex = (latticeX << 24) | (latticeY << 16) | index;
    
	Texture* pTexture = m_Textures[combinedIndex];
    
	if (!pTexture)
	{
        int indexX = index % m_pTerrain->GetNumberOfTextureTilesWidth();
        int indexY = index / m_pTerrain->GetNumberOfTextureTilesWidth();
        
        char szTexName[MAX_FILENAME_LENGTH];
        sprintf( szTexName, "%sTex.%d-%d.%d-%d.%s", m_LatticeBaseName.c_str(), latticeX, latticeY, indexX, indexY, m_LatticeTexExt.c_str() );
        
		if (Settings::GetInstance()->IsVerbose())
		{
			m_Logfile << "Loading texture file " << szTexName << endl;
		}
        
    	char szFullTexName[MAX_FILENAME_LENGTH];
    	if ( strstr(szTexName, "\\") || strstr(szTexName, "/") )
    		sprintf(szFullTexName, szTexName);
    	else
    		Settings::GetInstance()->PrependMediaPath(szTexName, szFullTexName);
        
        FILE *fileExist = fopen( szFullTexName, "r" );
        if ( !fileExist )
        {
    		if (Settings::GetInstance()->IsVerbose())
    		{
    			m_Logfile << "Texture file " << szTexName << " doesn't exist !" << endl;
    		}
            
            return TerrainTextureFactory::GetTexture(index, originX, originY, width, height);
        }
        fclose( fileExist );
        
        int textureWidth, textureHeight;
        Uint8* pTextureImage;
        LoadImage(szTexName, textureWidth, textureHeight, &pTextureImage, false);
        
        pTexture = new Texture(pTextureImage, textureWidth, textureHeight, textureWidth, 0, true, false, false);
        
        m_Textures[combinedIndex] = pTexture; 
        delete[] pTextureImage;	
        
		if (Settings::GetInstance()->IsVerbose())
		{
			m_Logfile << "Finished loading texture file" << endl;
		}
	}

	return pTexture;
}

}
