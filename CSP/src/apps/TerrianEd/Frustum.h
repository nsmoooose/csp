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

#if !defined(FRUSTUM_H)
#define FRUSTUM_H


class CFrustum  
{
public:
	void Update();
	bool PointInFrustum(float fPosX, float fPosY, float fPosZ);
	bool QuadInFrustum(float fPosX, float fPosY, float fPosZ, float fSize, float fHeight);
	CFrustum();
	virtual ~CFrustum();
private:
	float m_fFrustum[6][4];
};

#endif 