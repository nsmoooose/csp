/////////////////////////////////////////
// Texture.h - Texture mangement class //
/////////////////////////////////////////

#ifndef __Texture_h_
#define __Texture_h_

#define MAX_TEXTURE_NAME_LENGTH 64

class CTexture
{
public:
	int GetHeight();
	int GetWidth();
	void Lightmap();
	void Transparency();
	void RestoreState();
	void SaveState();
	void EnvironmentMapping();
	CTexture();
	~CTexture();

	// Create and load the files.
	bool LoadBMP(char* szFileName);
	// Buggy loading code (texture dimensions must be a mulipler of 4)
	bool LoadTransparentBMP(char* szFileName);
    
	void Toast();

	void Use();

private:
	// Generates the nesessary internal data.
	bool Create(char* szFileName);

	char m_szName[MAX_TEXTURE_NAME_LENGTH];
	unsigned int m_nID;

	// Status information.
	unsigned long int m_nWidth, m_nHeight;
};

#endif // __Texture_h_