///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   declaration of CQuadTexture
//   calculates a quad's texture
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//	8/13/2001	created file - zzed
//  9/19/2001	added code to change textures' resolution dynamically in
//				a background process - zzed
//
//
///////////////////////////////////////////////////////////////////////////

#if !defined(QUADTEXTURE_H)
#define QUADTEXTURE_H

#include "GLTexture.h"
#include "TextureZoom.h"


class CQuad;


class CQuadTexture : public CGLTexture
{
public:
	static uint m_uiCreateCount;

	CQuadTexture(CQuad *pQuad);
	virtual ~CQuadTexture();

	// called by CResourceManager
	virtual bool Create();

	// called by CQuad; (re)creates texture with specified size, 
	// returns false if size is equal to current texture, if not - true
	bool CreateTexture(ushort usSize);

	// used to bind texture to GL
	void Bind(GLenum eTarget);
private:
	CQuad		*m_pQuad;
	CTextureZoom *m_pLightTex;
	CTextureZoom *m_pLowTex;
	CTextureZoom *m_pHighTex;
	CTextureZoom *m_pHeightTex;
	bool		m_bGLHValid;    // is GL-handle valid?
	bool		m_bPValid;		// is pointer to texturedata valid?

	void Init();
	void CalcQuadTexture();
	void CreateHeightmapTexture();
	void LoadBuffer();
	void SaveBuffer();
};


#endif // QUADTEXTURE_H
