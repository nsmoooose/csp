///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   declaration of CQuad
//   encapsulates one part of the terrain - a single quad
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	7/30/2001	updated documentation - zzed
//  9/19/2001	added code to change textures' resolution dynamically in
//				a background process - zzed
//
//
///////////////////////////////////////////////////////////////////////////

#if !defined(MIPMAPQUAD_H)
#define MIPMAPQUAD_H

#pragma warning( disable : 4786 )	// disable warning about truncated characters in debug-info

#include <map>
#include <stack>
#include <queue>

#include "Vector.h"
#include "QuadTexture.h"
#include "ResourceManager.h"
#include "global.h"

class CTerrainData;


class CQuad  
{
public:
	class CQuadRenderer
	{
	public:
		virtual void DrawTriangle(int iX1, int iY1, int iX2, int iY2, int iX3, int iY3, CQuad *pQuad) = 0;
	};

	class CQuadSmoothRenderer : CQuadRenderer
	{
	public:
		virtual void DrawTriangle(int iX1, int iY1, int iX2, int iY2, int iX3, int iY3, CQuad *pQuad);
	};

	class CQuadWireframeRenderer : CQuadRenderer
	{
	public:
		virtual void DrawTriangle(int iX1, int iY1, int iX2, int iY2, int iX3, int iY3, CQuad *pQuad);
	};

	class CQuadMTexRenderer : CQuadRenderer
	{
	public:
		virtual void DrawTriangle(int iX1, int iY1, int iX2, int iY2, int iX3, int iY3, CQuad *pQuad);
	};

public:
	/// "z-radius" of the current quad (\f$\frac{\mathrm{highest z-value} - \mathrm{lowest z-value}}{2}\f$)
	float m_fHeight;			
	/// is this quad visible?
	bool m_bVisible;			
	/// x-value of the current position (one unit=one triangle at highest resolution)
	/// y-value and center z-value of quad for frustum culling
	CVector m_vPos;
	/// quad's length/2, could be understood as "radius" (frustum culling!)
	float m_fSize;								
	/// current resolution, specified in "steps"
	uint m_uiCurSteps;
	/// contains heightvalues
	float*	m_pfHeightmap;
	/// squared distance from camera
	float m_fDistSqrd;
	/// width of current quad in heightpoint-units
	uint m_uiWidth;
	/// pointers to the neighboring quad's resolution (m_iCurSteps) - left, top, right, bottom
	uint *m_pNeighbors[4];
	/// static member to represent highest possible quad-resolution (=1!)
	static uint m_uiMaxSteps;
	/// static member to record count of created gl-textures per frame
	static uint m_uiTexCreated;

	/// Revises current quad-data and decides whether change the resolution (either quad or heightdata)
	bool CheckData();
	
	/// pointer to complete terrain-data (needed to access neighboring quads, etc.)
	CTerrainData *m_pData;

	/**
	 * calculates the minimum distance of each quad's resolution
	 * @param fTao maximum pixel error of each point's z-value (referring to the next higher resolution)
	 * @param fNearClip distance of near clipping plane from eye
	 * @param fTopCoor don't know what it is :-)
	 * @param iVRes amount of horizontal lines on OpenGL screen
	 */
	int	CalcEpsilon(float fTao, float fNearClip, float fTopCoor, int iVRes);
	/** 
	 * Renders the current quad via OpenGL
	 * @param iStepTop specifies the resolution of the upper quad in "steps"
	 * @param iStepBottom specifies the resolution of the lower quad
	 * @param iStepLeft specifies the resolution of the left quad
	 * @param iStepRight specifies the resolution of the right quad
	 * @param bLines should the quad be displayed with lines or filled polygons?
	 */
	int	RenderQuad();
	/**
	 * calculates the proper resolution of the quad via its distance to the viewer
	 * @param fD distance of quad to eye
	 */
	void SetResolution(float fD);

	/**
	 * Loads the heightmap of a quad from the SCN file
	 *
	 * Das SCN file muﬂ geˆffnet sein, speicher wird von der funktion selbst allokiert
	 * => irgendwo muﬂ er auch wieder freigegeben werden, z.b. im destruktor (hab ich aber nicht drin)
	 * Die breite des quads wird aus der config gelesen => nach dem generieren der map dort eintragen!!!
	 * auﬂerdem brauchen wir die breite der map in quads. muﬂ auch irgendwo gespeichert werden.
	 */
	bool LoadQuadData();

	/**
	 * Creates a new quad and calculates "epsilon"
	 * @param iXPos x-value of the quad's center (!) inside the whole terrain (one unit is the distance between two neighboring height points)
	 * @param iYPos y-value of the quad's center
	 * @param pTerrainData specifies the heightvalues and shades for the current quad
	 */
	CQuad(int iXPos, int iYPos, CTerrainData *pData);
	/**
	 * Deletes all reserved memory
	 */
	virtual ~CQuad();

private:
	/// minimum quad's distance to eye for each resolution
	float *m_afD;	
	
	CQuadTexture *m_pTexture;
	uint m_hGLTextureWater;
	CGLTexture *m_pTextureWater;

	/**
	 * calculates the center z-value and the radius of the current quad (needed for frustum culling)
	 */
	void CalcMinMaxHeight();
	/**
	 * Does the essential crack-resolving, and is layed out to accomplish this task on all four sides of a quad. 
	 * Please refer to the GeoMipMapping paper for further explanation of the algorithm.
	 * @param fStartX x-value of the inner point to start crack-resolving from (in the picture: 1)
	 * @param fStartY y-value (in the picture: 7)
	 * @param pStart pointer to the heightvalues of fStart in the array
	 * @param iStepX how many points distance between the triangles from the inner line? depends on the quad's resolution. (in the picture: 1)
	 * @param iStepY same as iStepX, only y-value (in the picture: 0)
	 * @param fNStartX x-value of the outer starting-point (in the picture: 0)
	 * @param fNStartY y-value (in the picture: 8)
	 * @param pNStart pointe rto the hieghtvalues of fNStart in the array
	 * @param iNStepMult ratio of iStep to iNStep (in the picture: 2)
	 * @param fEndX x-value of the inner end-point (in the picture: 7)
	 * @param fEndY y-value of the inner end-point (in the picture: 7)
	 * <br>
	 * <img src="../QuadInterpolation.gif">
	 */
	int RenderInterpolation(int iStartX, int iStartY, float *pStart, int iStepX, int iStepY, int iNStartX, int iNStartY, 
		float *pNStart, int iNStepMult, int iEndX, int iEndY);
	void CreateTexture();
};



#endif 

