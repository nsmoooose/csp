///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   declaration of class CFrustum
//   used for frustum-culling
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	30/7/2001	created documentation - zzed
//
//
//
///////////////////////////////////////////////////////////////////////////

#if !defined(FRUSTUM_H)
#define FRUSTUM_H

#include "Vector.h"


class CFrustum  
{
public:
	/**
	 * precalculates internal matrix to speed up seperate frustum requests 
	 * !has to be called at the beginning of each frame!
	 */  
	void Update();
	/**
	 * returns true if point is inside frustum
	 */
	bool PointInFrustum(CVector vPos);
	/**
	 * returns true if quad is inside frustum
	 * @param vPos position of the quad's center
	 * @param fSize the quad's size of the outline
	 * @param fHeight the quad's height ... that's clear, isn't it? ;)
	 */
	bool QuadInFrustum(CVector vPos, float fSize, float fHeight);
	CFrustum();
	virtual ~CFrustum();
private:
	float m_fFrustum[6][4];
};

#endif 

