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

#if !defined(MIPMAPQUAD_H)
#define MIPMAPQUAD_H

#include "TerrainData.h"


class CMipMapQuad  
{
public:
	float m_fHeight;
	bool m_bVisible;
	int m_iXPos;
	int m_iYPos;	
	float m_fSize;
	float m_fZPos;
	int CalcEpsilon(float fTao, float fNearClip, float fTopCoor, int iVRes);
	int RenderQuad(int iStepTop, int iStepBottom, int iStepLeft, int iStepRight, bool bLines);
	int m_iCurSteps;
	void SetResolution(float fD);
	CMipMapQuad(int iXPos, int iYPos, int iLength, int iMaxDiv, CTerrainData *pTerrainData);
	virtual ~CMipMapQuad();

private:
	/// lines or filled polygons?
	bool m_bLines;				
	/// minimum quad's distance to eye for each resolution
	float *m_afD;				
	/// lowest resolution
	int m_iMaxSteps;			
	// heightvalues and shades for quad
	CTerrainData *m_pTerrainData; 
	/**
	 * calculates the center z-value and the radius of the current quad (needed for frustum culling)
	 */

	void CalcMinMaxHeight();
	void DrawTriangle(int iX1, int iY1, int iX2, int iY2, int iX3, int iY3);
	int RenderInterpolation(float fStartX, float fStartY, float *pStart, int iStepX, int iStepY, float fNStartX, float fNStartY, 
		float *pNStart, int iNStepMult, float fEndX, float fEndY);
};

#endif 
