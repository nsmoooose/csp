// Lightmap.h: Schnittstelle für die Klasse CLightmap.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTMAP_H__430932AF_23C7_4C61_832B_A9BB404A7432__INCLUDED_)
#define AFX_LIGHTMAP_H__430932AF_23C7_4C61_832B_A9BB404A7432__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLightmap  
{
public:
	CLightmap();
	virtual ~CLightmap();

/*	static char* computeNormals(Image *hmap);
	static CTexture* genLightmap(char* normal,Image* hm,vector3 fSunDir,int w,float fAmbient);
	static int intersect_map(const vector3& iv,const ray& r,Image* hm,float fHeightScale);
*/};

#endif // !defined(AFX_LIGHTMAP_H__430932AF_23C7_4C61_832B_A9BB404A7432__INCLUDED_)
