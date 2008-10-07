#ifdef _WIN32
#pragma warning( disable : 4786 )
#endif

#include <cstddef>

#include <string>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

//#ifdef DEMETER_MEMORYMANAGER
//#include "mmgr.h"
//#endif

#include "TerrainTextureFactory.h"


#undef LoadImage

void LoadImage(const char* szFilename,int& pWidth,int& pHeight,Uint8** pBuffer,bool bAlpha = false);

extern std::ofstream m_Logfile;


namespace Demeter
{



const float texelsPerMeter = 1.0f;
const float texelSpacing = 1.0f / texelsPerMeter;
const float textureSize = 256.0f;

inline float limit255(float a)		//Check for invalid values
{
	if(a < 0.0f) {
		return 0.0f;
	}
	else if(a > 255.0f) {
		return 255.0f;
	}
	else {
		return a;
	}
}

inline float texture_factor_low( float h1, float h2, float f)
{
	if (f < h1)
		return 1.0;
	else if (f > h2)
		return 0.0;
	else 
		return ( (f - h2) / (h1 - h2) );

}

inline float texture_factor_high( float h1, float h2, float f)
{
	if (f < h1)
		return 0.0;
	else if ( f > h2 )
		return 1.0;
	else 
		return ( ( f - h2 ) / ( h1 - h2 ));
}

inline float texture_factor_mid2( float h1, float h2, float h3, float h4, float f)
{
	if (f < h1)
		return 0.0f;
	else if ( f > h4) 
		return 0.0f;
	else if (f > h2 && f < h3)
		return 1.0f;
	else if (f >= h1 && f <= h2)
		return ( ( f - h1 ) / ( h2 - h1 ) );
	else if (f >= h3 && f <= h4)
		return ( ( f - h3 ) / ( h4 - h3) );
	else 
		return 0;  // we should not get here.

}

inline float texture_factor_mid(float h1, float h2, float h3, float f)		//Check for percentage of color
{
	//float t;
	if ( f < h1)
		return 0.0f;

	else if ( f > h3 )
		return 0.0f;

	else if ( f < h2)
		return ( (f - h1)/(h2 - h1) );

	else 
		return ( (f - h3)/(h2 - h3) );
//	t = (f - (float)abs(h1 - (int)h2)) / f;

//	if(t < 0.0f) t = 0.0f;
//	else if(t > 1.0f) t = 1.0f;

//	return t;
}

TerrainTextureFactory::TerrainTextureFactory()
{
	if (Settings::GetInstance()->IsVerbose())
	{
		m_Logfile << "TerrainTextureFactory::TerrainTextureFactory()" << std::endl;
	}

	
	int height, width;
	Uint8* pTextureImage;
	
	m_pTerrain = NULL;
	m_pTerrainLattice = NULL;


	m_BaseTextures.resize(6);

	LoadImage("Water1.tga", width, height, &pTextureImage, false);
	m_BaseTextures[0] = pTextureImage;

//	LoadImage("stone4.bmp",  width, height, &pTextureImage, false);
	LoadImage("Mud1.tga",  width, height, &pTextureImage, false);
	m_BaseTextures[1] = pTextureImage;

	//LoadImage("Snow1.tga", width, height, &pTextureImage, false);
	LoadImage("Grass1.bmp",  width, height, &pTextureImage, false);
	m_BaseTextures[2] = pTextureImage;

	LoadImage("Grass2.bmp", width, height, &pTextureImage, false);
	m_BaseTextures[3] = pTextureImage;

	LoadImage("Stone2.bmp", width, height, &pTextureImage, false);
	m_BaseTextures[4] = pTextureImage;

	LoadImage("Stone3.bmp", width, height, &pTextureImage, false);
	m_BaseTextures[5] = pTextureImage;



}

TerrainTextureFactory::~TerrainTextureFactory()
{
	delete[] m_BaseTextures[0];
	delete[] m_BaseTextures[1];
	delete[] m_BaseTextures[2];
	delete[] m_BaseTextures[3];
	delete[] m_BaseTextures[4];
	delete[] m_BaseTextures[5];
}

void TerrainTextureFactory::SetTerrain(Terrain * pTerrain)
{
	m_pTerrain = pTerrain;
}

void TerrainTextureFactory::SetTerrainLattice(TerrainLattice * pTerrainLattice)
{
	m_pTerrainLattice = pTerrainLattice;
}


Texture* TerrainTextureFactory::GetTexture(int index,float originX,float originY,float width,float height)
{

	int latticeX, latticeY;
	m_pTerrain->GetLatticePosition(latticeX, latticeY);

	int combinedIndex = (latticeX << 24) | (latticeY << 16) | index;
    
	Texture* pTexture = m_Textures[combinedIndex];
	if (!pTexture)
	{

		if (Settings::GetInstance()->IsVerbose())
		{
			m_Logfile << "Generating Texture" << latticeX << ":" << latticeY << std::endl;
		}


		if (m_pTerrain)
		{

//			int latticeX, latticeY;
//			m_pTerrain->GetLatticePosition(latticeX, latticeY);

//			originX += latticeX*m_pTerrain->GetWidth();
//			originY += latticeY*m_pTerrain->GetHeight();

			float texelSpacing = width / textureSize;
			Uint8* pTex1 = m_BaseTextures[0];
			Uint8* pTex2 = m_BaseTextures[1];
			Uint8* pTex3 = m_BaseTextures[2];
			Uint8* pTex4 = m_BaseTextures[3];
			Uint8* pTex5 = m_BaseTextures[4];
			Uint8* pTex6 = m_BaseTextures[5];

			//std::cout << "Generating Texture" << latticeX << ":" << latticeY << std::endl;
			//std::cout << originX << "," << originY << "  " << width << "," << height << "  " << textureSize << "\n";

			Uint8* pImage = new Uint8[(int)((textureSize + 1.0f) * (textureSize + 1.0f)) * 3];
			Uint8* Idx = pImage;
			for (float y = originY; y < originY + height; y += texelSpacing)
			{
				for (float x = originX; x < originX + width; x += texelSpacing)
				{
					float f0, f1, f2, f3, f4, f5;
					float elev = m_pTerrain->GetElevation(x,y);
	
					f0 = texture_factor_low( 1, 10, elev);					//Get percentage of color of pixel x/z from bitmap1, if height is zero (black) then bitmap1 receives 100%
					f1 = texture_factor_mid( 1, 5, 20, elev);
					f2 = texture_factor_mid2(5, 25, 500, 600, elev);					//Get percentage of color of pixel x/z from bitmap2, if height is 256  (white) then bitmap2 receives 100%
					f3 = texture_factor_mid2(10, 100, 800, 1000, elev);
					f4 = texture_factor_high(500, 1500 , elev);
					f5 = texture_factor_high(1000, 5000, elev);

					Uint8 red     = static_cast<unsigned char>(
						              limit255( f0 * *pTex1++ +
									f1 * *pTex2++ +
									f2 * *pTex3++ +
									f3 * *pTex4++ +
									f4 * *pTex5++ +
									f5 * *pTex6++ )
									);

					Uint8 green   = static_cast<unsigned char>(
						               limit255(f0 * *pTex1++ +
									f1 * *pTex2++ +
									f2 * *pTex3++ +
									f3 * *pTex4++ +
									f4 * *pTex5++ +
									f5 * *pTex6++ )
									);
							
					Uint8 blue    = static_cast<unsigned char>(
						               limit255(f0 * *pTex1++ +
									f1 * *pTex2++ +
									f2 * *pTex3++ +
									f3 * *pTex4++ +
									f4 * *pTex5++ +
									f5 * *pTex6++ )
									);



					*Idx++ = red;
					*Idx++ = green;
					*Idx++ = blue;
				}
			}

			
			pTexture = new Texture(pImage,(int)textureSize,(int)textureSize,(int)textureSize,0,true,false,false);
			m_Textures[combinedIndex] = pTexture; 
			delete[] pImage;	

		}

		if (Settings::GetInstance()->IsVerbose())
		{
			m_Logfile << "Finished Generating Texture" << std::endl;
		}


	}

	return pTexture;

}

void TerrainTextureFactory::UnloadTexture(int index)
{
	int latticeX, latticeY;
	m_pTerrain->GetLatticePosition(latticeX, latticeY);

	int combinedIndex = (latticeX << 24) | (latticeY << 16) | index;

	if (Settings::GetInstance()->IsVerbose())
	{
		m_Logfile << "TextureFactory:: Unload Texture" << std::endl;
	}

	Texture* pTexture = m_Textures[combinedIndex]; 
	delete pTexture;
	m_Textures[combinedIndex] = NULL;


}


}
