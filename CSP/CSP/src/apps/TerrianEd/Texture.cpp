///////////////////////////////////////////
// Texture.cpp - Texture mangement class //
///////////////////////////////////////////

#include "stdafx.h"
#include "Texture.h"
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

CTexture::CTexture()
{
	m_nID=0;
}

CTexture::~CTexture()
{
	Toast();
}

// Create is protected, since you're supposed to use the LoadBLAH() methods now.
bool CTexture::Create(char* szFileName)
{
	Toast();

	// Copy and terminate the filename.
	unsigned long int nszLen = strlen(szFileName);
	memcpy(m_szName, szFileName, nszLen);
	m_szName[nszLen]='\0';

	// Generate texture id.
	glGenTextures(1, &m_nID);
	m_nID++; // Use an offset of +1 to differentiate from non-initialized state.

	glBindTexture(GL_TEXTURE_2D, m_nID-1);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	return true;
}

void CTexture::Toast()
{
	if( m_nID )
	{
		m_nID--;
		glDeleteTextures(1, &m_nID);
		m_nID=0;
	}
}

void CTexture::Use()
{
	glBindTexture(GL_TEXTURE_2D, m_nID-1);
}

// Buggy loading code (texture dimensions must be a mulipler of 4)
bool CTexture::LoadTransparentBMP(char* szFileName)
{
	// Create the texture binding, and prepare additional info.
    Create(szFileName);

	GLubyte* colors; 
	int trans_color[3] = {0, 0, 0};
	
	FILE* bmpfile;
	BITMAPFILEHEADER bmfileheader;
	BITMAPINFOHEADER bminfoheader;
	RGBTRIPLE rgb; 

	int i,j,texpos = 0;

	bmpfile = fopen(szFileName, "rb");

	if (!bmpfile)
		exit(1);

	fread(&bmfileheader, sizeof(BITMAPFILEHEADER), 1, bmpfile);
	fread(&bminfoheader, sizeof(BITMAPINFOHEADER), 1, bmpfile);

	m_nWidth = bminfoheader.biWidth;
	m_nHeight = bminfoheader.biHeight;

	colors = (GLubyte*)malloc(m_nWidth * m_nHeight *4);

	// read color
	for (i = 0; i < (int) m_nWidth; i++)
	{	
		//start with the height because the first data you read in the color array of the bitmap
		for(j = m_nHeight - 1; j>=0 ; j--) 
		{ // is coord (1.0).
			fread(&rgb, sizeof(RGBTRIPLE),1,bmpfile);
			colors[texpos] = rgb.rgbtRed ;
			colors[texpos+1] = rgb.rgbtGreen;
			colors[texpos+2] = rgb.rgbtBlue;

			//test if color should be transparent
			if ((rgb.rgbtRed == trans_color[0]) && 
				(rgb.rgbtGreen == trans_color[1]) && (rgb.rgbtGreen == trans_color[2]))
				colors[texpos+3] = 0; 
			else
				colors[texpos+3] = 255;

		texpos += 4;
		}
	}
	fclose(bmpfile);

	// This is specific to the binary format of the data read in.
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 4, m_nWidth, m_nHeight, GL_RGBA, GL_UNSIGNED_BYTE, colors);

	if(colors)
		delete [] (BYTE *) colors;

	return true;
}

bool CTexture::LoadBMP(char* szFileName)
{
	// Create the texture binding, and prepare additional info.
    Create(szFileName);

	HANDLE hFileHandle;
	BITMAPINFO *pBitmapInfo = NULL;
	unsigned long lInfoSize = 0;
	unsigned long lBitSize = 0;
	BYTE *pBitmapData;

	// Open the Bitmap file
	hFileHandle = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ,
		NULL,OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN,NULL);

	// Check for open failure (most likely file does not exist).
	if(hFileHandle == INVALID_HANDLE_VALUE)
		return false;

	// File is Open. Read in bitmap header information
	BITMAPFILEHEADER	bitmapHeader;
	DWORD dwBytes;
	ReadFile(hFileHandle, &bitmapHeader, sizeof(BITMAPFILEHEADER),
		&dwBytes, NULL);

	__try {
		if(dwBytes != sizeof(BITMAPFILEHEADER))
			return false;

		// Check format of bitmap file
		if(bitmapHeader.bfType != 'MB')
			return false;

		// Read in bitmap information structure
		lInfoSize = bitmapHeader.bfOffBits - sizeof(BITMAPFILEHEADER);
		pBitmapInfo = (BITMAPINFO *) new BYTE[lInfoSize];

		ReadFile(hFileHandle, pBitmapInfo, lInfoSize, &dwBytes, NULL);

		if(dwBytes != lInfoSize)
			return false;

		m_nWidth = pBitmapInfo->bmiHeader.biWidth;
		m_nHeight = pBitmapInfo->bmiHeader.biHeight;
		lBitSize = pBitmapInfo->bmiHeader.biSizeImage;

		if(lBitSize == 0)
			lBitSize = (m_nWidth *
               pBitmapInfo->bmiHeader.biBitCount + 7) / 8 *
  			   abs(m_nHeight);

		// Allocate space for the actual bitmap
		pBitmapData = new BYTE[lBitSize];

		// Read in the bitmap bits
		ReadFile(hFileHandle,pBitmapData,lBitSize,&dwBytes,NULL);

		if(lBitSize != dwBytes)
			{
			if(pBitmapData)
				delete [] (BYTE *) pBitmapData;
			pBitmapData = NULL;

			return false;
			}
		}
	__finally // Fail or success, close file and free working memory
		{
		CloseHandle(hFileHandle);

		if(pBitmapInfo != NULL)
			delete [] (BYTE *)pBitmapInfo;
		}


	// This is specific to the binary format of the data read in.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);

    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, m_nWidth, m_nHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBitmapData);

	if(pBitmapData)
		delete [] (BYTE *) pBitmapData;

	return true;
}

void CTexture::EnvironmentMapping()
{
	// Set texture state to environment mapping

	// Set up the spheric coordinate generation
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, 
		// GL_EYE_LINEAR	// completely flat projector type projection from eye.
		// GL_OBJECT_LINEAR // like a normal fixed texture
		GL_SPHERE_MAP       // mapping relative to eye.  center of texture points toward viewer.
		);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, 
		// GL_EYE_LINEAR	// completely flat projector type projection from eye.
		// GL_OBJECT_LINEAR // like a normal fixed texture
		GL_SPHERE_MAP       // mapping relative to eye.  center of texture points toward viewer.
		);

	// Turn on the coordinate generate features for environment mapping
	glEnable(GL_TEXTURE_GEN_S); glEnable(GL_TEXTURE_GEN_T);
}

void CTexture::SaveState()
{
	// State should be saved before applying any texture effetcs
	glPushAttrib(GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void CTexture::RestoreState()
{
	// Restore state	
	glPopAttrib();
}

void CTexture::Transparency()
{
	// Enable transparency

	// Enable blending
	glEnable(GL_BLEND);

	glBlendFunc(
		// GL_SRC_COLOR,	GL_DST_COLOR        // opaque envmap texture
		// GL_ONE_MINUS_SRC_ALPHA,GL_SRC_ALPHA	// only original texture
		// GL_ONE_MINUS_SRC_COLOR,GL_DST_COLOR  // opaque target
		// GL_DST_COLOR,GL_ONE_MINUS_SRC_COLOR	// as good as it gets
		// GL_ONE, GL_ONE_MINUS_SRC_ALPHA	    // unreal env (fog)
		// GL_DST_COLOR, GL_SRC_COLOR		    // unreal lightmap
		GL_ONE, GL_ONE_MINUS_SRC_COLOR			// unreal translucent
		);
}

void CTexture::Lightmap()
{
	// Enable lightmapping

	// Enable blending
	glEnable(GL_BLEND);

	glBlendFunc(
		// GL_SRC_COLOR,	GL_DST_COLOR        // opaque envmap texture
		// GL_ONE_MINUS_SRC_ALPHA,GL_SRC_ALPHA	// only original texture
		// GL_ONE_MINUS_SRC_COLOR,GL_DST_COLOR  // opaque target
		// GL_DST_COLOR,GL_ONE_MINUS_SRC_COLOR	// as good as it gets
		// GL_ONE, GL_ONE_MINUS_SRC_ALPHA	    // unreal env (fog)
		// GL_ONE, GL_ONE_MINUS_SRC_COLOR		// unreal translucent
		GL_DST_COLOR, GL_SRC_COLOR				// unreal lightmap
		);
}

int CTexture::GetWidth()
{
	return m_nWidth;
}

int CTexture::GetHeight()
{
	return m_nHeight;
}
