///////////////////////////////////////////////////////////////////////////
//
//   test of a terrain-engine based on GeoMipMapping
//
//   coded by zzed and Stormbringer
//   email:   zzed@gmx.li    storm_bringer@gmx.li 
//
//   7.2.2001
//
///////////////////////////////////////////////////////////////////////////

#if !defined(HEIGHTMAP_H)
#define HEIGHTMAP_H



class CTerrainData
{
public:
	void CalcVertexShade();
	CTerrainData();
	unsigned char*	m_acShades;
	float*	m_afHeightmap;
	int		m_iWidth;
	int		m_iHeight;
	CTerrainData(char* sFilename, bool b16bit);
	virtual ~CTerrainData();
private:
	void CalcTriangleNormal(float afNormal[3], float afVec1[3], float afVec2[3], float afVec3[3]);
	void CalcRectNormal(float afNormal[3], int iXPos, int iYPos);
};

#endif 
